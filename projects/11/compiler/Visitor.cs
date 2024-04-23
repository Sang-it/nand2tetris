namespace Compiler.Visitor
{
    using Antlr4.Runtime;

    public class Visitor : JackBaseVisitor<object?>
    {
        public override object? VisitClass_(JackParser.Class_Context context)
        {
            var name = context.className().GetText();

            foreach (var declaration in context.propertyDeclaration())
            {
                Visit(declaration);
            }

            foreach (var declaration in context.subRoutineDeclaration())
            {
                Visit(declaration);
            }

            Console.WriteLine(name);
            return null;
        }

        public override object? VisitPropertyDeclaration(JackParser.PropertyDeclarationContext context) {
            var kind = context.propertyKind().GetText();
            var type = context.type().GetText();
            var name = context.varName().GetText();

            foreach (var multi in context.multiPropertyDeclaration())
            {
                Visit(multi);
            }

            Console.WriteLine($"{kind} {type} {name}");
            return null;
        }

        public override object? VisitMultiPropertyDeclaration(JackParser.MultiPropertyDeclarationContext context) {
            var name = context.varName().GetText();
            Console.WriteLine($"Multi : {name}");
            return null;
        }

        public override object? VisitSubRoutineDeclaration(JackParser.SubRoutineDeclarationContext context) {
            var kind = context.subRoutineKind().GetText();
            var type = context.returnType().GetText();
            var name = context.subRoutineName().GetText();

            Console.WriteLine($"{kind} {type} {name}");
            return null;
        }
    }
}

