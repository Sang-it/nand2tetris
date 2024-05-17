grammar Jack;

class_: 'class' className '{' propertyDeclaration* subRoutineDeclaration* '}' EOF;

propertyDeclaration : propertyKind type varName chainProperty*';';

subRoutineDeclaration : subRoutineKind returnType subRoutineName '(' parameterList ')' subRoutineBody;

returnType: 'void' | type;

parameterList : (type varName chainParameter*)?;

chainParameter : ',' type varName;

subRoutineBody : '{' variableDeclaration* statement* '}';

variableDeclaration : 'var' type varName chainVariable*';';

chainProperty : ',' varName;

chainVariable : ',' varName;

statement : letStatement
            | returnStatement
            | ifStatement
            | whileStatement
            | doStatement;

letStatement : 'let' varName '=' expression ';';

returnStatement : 'return' expression? ';';

ifStatement : 'if' '(' expression ')' ifClause elseClause?;

whileStatement : 'while' '(' expression ')' '{' statement* '}';

doStatement : 'do' subRoutineCall ';';

ifClause : '{' statement* '}';

elseClause : 'else' '{' statement* '}';

subRoutineCall : functionCall | methodCall;

functionCall : subRoutineName '(' expressionList ')';

methodCall : (className | varName) '.' subRoutineName '(' expressionList ')';

expressionList : (expression chainExpressionList*)?;

chainExpressionList : ',' expression;

expression : term chainExpression*;

chainExpression : op term;

term : integerConstant | subRoutineCall | keywordConstant | unaryTerm | varName;

unaryTerm : unaryOp term;

integerConstant : INTEGER_CONSTANT;

keywordConstant : KEYWORD_CONSTANT;

op : '+' | '-' | '!' | '|' | '&' | '*' | '/' | '<' | '>' | '=';

unaryOp : '-' | '~';

className : ID;

varName : ID;

subRoutineName : ID;

type: 'int' | 'char' | 'boolean' | ID;

propertyKind : 'static' | 'field';

subRoutineKind : 'constructor' | 'function' | 'method';

KEYWORD_CONSTANT : 'true' | 'false' | 'null' | 'this';

INTEGER_CONSTANT : [0-9]+;

ID: [a-zA-Z]+;

WS: [ \t\r\n]+ -> skip;
/*


doStatement : 'do' '(' expressionList ')' ';';

returnStatement : 'return' expression? ';';

expressionList : (expression (',' expression)*)?;

expression : term (op term)*;

term : INTEGER_CONSTANT | STRING_CONSTANT | KEYWORD_CONSTANT | varName | varName '[' expression ']' | subRoutineCall | '(' expression ')' | unaryOp term;

subRoutineCall : ID '(' expressionList ')' | (ID '.') ID '(' expressionList ')';

unaryOp : '-' | '~';

*/
