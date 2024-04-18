defmodule JackParser.Expression do
  import NimbleParsec

  whitespace_ = ascii_char([32, 9, 10])

  whitespace = fn combinator ->
    ignore(repeat(whitespace_)) |> concat(combinator) |> ignore(repeat(whitespace_))
  end

  sorround = fn between, open, close ->
    whitespace.(string(open))
    |> tag(:symbol)
    |> concat(between)
    |> concat(whitespace.(string(close)) |> tag(:symbol))
  end

  numberConstant = integer(min: 1) |> tag(:integerConstant)

  identifier = ascii_string([?a..?z], min: 1) |> tag(:identifier)

  stringConstant =
    ignore(choice([ascii_char([22, 27]), string("\""), string("\'")]))
    |> ascii_string([?a..?z], min: 1)
    |> ignore(choice([ascii_char([22, 27]), string("\""), string("\'")]))
    |> tag(:stringConstant)

  keywordConstant =
    Enum.map(["true", "false", "null", "this"], fn x -> string(x) end)
    |> choice
    |> tag(:keywordConstant)

  operations =
    Enum.map(["+", "-", "*", "/", "&", "|", "<", ">", "="], fn x -> string(x) end)
    |> choice
    |> tag(:symbol)

  unary_operations =
    Enum.map(["-", "~"], fn x -> string(x) end)
    |> choice
    |> tag(:symbol)

  sub_rountine_call =
    choice([
      whitespace.(identifier) |> concat(sorround.(parsec(:expression_list), "(", ")")),
      whitespace.(identifier)
      |> concat(whitespace.(string(".")))
      |> concat(whitespace.(identifier) |> concat(sorround.(parsec(:expression_list), "(", ")")))
    ])

  defcombinatorp(
    :expression_list,
    choice([
      parsec(:expression)
      |> repeat(string(",") |> parsec(:expression)),
      empty()
    ])
    |> tag(:expressionList)
  )

  defcombinatorp(
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

  defcombinatorp(
    :expression,
    parsec(:term)
    |> repeat(operations |> parsec(:term))
    |> tag(:expression)
  )

  defparsec(:expression, parsec(:expression))
end

defmodule JackParser do
  def main(arg \\ []) do
    if arg == [] do
      IO.puts("Usage: jack_parser <filename>")
      System.halt(1)
    end

    case File.read(arg) do
      {:ok, content} ->
        IO.inspect(JackParser.Expression.expression(content))

      {:error, reason} ->
        IO.puts("Error: #{reason}")
    end
  end
end
