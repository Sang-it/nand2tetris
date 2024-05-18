namespace Compiler
{
    using Antlr4.Runtime;

    class Core {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("No file or folder provided.");
                return;
            }
            string path = args[0];
            if (File.Exists(path))
            {
                CompileFile(path);
            }
            else if (Directory.Exists(path))
            {
                CompileDirectory(path);
            }
            else
            {
                Console.WriteLine($"The path '{path}' is invalid.");
            }
        }

        static void CompileFile(string path)
        {
            string input = File.ReadAllText(path);
            string vm_path = Path.ChangeExtension(path, ".vm");

            AntlrInputStream inputStream = new AntlrInputStream(input);
            JackLexer jackLexer = new JackLexer(inputStream);
            CommonTokenStream commonTokenStream = new CommonTokenStream(jackLexer);
            JackParser jackParser = new JackParser(commonTokenStream);
            JackParser.Class_Context classContext = jackParser.class_();
            JackBaseVisitor<object?> visitor = new Visitor();
            string output_vm = (string)visitor.Visit(classContext);

            File.WriteAllText(vm_path, output_vm);
        }

        static void CompileDirectory(string path)
        {
            string[] files = Directory.GetFiles(path);
            foreach (string file in files)
            {
                if (Path.GetExtension(file) == ".jack")
                {
                    CompileFile(file);
                }
            }
        }
    }
}
