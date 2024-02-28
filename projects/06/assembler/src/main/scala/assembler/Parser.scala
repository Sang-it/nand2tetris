package assembler

import java.io.BufferedReader
import java.io.ByteArrayInputStream
import java.io.File
import java.io.InputStreamReader
import java.nio.file.Files
import scala.ValueOf

import fastparse._, NoWhitespace._

object I {

  sealed trait Instruction
  sealed trait RealInstruction extends Instruction

  case class SymbolInstruction(symbol: String) extends Instruction

  case class AInstruction(constOrSymbol: Either[Int, String])
      extends RealInstruction
  object AInstruction {
    def apply(const: Int): AInstruction = AInstruction(Left(const))
    def apply(symbol: String): AInstruction = AInstruction(Right(symbol))
  }

  case class Destination(expr: String, machineCode: String)
  case class Computation(expr: String, machineCode: String)
  case class Jump(expr: String, machineCode: String)

  case class CInstruction(dest: Destination, comp: Computation, jump: Jump)
      extends RealInstruction

  object CInstruction {
    val destinationsMap = Map(
      ("" -> "000"),
      ("M" -> "001"),
      ("D" -> "010"),
      ("MD" -> "011"),
      ("A" -> "100"),
      ("AM" -> "101"),
      ("AD" -> "110"),
      ("AMD" -> "111")
    )

    val computationsMapNotA = Map(
      ("0" -> "101010"),
      ("1" -> "111111"),
      ("-1" -> "111010"),
      ("D" -> "001100"),
      ("A" -> "110000"),
      ("!D" -> "001101"),
      ("!A" -> "110001"),
      ("-D" -> "001111"),
      ("-A" -> "110011"),
      ("D+1" -> "011111"),
      ("A+1" -> "110111"),
      ("D-1" -> "001110"),
      ("A-1" -> "110010"),
      ("D+A" -> "000010"),
      ("D-A" -> "010011"),
      ("A-D" -> "000111"),
      ("D&A" -> "000000"),
      ("D|A" -> "010101")
    )
    val computationsMapA = Map(
      ("M" -> "110000"),
      ("!M" -> "110001"),
      ("-M" -> "110011"),
      ("M+1" -> "110111"),
      ("M-1" -> "110010"),
      ("D+M" -> "000010"),
      ("D-M" -> "010011"),
      ("M-D" -> "000111"),
      ("D&M" -> "000000"),
      ("D|M" -> "010101")
    )

    val jumpsMap = Map(
      ("" -> "000"),
      ("JGT" -> "001"),
      ("JEQ" -> "010"),
      ("JGE" -> "011"),
      ("JLT" -> "100"),
      ("JNE" -> "101"),
      ("JLE" -> "110"),
      ("JMP" -> "111")
    )

    def destFromExpression(expr: String): Destination =
      Destination(expr, destinationsMap(expr))

    def compFromExpression(expr: String): Computation = {
      computationsMapNotA.get(expr) match {
        case Some(v) => Computation(expr, "0" + v) // a=0
        case None =>
          computationsMapA.get(expr) match {
            case Some(v) => Computation(expr, "1" + v) // a=1
            case None =>
              throw new RuntimeException("Expression not allowed: " + expr)
          }
      }
    }

    def jumpFromExpression(expr: String): Jump = Jump(expr, jumpsMap(expr))

  }
}

/** The parser.
  * @param br1
  *   BR for first pass, picking labels
  * @param br2
  *   BR for second pass, parsing
  */
