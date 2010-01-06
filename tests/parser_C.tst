/*
 * C Test Grammar, based on http://lists.canonical.org/pipermail/kragen-hacks/1999-October/000201.html
 */
grammar C;

translation_unit: 
	(function_definition | declaration)+
	;

function_definition: 
 	declaration_specifiers? declarator declaration* block
	;

declaration: 
	declaration_specifiers init_declarator<,> ";"
	;

declaration_specifiers: 
  	(storage_class_specifier | type_specifier | type_qualifier)+
	;

storage_class_specifier: 
	("auto" | "register" | "static" | "extern" | "typedef")
	;
	
type_specifier: 
	("void" | 
	"char" | 
	"short" | 
	"int" | 
	"long" | 
	"float" |
 	"double" | 
	"signed" | 
	"unsigned" | 
	struct_or_union_specifier | 
  	enum_specifier | 
	typedef_name)
	;
	
type_qualifier: 
	("const" | "volatile")
	;
	
struct_or_union_specifier: 
  	("struct" | "union") (
    	identifier? "{" struct_declaration+ "}" |
    	identifier
  	)
	;

init_declarator: 
	declarator ("=" initializer)?
	;
	
struct_declaration: 
  	(type_specifier | type_qualifier)+ struct_declarator<,>
	;

struct_declarator: 
	declarator | declarator? ":" constant_expression
	;

enum_specifier: "enum" (identifier | identifier? "{" enumerator<,> "}")
	;

enumerator: identifier ("=" constant_expression)?
	;

declarator: 
  	pointer? (identifier | "(" declarator ")") (
    	"[" constant_expression? "]" |
    	"(" parameter_type_list ")" |
    	"(" identifier<,>? ")"
  	)*
	;

pointer:
  	("*" type_qualifier*)*
	;

parameter_type_list: parameter_declaration<,> ("," "...")?
	;
	
parameter_declaration: 
	declaration_specifiers (declarator | abstract_declarator)?
	;
	
initializer: assignment_expression | "{" initializer<,> ","? "}"
	;

type_name: (type_specifier | type_qualifier)+ abstract_declarator?
	;
	
abstract_declarator: 
  	pointer ("(" abstract_declarator ")")? (
    	"[" constant_expression? "]" |
    	"(" parameter_type_list? ")"
  	)*
	;

statement:
  ((identifier | "case" constant_expression | "default") ":")*
  (expression? ";" | 
   block | 
   "if" "(" expression ")" statement |
   "if" "(" expression ")" statement "else" statement |
   "switch" "(" expression ")" statement |
   "while" "(" expression ")" statement |
   "do" statement "while" "(" expression ")" ";" |
   "for" "(" expression? ";" expression? ";" expression? ")" statement |
   "goto" identifier ";" |
   "continue" ";" |
   "break" ";" |
   "return" expression? ";"
  )
	;

block: "{" declaration* statement* "}"
	;
	
expression: 
	assignment_expression<,>
	;
	
assignment_expression: (
    unary_expression (
      "=" | "*=" | "/=" | "%=" | "+=" | "-=" | "<<=" | ">>=" | "&=" |
      "^=" | "|="
    )
  )* conditional_expression
	;
	
conditional_expression:
  	logical_OR_expression ( "?" expression ":" conditional_expression )?
	;
	
constant_expression: conditional_expression
	;
	
logical_OR_expression:
  	logical_AND_expression ( "||" logical_AND_expression )*
	;
	
logical_AND_expression:
  	inclusive_OR_expression ( "&&" inclusive_OR_expression )*
	;
	
inclusive_OR_expression:
  	exclusive_OR_expression ( "|" exclusive_OR_expression )*
	;
	
exclusive_OR_expression:
  	AND_expression ( "^" AND_expression )*
	;

AND_expression:
  	equality_expression ( "&" equality_expression )*
	;
	
equality_expression:
  	relational_expression ( ("==" | "!=") relational_expression )*
	;
	
relational_expression:
  	shift_expression ( ("<" | ">" | "<=" | ">=") shift_expression )*
	;
	
shift_expression:
  	additive_expression ( ("<<" | ">>") additive_expression )*
	;
	
additive_expression:
  	multiplicative_expression ( ("+" | "-") multiplicative_expression )*
	;
	
multiplicative_expression:
  	cast_expression ( ("*" | "/" | "%") cast_expression )*
	;
	
cast_expression:
  	( "(" type_name ")" )* unary_expression
  	;

unary_expression:
  ("++" | "--" | "sizeof" ) * ( 
    "sizeof" "(" type_name ")"                           |
    ("&" | "*" | "+" | "-" | "~" | "!" ) cast_expression |
    postfix_expression
  )
	;

postfix_expression:
  (identifier | constant | string | "(" expression ")") (
    "[" expression "]"             |
    "(" assignment_expression<,> ")" |
    "." identifier                 |
    "->" identifier                |
    "++"                           |
    "--"
  )*
	;
	
constant: 
  integer_constant | 
  character_constant | 
  floating_constant | 
  enumeration_constant
	;