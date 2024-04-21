defmodule JackParser.Statement do
  import NimbleParsec
  import JackParser.Helper
  import JackParser.Expression

  # identifier = JackParser.Helper.identifier()
  sub_rountine_call = JackParser.Helper.sub_rountine_call()
  whitespace = fn x -> JackParser.Helper.whitespace(x) end
  sorround = fn x, y, z -> JackParser.Helper.sorround(x, y, z) end
  semi_colon = JackParser.Helper.semi_colon()

  defcombinator(
    :if_statement,
    whitespace.(string("if"))
    |> unwrap_and_tag(:keyword)
    |> concat(sorround.(parsec(:expression), "(", ")"))
    |> concat(sorround.(parsec(:statements), "{", "}"))
    |> optional(
      whitespace.(string("else"))
      |> concat(sorround.(parsec(:statements), "{", "}"))
    )
  )

  defcombinator(
    :while_statement,
    whitespace.(string("while"))
    |> unwrap_and_tag(:keyword)
    |> concat(sorround.(parsec(:expression), "(", ")"))
    |> concat(sorround.(parsec(:statements), "{", "}"))
    |> tag(:while_statement)
  )

  defcombinator(
    :do_statement,
    whitespace.(string("do"))
    |> unwrap_and_tag(:keyword)
    |> concat(sub_rountine_call)
    |> concat(semi_colon)
    |> tag(:do_statement)
  )

  defcombinator(
    :let_statement,
    whitespace.(string("let"))
    |> unwrap_and_tag(:keyword)
    |> choice([
      whitespace.(parsec(:identifier)) |> concat(sorround.(parsec(:expression), "[", "]")),
      whitespace.(parsec(:identifier))
    ])
    |> concat(whitespace.(string("=")) |> unwrap_and_tag(:symbol))
    |> concat(parsec(:expression))
    |> concat(semi_colon)
    |> tag(:let_statement)
  )

  defcombinator(
    :return_statement,
    whitespace.(string("return"))
    |> unwrap_and_tag(:keyword)
    |> optional(parsec(:expression))
    |> concat(semi_colon)
    |> tag(:return_statement)
  )

  defcombinator(
    :statement,
    choice([
      parsec(:let_statement),
      parsec(:return_statement),
      parsec(:do_statement),
      parsec(:while_statement),
      parsec(:if_statement)
    ])
  )

  defcombinator(
    :statements,
    repeat(parsec(:statement))
    |> tag(:statements)
  )
end
