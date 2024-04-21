defmodule JackParser.Program do
  import NimbleParsec
  import JackParser.Statement
  import JackParser.Helper

  whitespace = fn x -> JackParser.Helper.whitespace(x) end
  sorround = fn x, y, z -> JackParser.Helper.sorround(x, y, z) end
  semi_colon = JackParser.Helper.semi_colon()

  type =
    choice([
      string("int") |> unwrap_and_tag(:keyword),
      string("char") |> unwrap_and_tag(:keyword),
      string("boolean") |> unwrap_and_tag(:keyword),
      parsec(:identifier)
    ])

  defcombinator(
    :variable_declaration,
    whitespace.(string("var"))
    |> unwrap_and_tag(:keyword)
    |> concat(whitespace.(type))
    |> concat(whitespace.(parsec(:identifier)))
    |> repeat(
      whitespace.(string(","))
      |> concat(whitespace.(parsec(:identifier)))
    )
    |> concat(semi_colon)
    |> tag(:variable_declaration)
  )

  defcombinator(
    :class_variable_declaration,
    whitespace.(
      choice([
        string("static"),
        string("field")
      ])
      |> unwrap_and_tag(:keyword)
    )
    |> concat(whitespace.(type))
    |> concat(whitespace.(parsec(:identifier)))
    |> repeat(
      whitespace.(string(","))
      |> concat(whitespace.(parsec(:identifier)))
    )
    |> concat(semi_colon)
    |> tag(:variable_declaration)
  )

  defcombinator(
    :parameter_list,
    optional(
      whitespace.(parsec(:identifier))
      |> concat(whitespace.(parsec(:identifier)))
      |> repeat(
        whitespace.(string(","))
        |> concat(whitespace.(parsec(:identifier)))
        |> concat(whitespace.(parsec(:identifier)))
      )
    )
    |> tag(:parameter_list)
  )

  defcombinator(
    :subroutine_body,
    sorround.(
      repeat(parsec(:variable_declaration))
      |> concat(parsec(:statements)),
      "{",
      "}"
    )
    |> tag(:subroutine_body)
  )

  defcombinator(
    :subroutine_declaration,
    whitespace.(
      choice([
        string("constructor"),
        string("function"),
        string("method")
      ])
      |> unwrap_and_tag(:keyword)
    )
    |> concat(
      whitespace.(
        choice([
          string("void") |> unwrap_and_tag(:keyword),
          type
        ])
      )
    )
    |> concat(whitespace.(parsec(:identifier)))
    |> concat(sorround.(parsec(:parameter_list), "(", ")"))
    |> concat(parsec(:subroutine_body))
    |> tag(:subroutine_declaration)
  )

  defcombinator(
    :class,
    whitespace.(string("class"))
    |> unwrap_and_tag(:keyword)
    |> concat(whitespace.(parsec(:identifier)))
    |> concat(
      sorround.(
        repeat(parsec(:class_variable_declaration))
        |> repeat(parsec(:subroutine_declaration)),
        "{",
        "}"
      )
    )
    |> tag(:class_declaration)
  )

  defparsec(:parse, parsec(:class))
end
