defmodule JackParser.Expression do
  import NimbleParsec

  whitespace = fn x -> JackParser.Helper.whitespace(x) end
  numberConstant = JackParser.Helper.numberConstant()
  sorround = fn x, y, z -> JackParser.Helper.sorround(x, y, z) end
  identifier = JackParser.Helper.identifier()
  stringConstant = JackParser.Helper.stringConstant()
  keywordConstant = JackParser.Helper.keywordConstant()
  operations = JackParser.Helper.operations()
  unary_operations = JackParser.Helper.unary_operations()
  sub_rountine_call = JackParser.Helper.sub_rountine_call()

  defcombinator(
    :expression_list,
    choice([
      parsec(:expression)
      |> repeat(string(",") |> parsec(:expression)),
      empty()
    ])
    |> tag(:expressionList)
  )

  defcombinator(
    :term,
    choice([
      whitespace.(identifier) |> concat(sorround.(parsec(:expression), "[", "]")),
      whitespace.(sub_rountine_call),
      whitespace.(identifier),
      whitespace.(keywordConstant),
      whitespace.(numberConstant),
      whitespace.(stringConstant),
      whitespace.(unary_operations) |> parsec(:term) |> tag(:unaryOpTerm),
      sorround.(parsec(:expression), "(", ")")
    ])
  )

  defcombinator(
    :expression,
    parsec(:term)
    |> repeat(operations |> parsec(:term))
    |> tag(:expression)
  )

  def parse do
    parsec(:expression)
  end
end
