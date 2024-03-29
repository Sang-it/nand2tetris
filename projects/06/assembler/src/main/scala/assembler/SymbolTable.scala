package assembler

class SymbolTable {
  private val registers = (0 to 15).map(n => ("R" + n) -> n)
  private var symbols = Map(
    ("SP" -> 0),
    ("LCL" -> 1),
    ("ARG" -> 2),
    ("THIS" -> 3),
    ("THAT" -> 4),
    ("SCREEN" -> 16384),
    ("KBD" -> 24576)
  )
  symbols ++= registers

  private var nextVariableAddress = 15

  def add(symbolName: String, address: Int): Unit = {
    if (get(symbolName).isDefined) {
      throw new RuntimeException("Symbol already defined: " + symbolName)
    }
    symbols += symbolName -> address
  }

  def get(symbolName: String): Option[Int] = symbols.get(symbolName)

  def getNextVarAddress(): Int = {
    nextVariableAddress += 1
    nextVariableAddress
  }
}
