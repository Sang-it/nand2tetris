using Antlr4.Runtime;
using Compiler.Visitor;

var filename = "Square.jack";

string input = File.ReadAllText(filename);

AntlrInputStream inputStream = new AntlrInputStream(input);
JackLexer jackLexer = new JackLexer(inputStream);
CommonTokenStream commonTokenStream = new CommonTokenStream(jackLexer);
JackParser jackParser = new JackParser(commonTokenStream);
JackParser.Class_Context classContext = jackParser.class_();
JackBaseVisitor<object?> visitor = new Visitor();

var path = "./Square.vm";
string output_vm = (string)visitor.Visit(classContext);
File.WriteAllText(path, output_vm);

