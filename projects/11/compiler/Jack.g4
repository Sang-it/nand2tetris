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

statement : letStatement | returnStatement;

letStatement : 'let' varName '=' expression ';';

returnStatement : 'return' expression? ';';

expression : term chainExpression*;

chainExpression : op term;

term : integerConstant;

integerConstant : INTEGER_CONSTANT;

op : '+' | '-' | '!' | '|' | '&' | '*' | '/' | '<' | '>' | '=';

className : ID;

varName : ID;

subRoutineName : ID;

type: 'int' | 'char' | 'boolean' | ID;

propertyKind : 'static' | 'field';

subRoutineKind : 'constructor' | 'function' | 'method';

INTEGER_CONSTANT : [0-9]+;

ID: [a-zA-Z]+;

WS: [ \t\r\n]+ -> skip;
/*


statements : (statement)*;

statement
    : letStatement
    | ifStatement
    | whileStatement
    | doStatement
    | returnStatement
    ;

letStatement : 'let' varName ('[' expression ']')? '=' expression ';';

ifStatement : 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?;

whileStatement : 'while' '(' expression ')' '{' statements '}';

doStatement : 'do' '(' expressionList ')' ';';

returnStatement : 'return' expression? ';';

expressionList : (expression (',' expression)*)?;

expression : term (op term)*;

term : INTEGER_CONSTANT | STRING_CONSTANT | KEYWORD_CONSTANT | varName | varName '[' expression ']' | subRoutineCall | '(' expression ')' | unaryOp term;

subRoutineCall : ID '(' expressionList ')' | (ID '.') ID '(' expressionList ')';

op : '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '=';

unaryOp : '-' | '~';

varName : ID;

subRoutineName : ID;

KEYWORD_CONSTANT : 'true' | 'false' | 'null' | 'this';

INTEGER_CONSTANT : [0-9]+;

STRING_CONSTANT : '"' .*? '"';

SUB_ROUTINE_KIND : 'constructor' | 'function' | 'method';

*/
