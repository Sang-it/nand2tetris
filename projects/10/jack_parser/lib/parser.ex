defmodule JackParser do
  def main(arg \\ []) do
    if arg == [] do
      IO.puts("Usage: jack_parser <filename>")
      System.halt(1)
    end

    case File.read(arg) do
      {:ok, content} ->
        IO.inspect(JackParser.Statement.parse(content))

      {:error, reason} ->
        IO.puts("Error: #{reason}")
    end
  end
end
