grammar Jack;

class_: 'class' className '{' propertyDeclaration* subRoutineDeclaration* '}' EOF;

propertyDeclaration : propertyKind type varName multiPropertyDeclaration*';';

multiPropertyDeclaration : (',' varName);

subRoutineDeclaration : subRoutineKind returnType subRoutineName '(' ')' subRoutineBody;

returnType: 'void' | type;

subRoutineBody : '{' '}';

className : ID;

varName : ID;

subRoutineName : ID;

propertyKind : 'static' | 'field';

subRoutineKind : 'constructor' | 'function' | 'method';

type: 'int' | 'char' | 'boolean' | ID;

ID: [a-zA-Z]+;

WS: [ \t\r\n]+ -> skip;
/*

parameterList : (TYPE varName (',' TYPE varName)*)?;

subRoutineBody : '{' (varDeclaration)* statements '}';

varDeclaration : 'var' TYPE varName (',' varName)*';';

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
