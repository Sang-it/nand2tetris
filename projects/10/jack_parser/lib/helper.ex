defmodule JackParser.Helper do
  import NimbleParsec

  def whitespace(combinator) do
    whitespace_ = ascii_char([32, 9, 10])
    ignore(repeat(whitespace_)) |> concat(combinator) |> ignore(repeat(whitespace_))
  end

  def sorround(between, open, close) do
    whitespace(string(open))
    |> unwrap_and_tag(:symbol)
    |> concat(between)
    |> concat(whitespace(string(close)) |> unwrap_and_tag(:symbol))
  end

  defcombinator(
    :number_constant,
    integer(min: 1)
    |> unwrap_and_tag(:integer_constant)
  )

  defcombinator(
    :identifier,
    ascii_string([?a..?z, ?A..?Z], min: 1)
    |> unwrap_and_tag(:identifier)
  )

  defp not_quote(<<?", _::binary>>, context, _, _), do: {:halt, context}
  defp not_quote(_, context, _, _), do: {:cont, context}

  defcombinator(
    :string_constant,
    ignore(string("\""))
    |> repeat_while(ascii_char([]), {:not_quote, []})
    |> ignore(string("\""))
    |> reduce({List, :to_string, []})
    |> unwrap_and_tag(:string_constant)
  )

  defcombinator(
    :keyword_constant,
    Enum.map(["true", "false", "null", "this"], fn x -> string(x) end)
    |> choice
    |> unwrap_and_tag(:keyword)
  )

  defcombinator(
    :operation,
    Enum.map(["+", "-", "*", "/", "&", "|", "<", ">", "="], fn x -> string(x) end)
    |> choice
    |> unwrap_and_tag(:symbol)
  )

  defcombinator(
    :unary_operation,
    Enum.map(["-", "~"], fn x -> string(x) end)
    |> choice
    |> unwrap_and_tag(:symbol)
  )

  def sub_rountine_call do
    choice([
      whitespace(parsec(:identifier))
      |> concat(
        sorround(
          optional(parsec(:expression_list)),
          "(",
          ")"
        )
      ),
      whitespace(parsec(:identifier))
      |> concat(whitespace(string(".")) |> unwrap_and_tag(:symbol))
      |> concat(
        whitespace(parsec(:identifier))
        |> concat(sorround(parsec(:expression_list), "(", ")"))
      )
    ])
  end

  def semi_colon do
    whitespace(string(";")) |> unwrap_and_tag(:symbol)
  end
end
