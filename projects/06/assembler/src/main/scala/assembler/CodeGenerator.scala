package assembler

import java.io.File
import java.nio.charset.StandardCharsets
import java.io.Writer
import java.io.PrintWriter

class CodeGenerator(
    private val outWriter: Writer,
    private val symbolTable: SymbolTable
) {

  def write(instruction: I.RealInstruction): Unit = {
    val res = instruction match {
      case I.AInstruction(value) => handleAInstruction(value)
      case I.CInstruction(dest, comp, jump) =>
        s"111${comp.machineCode}${dest.machineCode}${jump.machineCode}"
    }
    outWriter.write(s"$res\n")
  }

  private def handleAInstruction(value: Either[Int, String]): String = {
    val decimalValue = value.fold(identity, resolveSymbol)
    val binaryValue = decimalValue.toBinaryString
    val paddedBinaryValue = padZeros(binaryValue)
    s"0$paddedBinaryValue"
  }

  private def resolveSymbol(symbolName: String): Int = {
    symbolTable.get(symbolName) match {
      case Some(value) => value
      case None =>
        val varAddress = symbolTable.getNextVarAddress()
        symbolTable.add(symbolName, varAddress)
        varAddress
    }
  }

  private def padZeros(binaryValue: String): String = {
    val padLength = 15 - binaryValue.length
    "0" * padLength + binaryValue
  }

  def close(): Unit = {
    outWriter.close()
  }
}
object CodeGenerator {

  def apply(outputFile: File, symbolTable: SymbolTable): CodeGenerator = {
    val outWriter = new PrintWriter(outputFile, StandardCharsets.UTF_8.name)
    new CodeGenerator(outWriter, symbolTable)
  }

  def apply(outWriter: Writer, symbolTable: SymbolTable): CodeGenerator = {
    new CodeGenerator(outWriter, symbolTable)
  }

  def apply(symbolTable: SymbolTable): CodeGenerator = {
    val outWriter = new PrintWriter(System.out)
    new CodeGenerator(outWriter, symbolTable)
  }
}
