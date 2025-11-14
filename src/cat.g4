grammar cat;

options
{
	language = Cpp;
}

fragment
Q: '\'';
fragment
NQ: ~['];
fragment
QQ: '"';
fragment
NQQ: ~["];
fragment
BS: '\\';

fragment
SPACER: '_' | '`';

fragment
SIGN:   '+' | '-';

fragment
DIGIT:  [0-9] ;

fragment
LETTER: [a-zA-Z];

fragment
NIBBLE: [a-fA-F0-9];

#define SCI ([eE] SIGN DIGIT*)

#define F_SUFFIX [fFlL]
#define I_SUFFIX [uUlL]

#define HEX ( '0'[xX]  (NIBBLE | SPACER)+ )
#define OCT ( '0'[oO]? (DIGIT  | SPACER)+ )
#define BIN ( '0'[bB]  (DIGIT  | SPACER)+ )
#define HOL ( DIGIT (DIGIT | SPACER)*	)

#define DEC \
( \
	(HOL? '.' HOL)  \
|	(HOL  '.' HOL?) \
)

INT_CONST:   SIGN? (HEX | OCT | BIN | HOL) I_SUFFIX?;

FLOAT_CONST: SIGN? DEC SCI? F_SUFFIX?;

CHAR_CONST:  'L'?  Q (BS  Q |  NQ)*  Q;

STR_CONST:   'L'? QQ (BS QQ | NQQ)* QQ;

LINE_COMMENT: '//' ~[\r\n]* -> channel (HIDDEN);

BLOCK_COMMENT: '/*' .*? '*/' -> channel (HIDDEN);

L_PAREN: '(';

WHITESPACE: [ \t\\r\n] -> skip;

IGNORE: . -> skip;

ALIAS: 'alias';
MACRO: 'macro';
TEMPLATE: 'template';
ITERATOR: 'iterator';
OPERATOR: 'operator';

INCLUDE: 'include';
INLINE: 'inline';
EXTERN: 'extern';

SIZEOF: 'sizeof';
COUNTOF: 'countof';
NAMEOF: 'nameof';
TYPEOF: 'typeof';

LOCAL: 'local';
STATIC: 'static';
CONST: 'const';

STRUCT: 'struct';
MODULE: 'module';
UNION: 'union';
ENUM: 'enum';

SIGNED: 'signed';
UNSIGNED: 'unsigned';
COMPLEX: 'complex';
IMAGINARY: 'imaginary';
LONG: 'long';
DOUBLE: 'double';

LET: 'let';
BIT: 'bit';
CHAR: 'char';
BYTE: 'byte';
SHORT: 'short';
INT: 'int';
FLOAT: 'float';

BREAK: 'break';
CONTINUE: 'continue';
RETURN: 'return';
GOTO: 'goto';

WHILE: 'while';
DO: 'do';
FOR: 'for';

IF: 'if';
ELSE: 'else';
SWITCH: 'switch';
CASE: 'case';
DEFAULT: 'default';
AS: 'as';

NAME: (LETTER | SPACER) (LETTER | DIGIT | SPACER)*;

block: statement*;

statement:

	NAME ':'
|	CASE (expression | range_expression) ':'?
|	DEFAULT ':'?
|	CONTINUE ';'?
|	BREAK ';'?
|	GOTO NAME ';'?
|	RETURN expression? ';'?
|	IF expression scope ('else' scope)?
|	SWITCH expression scope
|	WHILE expression scope
|	DO WHILE expression scope
|	DO scope WHILE expression ';'?
|	FOR iterator scope
|	ITERATOR iterator_define ';'?
|	OPERATOR definable_operator tuple scope
|	line
;

line:

	note line
|	ALIAS NAME '=' label ';'?
|	MACRO NAME  .*? '#'
|	INCLUDE label ('as' NAME)? ';'?
|	generics? qualifier* module (instances ';'?)?
|	generics? qualifier* function
|	qualifier* variables ';'?
|	expression ';'?
|	stack_frame
|	';'
;

note: 'note' STR_CONST+;

stack_frame: qualifier* braced_scope;

module:

	('struct' | 'union' | 'module') label scope
|	'enum' label enum_scope
;

scope:

	braced_scope
|	':'? block ('...' | EOF)
;

braced_scope: '{' block '}';

enum_scope:

	'{'  instances? '}'
|	':'? instances? ('...' | EOF)
;

generics: '[' label+ ']';

iterator:

	'(' iterator ')'
|	loop_iterator
|	label ':' range_expression
|	label ':' expression
;

iterator_define:

	'(' iterator_define ')'
|	loop_iterator
|	range_expression
|	expression
;

loop_iterator: line expression ';' line;

label: NAME ('.' NAME)*;

variable: type instance;

variables:

	type
	// prevent parsing functions as variables
	{_input->LA (1) != catParser::L_PAREN}?
	instances;

instances: instance (',' instance)*;

instance: label ('[' expression ']')* (initializer | '..')?;

initializer: '=' expression;

function: type label tuple scope;

tuple: '(' parameters? ')';

parameters: variable (',' variable)*;

type:

	type_qualifier* data pointer*
|	type_qualifier* data tuple pointer*
;

data:

	'let'
|	builtin
|	tuple
|	'typeof' label
|	label
;

expression:

	atomic
|	prefix expression
|	expression infix expression
|	expression postfix
|	expression expression
|	expression ',' expression
;

atomic:

	'(' expression? ')'
|	'[' expression? ']'
|	value
;

value:

	label
|	literal
|	meta label
;

literal: INT_CONST | FLOAT_CONST | CHAR_CONST | STR_CONST;

range_expression: expression '..' expression;

pointer:

	'~' 'const'? // cat-style pointer
|	'*' 'const'? // c-style pointer
|	'!' 'const'? // c++ reference-style pointer (const keyword is extraneous, it will always be const)
;

builtin:

	'bit'
|	'char'
|	'byte'
|	'short' 'int'?
|	'long'+ 'int'?
|	'int'
|	'float'
|	'long'+ 'double'
;

qualifier: 'local' | 'static' | 'extern' | 'inline';

type_qualifier: 'const' | 'signed' | 'unsigned' | 'complex' | 'imaginary';

meta: 'sizeof' | 'countof' | 'nameof' | 'stringof';

prefix:

	'++'
|	'--'
|	'+'
|	'-'
|	'!'
|	'&'
|	'*'
|	'~'
;

postfix: '++' | '--';

infix:

	('and' | '&&')
|	( 'or' | '||')
	
|	'==' | '!='
	
|	'<' | '<=' | '>=' | '>'
	
|	arithmetic
|	arithmetic '='
|	'='
;

arithmetic:

	  '+' |  '-'
|	  '*' |  '/' | '%'
|	  '&' |  '^' | '|'
|	 '<<' | '>>'
|	'<<<' | '>>>'
;

definable_operator:

	'++' | '--' |   '+' |   '-' | '%'
|	 '&' |  '*' |   '~' |   '/'
|	'<<' | '>>' | '<<<' | '>>>'
;
