defmodule JackParser.Helper do
  import NimbleParsec

  def whitespace(combinator) do
    whitespace_ = ascii_char([32, 9, 10])
    ignore(repeat(whitespace_)) |> concat(combinator) |> ignore(repeat(whitespace_))
  end

  def numberConstant do
    integer(min: 1) |> tag(:integerConstant)
  end

  def sorround(between, open, close) do
    whitespace(string(open))
    |> tag(:symbol)
    |> concat(between)
    |> concat(whitespace(string(close)) |> tag(:symbol))
  end

  def identifier do
    ascii_string([?a..?z], min: 1) |> tag(:identifier)
  end

  def stringConstant do
    ignore(choice([ascii_char([22, 27]), string("\""), string("\'")]))
    |> ascii_string([?a..?z], min: 1)
    |> ignore(choice([ascii_char([22, 27]), string("\""), string("\'")]))
    |> tag(:stringConstant)
  end

  def keywordConstant do
    Enum.map(["true", "false", "null", "this"], fn x -> string(x) end)
    |> choice
    |> tag(:keywordConstant)
  end

  def operations do
    Enum.map(["+", "-", "*", "/", "&", "|", "<", ">", "="], fn x -> string(x) end)
    |> choice
    |> tag(:symbol)
  end

  def unary_operations do
    Enum.map(["-", "~"], fn x -> string(x) end)
    |> choice
    |> tag(:symbol)
  end

  def sub_rountine_call do
    choice([
      whitespace(identifier()) |> concat(sorround(parsec(:expression_list), "(", ")")),
      whitespace(identifier())
      |> concat(whitespace(string(".")))
      |> concat(whitespace(identifier()) |> concat(sorround(parsec(:expression_list), "(", ")")))
    ])
  end
end