class Parser(
    br1: BufferedReader,
    br2: BufferedReader,
    symbolTable: SymbolTable
) {
  import Parser._

  // FIRST PASS
  buildSymbolTable()

  // SECOND PASS, iterated from main
  // TODO handle FAILURE CASE when parsing!
  def next(): Option[I.RealInstruction] = {
    Option(br2.readLine()) match {
      case None => { // end of input
        br1.close()
        None
      }
      case Some(line) =>
        parse(line, FinalP).get.value match {
          case Some(i: I.RealInstruction) => Option(i)
          case _                          => next() // if not A or C, skip
        }
    }
  }

  private def buildSymbolTable(): Unit = {
    var line: String = null
    var realInstructionsCounter = 0
    try {
      while ({ line = br1.readLine(); line != null }) {
        parse(line, FinalP).get.value match {
          case Some(I.SymbolInstruction(symbol)) =>
            symbolTable.add(symbol, realInstructionsCounter)
          case Some(_) => realInstructionsCounter += 1
          case _       => // if empty line, or just a comment
        }
      }
    } finally {
      br1.close()
    }
  }

  def close(): Unit = {
    br2.close()
  }
}

object Parser {

  def apply(inputFile: File, symbolTable: SymbolTable): Parser = {
    val br1, br2 = Files.newBufferedReader(inputFile.toPath())
    new Parser(br1, br2, symbolTable)
  }

  def apply(inputString: String, symbolTable: SymbolTable): Parser = {
    val br1, br2 = new BufferedReader(
      new InputStreamReader(new ByteArrayInputStream(inputString.getBytes))
    )
    new Parser(br1, br2, symbolTable)
  }

  def Letter[$: P] = P(CharPred(_.isLetter))
  def Digit[$: P] = P(CharPred(_.isDigit))
  def SymFirst[$: P] = P(
    Letter | "." | "_" | "$" | ":"
  )

  def Symbol[$: P] = P(SymFirst ~ (SymFirst | Digit).rep)

  def Newline[$: P] = P(StringIn("\r\n", "\n"))
  def Whitespace[$: P] = P(" " | "\t")
  def Comment[$: P] = P("//" ~ AnyChar.rep ~ End)

  def Number[$: P]: P[Int] =
    Digit.rep(min = 1).!.map(_.toInt)

  def SymbolInstruction[$: P] =
    P("(" ~ Symbol.! ~ ")").map(s => I.SymbolInstruction(s))

  def AInstruction[$: P] = P(
    "@" ~ (Symbol.!.map(I.AInstruction.apply) | Number.map(
      I.AInstruction.apply
    ))
  )

  def dest[$: P] =
    StringIn(
      "",
      "M",
      "D",
      "MD",
      "A",
      "AM",
      "AD",
      "AMD"
    ).!.map(
      I.CInstruction.destFromExpression
    )
  def comp[$: P] =
    StringIn(
      "0",
      "1",
      "-1",
      "D",
      "A",
      "!D",
      "!A",
      "-D",
      "-A",
      "D+1",
      "A+1",
      "D-1",
      "A-1",
      "D+A",
      "D-A",
      "A-D",
      "D&A",
      "D|A",
      "M",
      "!M",
      "-M",
      "M+1",
      "M-1",
      "D+M",
      "D-M",
      "M-D",
      "D&M",
      "D|M"
    ).!.map(
      I.CInstruction.compFromExpression
    )
  def jump[$: P] = StringIn(
    "",
    "JGT",
    "JEQ",
    "JGE",
    "JLT",
    "JNE",
    "JLE",
    "JMP"
  ).!.map(
    I.CInstruction.jumpFromExpression
  )

  def CInstruction[$: P] = ((dest ~ "=").? ~ comp ~ (";" ~ jump).?).map {
    case (d, computation, j) =>
      val destination = d.getOrElse(I.CInstruction.destFromExpression(""))
      val jump = j.getOrElse(I.CInstruction.jumpFromExpression(""))
      I.CInstruction(destination, computation, jump)
  }

  def Instruction[$: P]: P[I.Instruction] =
    SymbolInstruction | AInstruction | CInstruction
  def FinalP[$: P] = Whitespace.rep ~ Instruction.? ~ Whitespace.rep ~ Comment.?
}
