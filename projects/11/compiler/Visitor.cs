namespace Compiler.Visitor
{
    using System.Text;
    using Antlr4.Runtime;

    public class Visitor : JackBaseVisitor<object?>
    {
        public string vm_output;
        private string className = "";
        private int field_offset = 0;
        private int static_offset = 0;
        private int local_offset = 0;
        private int argument_offset = 0;

        private int if_count = 0;
        private int while_count = 0;

        public Dictionary<string, Entry> ClassSymbolTable = new Dictionary<string, Entry>();
        public Dictionary<string, Entry> CurrFunctionSymbolTable = new Dictionary<string, Entry>();

        public override object? VisitClass_(JackParser.Class_Context context)
        {
            var className_ = context.className().GetText();
            className = className_;

            foreach (var declaration in context.propertyDeclaration()) {
                Visit(declaration);
            }

            foreach (var declaration in context.subRoutineDeclaration()) {
                Visit(declaration);
            }

            return vm_output;
        }

        private void updateClassSymbolTable(string name, string type,string kind){
            if (kind == "static") {
                ClassSymbolTable[name] = new Entry(type, "static", static_offset++);
                return;
            }
            ClassSymbolTable[name] = new Entry(type, "this", field_offset++);
        }

        public override object? VisitPropertyDeclaration(JackParser.PropertyDeclarationContext context) {
            var name = context.varName().GetText();
            var type = context.type().GetText();
            var kind = context.propertyKind().GetText();

            updateClassSymbolTable(name, type, kind);

            foreach (var multi in context.chainProperty()) {
                string name_ = (string) Visit(multi);
                updateClassSymbolTable(name_, type , kind);
            }

            return null;
        }

        public override object? VisitChainProperty(JackParser.ChainPropertyContext context) {
            return context.varName().GetText();
        }

        private void ResetCurrFunction() {
            CurrFunctionSymbolTable.Clear();
            argument_offset = 0;
            local_offset = 0;
        }

        public override object? VisitSubRoutineDeclaration(JackParser.SubRoutineDeclarationContext context) {
            var kind = context.subRoutineKind().GetText();
            var type = context.returnType().GetText();
            var name = context.subRoutineName().GetText();

            if(!string.IsNullOrEmpty(context.parameterList().GetText())) {
                Visit(context.parameterList());
            }
            string bodyOutput = (string)Visit(context.subRoutineBody());

            vm_output += $"function {className}.{name} {local_offset}\n";
            vm_output += bodyOutput;

            ResetCurrFunction();
            return null;
        }

        private void updateCurrFunctionSymbolTable(string name, string type, string kind){
            if(kind == "argument") {
                CurrFunctionSymbolTable[name] = new Entry(type, kind, argument_offset++);
                return;
            }
            CurrFunctionSymbolTable[name] = new Entry(type, kind, local_offset++);
        }

        public override object? VisitParameterList(JackParser.ParameterListContext context) {
            var name = context.varName().GetText();
            var type = context.type().GetText();
            updateCurrFunctionSymbolTable(name , type , "argument");
            foreach (var multi in context.chainParameter()) {
                Visit(multi);
            }
            return null;
        }

        public override object? VisitChainParameter(JackParser.ChainParameterContext context) {
            var name = context.varName().GetText();
            var type = context.type().GetText();
            updateCurrFunctionSymbolTable(name , type , "argument");
            return null;
        }


        public override object? VisitSubRoutineBody(JackParser.SubRoutineBodyContext context) {
            string output = "";

            foreach (var declaration in context.variableDeclaration()){
                Visit(declaration);
            }
            foreach (var statement in context.statement()){
                output += (string)Visit(statement);
            }

            return output;
        }

        public override object? VisitVariableDeclaration(JackParser.VariableDeclarationContext context) {
            var type = context.type().GetText();
            var name = context.varName().GetText();
            updateCurrFunctionSymbolTable(name , type , "local");

            foreach (var multi in context.chainVariable()) {
                string name_ = (string) Visit(multi);
                updateCurrFunctionSymbolTable(name_, type , "local");
            }

            return null;
        }

        public override object? VisitChainVariable(JackParser.ChainVariableContext context) {
            return context.varName().GetText();
        }

        public override object? VisitStatement(JackParser.StatementContext context) {
            return VisitChildren(context);
        }

        private Entry? getEntry(string name){
            if (CurrFunctionSymbolTable.ContainsKey(name)) {
                return CurrFunctionSymbolTable[name];
            } else if (ClassSymbolTable.ContainsKey(name)) {
                return ClassSymbolTable[name];
            }
            return null;
        }

        public override object? VisitLetStatement(JackParser.LetStatementContext context) {
            string output = "";
            var name = context.varName().GetText();
            output += Visit(context.expression());
            Entry? entry = getEntry(name);
            if (entry.HasValue) {
                output += $"pop {entry.Value.Kind} {entry.Value.Offset}\n";
            } else {
                throw new Exception($"Variable {name} not found");
            }
            return output;
        }

        public override object? VisitExpression(JackParser.ExpressionContext context) {
            string output = "";
            output += Visit(context.term());
            foreach (var expression in context.chainExpression()){
                output += Visit(expression);
            };
            return output;
        }

        public override object? VisitIntegerConstant(JackParser.IntegerConstantContext context) {
            return $"push constant {context.GetText()}\n";
        }

        private string opCodeToString(string op){
            switch(op) {
                case "+":
                    return "add\n";
                case "-":
                    return "sub\n";
                case "!":
                    return "neg\n";
                case "|":
                    return "or\n";
                case "&":
                    return "and\n";
                case "=":
                    return "eq\n";
                case ">":
                    return "gt\n";
                case "<":
                    return "lt\n";
                case "*":
                    return "call Math.multiply 2\n";
                case "/":
                    return "call Math.divide 2\n";
                default:
                    return "unreachable!()";
            }
        }

        public override object? VisitChainExpression(JackParser.ChainExpressionContext context) {
            string output = "";
            output += Visit(context.term());
            string op = context.op().GetText();
            output += opCodeToString(op);
            return output;
        }

        public override object? VisitIfStatement(JackParser.IfStatementContext context) {
            string output = "";
            int count = if_count++;
            output += Visit(context.expression());
            output += $"if-goto TRUE_{count}\n";
            output += $"goto FALSE_{count}\n";;
            output += $"label TRUE_{count}\n";

            output += Visit(context.ifClause());

            output += $"goto END_{count}\n";
            output += $"label FALSE_{count}\n";

            output += Visit(context.elseClause());

            output += $"label END_{count}\n";
            return output;
        }

        public override object? VisitIfClause(JackParser.IfClauseContext context) {
            string output = "";
            foreach (var statement in context.statement()){
                output += Visit(statement);
            }
            return output;
        }

        public override object? VisitElseClause(JackParser.ElseClauseContext context) {
            string output = "";
            foreach (var statement in context.statement()){
                output += Visit(statement);
            }
            return output;
        }

        public override object? VisitWhileStatement(JackParser.WhileStatementContext context) {
            string output = "";
            int count = while_count++;
            output += $"label WHILE_START_{count}\n";
            output += Visit(context.expression());
            output += $"not\n";
            output += $"if-goto WHILE_END_{count}\n";
            foreach (var statement in context.statement()){
                output += Visit(statement);
            }
            output += $"goto WHILE_START_{count}\n";
            output += $"label WHILE_END_{count}\n";
            return output;
        }

        public override object? VisitDoStatement(JackParser.DoStatementContext context) {
            string output = "";
            output += Visit(context.subRoutineCall());
            output += "pop temp 0\n";
            return output;
        }

        public override object? VisitSubRoutineCall(JackParser.SubRoutineCallContext context) {
            return VisitChildren(context);
        }

        public override object? VisitFunctionCall(JackParser.FunctionCallContext context) {
            string output = "";
            output += "push pointer 0\n";
            int count = 1;
            if (!string.IsNullOrEmpty(context.expressionList().GetText())) {
                var (output_, count_) = ((string,int))Visit(context.expressionList());
                output +=output_;
                count += count_;
            }
            string name = context.subRoutineName().GetText();
            output += "call " + className + "." + name + " " + count + "\n";
            return output;
        }

        public override object? VisitMethodCall(JackParser.MethodCallContext context) {
            string output = "";
            string name = context.subRoutineName().GetText();
            string className_ = context.className().GetText();

            Entry? entry = getEntry(className_);
            int count = 0;
            if (entry.HasValue) {
                output += $"push {entry.Value.Kind} {entry.Value.Offset}\n";
                count = 1;
            }
            if (!string.IsNullOrEmpty(context.expressionList().GetText())) {
                var (output_, count_) = ((string,int))Visit(context.expressionList());
                output +=output_;
                count += count_;
            }
            output += "call " + className_ + "." + name + " " + count + "\n";
            return output;
        }

        public override object? VisitExpressionList(JackParser.ExpressionListContext context) {
            string output = "";
            int count = 1;
            output += Visit(context.expression());
            foreach (var expression in context.chainExpressionList()){
                output += Visit(expression);
                count++;
            }
            return (output, count);
        }

        public override object? VisitChainExpressionList(JackParser.ChainExpressionListContext context) {
            string output = "";
            output += Visit(context.expression());
            return output;
        }
    }


    public struct Entry
    {
        public Entry(string type, string kind, int offset)
        {
            Type = type;
            Kind = kind;
            Offset = offset;
        }
        public string Type { get; }
        public string Kind { get; }
        public int Offset { get; }
        public override string ToString()
        {
            return $"Type: {Type}, Kind: {Kind}, Offset: {Offset}";
        }
    }
}

