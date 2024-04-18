defmodule JackParser.Statement do
  import NimbleParsec
  import JackParser.Expression

  expression = JackParser.Expression.parse()
  identifier = JackParser.Helper.identifier()
  sub_rountine_call = JackParser.Helper.sub_rountine_call()
  whitespace = fn x -> JackParser.Helper.whitespace(x) end
  sorround = fn x, y, z -> JackParser.Helper.sorround(x, y, z) end

  defcombinatorp(
    :if_statement,
    whitespace.(string("if"))
    |> concat(sorround.(expression, "(", ")"))
    |> concat(sorround.(parsec(:statements), "{", "}"))
    |> choice([
      whitespace.(string("else"))
      |> concat(sorround.(parsec(:statements), "{", "}")),
      empty()
    ])
    |> tag(:if_statement)
  )

  defcombinatorp(
    :while_statement,
    whitespace.(string("while"))
    |> concat(sorround.(expression, "(", ")"))
    |> concat(sorround.(parsec(:statements), "{", "}"))
    |> tag(:while_statement)
  )

  defcombinatorp(
    :do_statement,
    whitespace.(string("do"))
    |> concat(sub_rountine_call)
    |> concat(whitespace.(string(";")))
    |> tag(:do_statement)
  )

  defcombinatorp(
    :let_statement,
    whitespace.(string("let"))
    |> choice([
      whitespace.(identifier) |> concat(sorround.(expression, "[", "]")),
      whitespace.(identifier)
    ])
    |> concat(whitespace.(string("=")))
    |> concat(expression)
    |> concat(whitespace.(string(";")))
    |> tag(:let_statement)
  )

  defcombinatorp(
    :return_statement,
    whitespace.(string("return"))
    |> choice([
      expression,
      empty()
    ])
    |> concat(whitespace.(string(";")))
    |> tag(:return_statement)
  )

  defcombinatorp(
    :statement,
    choice([
      parsec(:let_statement),
      parsec(:return_statement),
      parsec(:do_statement),
      parsec(:while_statement),
      parsec(:if_statement)
    ])
  )

  defcombinatorp(
    :statements,
    repeat(parsec(:statement))
  )

  defparsec(
    :parse,
    parsec(:statements)
  )
end
