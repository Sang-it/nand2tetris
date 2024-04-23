using Antlr4.Runtime;

string input = "hello world";

AntlrInputStream inputStream = new AntlrInputStream(input);
HelloLexer speakLexer = new HelloLexer(inputStream);
CommonTokenStream commonTokenStream = new CommonTokenStream(speakLexer);
HelloParser speakParser = new HelloParser(commonTokenStream);
HelloParser.RContext chatContext = speakParser.r();
var visitor = new Visitor();
visitor.Visit(chatContext);

Console.WriteLine(visitor.Lines[0]);


public class Visitor : HelloBaseVisitor<object?>
{
    public List<String> Lines = new List<String>();

    public override object? VisitR(HelloParser.RContext context)
    {
        var name = context.ID().GetText();
        Lines.Add(name);
        return null;
    }
}
