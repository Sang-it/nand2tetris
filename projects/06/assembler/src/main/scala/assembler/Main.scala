package assembler

import scala.io.Source
import java.io.File
import java.io.PrintWriter
import java.nio.charset.StandardCharsets
import java.io.BufferedReader
import java.io.FileReader
import java.nio.file.Files
import java.nio.file.Path
import java.nio.file.Paths
import java.io.RandomAccessFile

object Main {

  def main(args: Array[String]): Unit = {

    handleArgs(args) match {
      case Some((inputFile, outputFile)) =>
        val symbolTable = new SymbolTable()
        val parser = Parser(inputFile, symbolTable)
        val codeGenerator = CodeGenerator(outputFile, symbolTable)

        var instruction: Option[I.RealInstruction] = null
        while ({ instruction = parser.next(); instruction.isDefined }) {
          codeGenerator.write(instruction.get)
        }

        parser.close()
        codeGenerator.close()

      case None =>
        println("Exiting program.")
    }

  }

  private def handleArgs(args: Array[String]): Option[(File, File)] = {
    args.headOption
      .flatMap { fileName =>
        if (
          fileName == null || fileName.trim.isEmpty || fileName.length <= 4 || !fileName
            .endsWith(".asm")
        ) {
          println(
            "Invalid file name or extension! File name must have a '.asm' extension."
          )
          None
        } else {
          val inputFile = new File(fileName)
          val outputFile =
            new File(inputFile.getParent, fileName.dropRight(4) + ".hack")
          Some((inputFile, outputFile))
        }
      }
      .orElse {
        println("You must provide a file name!")
        None
      }
  }

}
