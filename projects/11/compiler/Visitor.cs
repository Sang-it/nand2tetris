namespace Compiler
{
    using System.Text;
    using Antlr4.Runtime;

    public class Visitor : JackBaseVisitor<object?>
    {
        private string className = "";
        private int fieldOffset = 0;
        private int staticOffset = 0;
        private int localOffset = 0;
        private int argumentOffset = 0;
        private int ifCount = 0;
        private int whileCount = 0;
        public Dictionary<string, Entry> ClassSymbolTable = new Dictionary<string, Entry>();
        public Dictionary<string, Entry> CurrFunctionSymbolTable = new Dictionary<string, Entry>();

        public override object? VisitClass_(JackParser.Class_Context context)
        {
            var vmOutput = new StringBuilder();

            var className_ = context.className().GetText();
            className = className_;

            foreach (var declaration in context.propertyDeclaration()) {
                Visit(declaration);
            }

            foreach (var declaration in context.subRoutineDeclaration()) {
                vmOutput.Append(Visit(declaration));
            }

            return vmOutput.ToString();
        }

        private void updateClassSymbolTable(string name, string type,string kind){
            if (kind == "static") {
                ClassSymbolTable[name] = new Entry(type, "static", staticOffset++);
                return;
            }
            ClassSymbolTable[name] = new Entry(type, "this", fieldOffset++);
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
            argumentOffset = 0;
            localOffset = 0;
        }

        public override object? VisitSubRoutineDeclaration(JackParser.SubRoutineDeclarationContext context) {
            var output = new StringBuilder();
            var kind = context.subRoutineKind().GetText();
            var type = context.returnType().GetText();
            var name = context.subRoutineName().GetText();

            if(!string.IsNullOrEmpty(context.parameterList().GetText())) {
                Visit(context.parameterList());
            }
            string bodyOutput = (string)Visit(context.subRoutineBody());
            output.AppendLine($"function {className}.{name} {localOffset}");
            output.Append(bodyOutput);

            ResetCurrFunction();
            return output.ToString();
        }

        private void updateCurrFunctionSymbolTable(string name, string type, string kind){
            if(kind == "argument") {
                CurrFunctionSymbolTable[name] = new Entry(type, kind, argumentOffset++);
                return;
            }
            CurrFunctionSymbolTable[name] = new Entry(type, kind, localOffset++);
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
            var output = new StringBuilder();

            foreach (var declaration in context.variableDeclaration()){
                Visit(declaration);
            }
            foreach (var statement in context.statement()){
                output.Append(Visit(statement));
            }

            return output.ToString();
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

        private Entry? GetEntry(string name){
            if (CurrFunctionSymbolTable.ContainsKey(name)) {
                return CurrFunctionSymbolTable[name];
            } else if (ClassSymbolTable.ContainsKey(name)) {
                return ClassSymbolTable[name];
            }
            return null;
        }

        public override object? VisitLetStatement(JackParser.LetStatementContext context) {
            var output = new StringBuilder();
            var name = context.varName().GetText();
            Entry? entry = GetEntry(name);
            if (entry.HasValue) {
                if (context.chainArrayAccess() != null) {
                    output.Append(Visit(context.chainArrayAccess()));
                    output.AppendLine($"push {entry.Value.Kind} {entry.Value.Offset}");
                    output.AppendLine("add");
                    output.AppendLine((string)Visit(context.expression()));
                    output.AppendLine("pop temp 0");
                    output.AppendLine("pop pointer 1");
                    output.AppendLine("push temp 0");
                    output.AppendLine("pop that 0");
                } else {
                    output.Append(Visit(context.expression()));
                    output.AppendLine($"pop {entry.Value.Kind} {entry.Value.Offset}");
                }
            } else {
                throw new Exception($"Variable {name} not found");
            }
            return output.ToString();
        }

        public override object? VisitChainArrayAccess(JackParser.ChainArrayAccessContext context) {
            return Visit(context.expression());
        }

        public override object? VisitExpression(JackParser.ExpressionContext context)
        {
            var output = new StringBuilder();
            output.Append(Visit(context.term()));
            foreach (var expression in context.chainExpression())
            {
                output.Append(Visit(expression));
            }
            return output.ToString();
        }

        public override object? VisitIntegerConstant(JackParser.IntegerConstantContext context) {
            return $"push constant {context.GetText()}\n";
        }

        public override object? VisitKeywordConstant(JackParser.KeywordConstantContext context)
        {
            return context.GetText() switch
            {
                "false" => "push constant 0\n",
                    "null" => "push constant 0\n",
                    "true" => "push constant 1\n",
                    "this" => "push pointer 0\n",
                    _ => throw new Exception("Unexpected keyword constant")
            };
        }

        private string UnaryOpCodeToString(string op){
            return op switch
            {
                "-" => "neg\n",
                    "~" => "not\n",
                    _ => throw new Exception("Unexpected unary operator")
            };
        }

        public override object? VisitUnaryTerm(JackParser.UnaryTermContext context) {
            var output = new StringBuilder();
            output.Append(Visit(context.term()));
            var op = context.unaryOp().GetText();
            output.Append(UnaryOpCodeToString(op));
            return output.ToString();
        }

        public override object? VisitVarName(JackParser.VarNameContext context) {
            var name = context.GetText();
            var entry = GetEntry(name);

            if (entry.HasValue)
            {
                return $"push {entry.Value.Kind} {entry.Value.Offset}\n";
            }
            else
            {
                throw new Exception($"Variable {name} not found");
            }
        }

        public override object? VisitParenthesisExpression(JackParser.ParenthesisExpressionContext context) {
            return Visit(context.expression());
        }

        public override object? VisitArrayAccess(JackParser.ArrayAccessContext context)
        {
            var output = new StringBuilder();
            var name = context.varName().GetText();
            output.Append(Visit(context.expression()));
            var entry = GetEntry(name);

            if (entry.HasValue)
            {
                output.AppendLine($"push {entry.Value.Kind} {entry.Value.Offset}");
                output.AppendLine("add");
                output.AppendLine("pop pointer 1");
                output.AppendLine("push that 0");
            }
            else
            {
                throw new Exception($"Variable {name} not found");
            }

            return output.ToString();
        }

        public override object? VisitStringConstant(JackParser.StringConstantContext context)
        {
            var output = new StringBuilder();
            var str = context.GetText().Trim('"');
            output.AppendLine($"push constant {str.Length}");
            output.AppendLine("call String.new 1");

            foreach (var c in str)
            {
                output.AppendLine($"push constant {(int)c}");
                output.AppendLine("call String.appendChar 2");
            }

            return output.ToString();
        }

        private string OpCodeToString(string op){
            return op switch
            {
                "+" => "add\n",
                    "-" => "sub\n",
                    "&" => "and\n",
                    "|" => "or\n",
                    "<" => "lt\n",
                    ">" => "gt\n",
                    "=" => "eq\n",
                    "*" => "call Math.multiply 2\n",
                    "/" => "call Math.divide 2\n",
                    _ => throw new Exception("Unexpected operator")
            };
        }

        public override object? VisitChainExpression(JackParser.ChainExpressionContext context) {
            var output = new StringBuilder();
            output.Append(Visit(context.term()));
            string op = context.op().GetText();
            output.Append(OpCodeToString(op));
            return output.ToString();
        }

        public override object? VisitIfStatement(JackParser.IfStatementContext context) {
            var output = new StringBuilder();
            int count = ifCount++;
            output.Append(Visit(context.expression()));
            output.AppendLine($"if-goto TRUE_{count}");
            output.AppendLine($"goto FALSE_{count}");
            output.AppendLine($"label TRUE_{count}");

            if (context.elseClause() != null)
            {
                output.Append(Visit(context.ifClause()));
            }

            output.AppendLine($"goto END_{count}");
            output.AppendLine($"label FALSE_{count}");

            if (context.elseClause() != null)
            {
                output.Append(Visit(context.elseClause()));
            }

            output.AppendLine($"label END_{count}");
            return output.ToString();
        }

        public override object? VisitIfClause(JackParser.IfClauseContext context) {
            var output = new StringBuilder();
            foreach (var statement in context.statement())
            {
                output.Append(Visit(statement));
            }
            return output.ToString();
        }

        public override object? VisitElseClause(JackParser.ElseClauseContext context) {
            var output = new StringBuilder();
            foreach (var statement in context.statement())
            {
                output.Append(Visit(statement));
            }
            return output.ToString();
        }

        public override object? VisitWhileStatement(JackParser.WhileStatementContext context)
        {
            StringBuilder output = new StringBuilder();
            int count = whileCount++;
            output.AppendLine($"label WHILE_START_{count}");
            output.Append(Visit(context.expression()));
            output.AppendLine("not");
            output.AppendLine($"if-goto WHILE_END_{count}");

            foreach (var statement in context.statement())
            {
                output.Append(Visit(statement));
            }

            output.AppendLine($"goto WHILE_START_{count}");
            output.AppendLine($"label WHILE_END_{count}");

            return output.ToString();
        }

        public override object? VisitDoStatement(JackParser.DoStatementContext context)
        {
            var output = new StringBuilder();
            output.Append(Visit(context.subRoutineCall()));
            output.AppendLine("pop temp 0");
            return output.ToString();
        }

        public override object? VisitFunctionCall(JackParser.FunctionCallContext context)
        {
            var output = new StringBuilder();
            output.AppendLine("push pointer 0");
            int count = 1;

            if (!string.IsNullOrEmpty(context.expressionList().GetText()))
            {
                var (expressionOutput, expressionCount) = ((string, int)) Visit(context.expressionList());
                output.Append(expressionOutput);
                count += expressionCount;
            }

            string name = context.subRoutineName().GetText();
            output.AppendLine($"call {className}.{name} {count}");
            return output.ToString();
        }

        public override object? VisitMethodCall(JackParser.MethodCallContext context)
        {
            StringBuilder output = new StringBuilder();
            string name = context.subRoutineName().GetText();
            string className_ = context.className().GetText();

            Entry? entry = GetEntry(className_);
            int count = 0;

            if (entry.HasValue)
            {
                output.AppendLine($"push {entry.Value.Kind} {entry.Value.Offset}");
                count = 1;
            }

            if (!string.IsNullOrEmpty(context.expressionList().GetText()))
            {
                var (expressionOutput, expressionCount) = ((string, int)) Visit(context.expressionList());
                output.Append(expressionOutput);
                count += expressionCount;
            }

            output.AppendLine($"call {className_}.{name} {count}");
            return output.ToString();
        }

        public override object? VisitExpressionList(JackParser.ExpressionListContext context)
        {
            StringBuilder output = new StringBuilder();
            int count = 1;
            output.Append(Visit(context.expression()));

            foreach (var expression in context.chainExpressionList())
            {
                output.Append(Visit(expression));
                count++;
            }

            return (output.ToString(), count);
        }

        public override object? VisitChainExpressionList(JackParser.ChainExpressionListContext context)
        {
            StringBuilder output = new StringBuilder();
            output.Append(Visit(context.expression()));
            return output.ToString();
        }

        public override object? VisitReturnStatement(JackParser.ReturnStatementContext context)
        {
            StringBuilder output = new StringBuilder();

            if (context.expression() != null)
            {
                output.Append(Visit(context.expression()));
            }
            else
            {
                output.AppendLine("push constant 0");
            }

            output.AppendLine("return");
            return output.ToString();
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
