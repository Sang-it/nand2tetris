defmodule JackParser.Expression do
  import NimbleParsec
  import JackParser.Helper

  whitespace = fn x -> JackParser.Helper.whitespace(x) end
  sorround = fn x, y, z -> JackParser.Helper.sorround(x, y, z) end
  sub_rountine_call = JackParser.Helper.sub_rountine_call()

  defcombinator(
    :expression_list,
    optional(
      parsec(:expression)
      |> repeat(string(",") |> parsec(:expression))
    )
    |> tag(:expression_list)
  )

  defcombinator(
    :term,
    choice([
      whitespace.(parsec(:identifier)) |> concat(sorround.(parsec(:expression), "[", "]")),
      whitespace.(sub_rountine_call),
      whitespace.(parsec(:keyword_constant)),
      whitespace.(parsec(:number_constant)),
      whitespace.(parsec(:string_constant)),
      whitespace.(parsec(:unary_operation)) |> parsec(:term),
      sorround.(parsec(:expression), "(", ")"),
      whitespace.(parsec(:identifier))
    ])
    |> tag(:term)
  )

  defcombinator(
    :expression,
    parsec(:term)
    |> repeat(parsec(:operation) |> parsec(:term))
    |> tag(:expression)
  )
end
