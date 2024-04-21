defmodule JackParser do
  def main(arg \\ []) do
    if arg == [] do
      IO.puts("Usage: jack_parser <filename>")
      System.halt(1)
    end

    case File.read(arg) do
      {:ok, content} ->
        File.open!(Path.basename(arg) <> ".out", [:write], fn file ->
          output = JackParser.Program.parse(content)
          IO.inspect(file, output, [])
        end)

      {:error, reason} ->
        IO.puts("Error: #{reason}")
    end
  end
end
