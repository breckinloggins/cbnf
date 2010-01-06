// From: http://infoether.com/~tom/java.bnf
grammar JAVA;

/*****************************************
 * THE JAVA LANGUAGE GRAMMAR STARTS HERE *
 *****************************************/

/*
 * Program structuring syntax follows.
 */

CompilationUnit   :   ( PackageDeclaration )? ( ImportDeclaration )* ( TypeDeclaration )+ ( "\u001a" )? ;
	
PackageDeclaration  :   Modifiers "package" Name ";" ;
ImportDeclaration   :   "import" ( "static" )? Name ( "." "*" )? ";" ;

/*
 * Modifiers. We match all modifiers in a single rule to reduce the chances of
 * syntax errors for simple modifier mistakes. It will also enable us to give
 * better error messages.
 */

Modifiers   :   ( ( "public" | "static" | "protected" | "private" | "final" | "abstract" | "synchronized" | "native" | "transient" | "volatile" | "strictfp" | Annotation ) )* ;

/*
 * Declaration syntax follows.
 */

TypeDeclaration   :   ";"
  |   Modifiers ( ClassOrInterfaceDeclaration | EnumDeclaration | AnnotationTypeDeclaration ) ;

ClassOrInterfaceDeclaration   :   ( "class" | "interface" ) IDENTIFIER ( TypeParameters )? ( ExtendsList )? ( ImplementsList )? ClassOrInterfaceBody ;

ExtendsList   :   "extends" ClassOrInterfaceType ( "," ClassOrInterfaceType )* ;

ImplementsList  :   "implements" ClassOrInterfaceType ( "," ClassOrInterfaceType )* ;

EnumDeclaration   :   "enum" IDENTIFIER ( ImplementsList )? EnumBody ;

EnumBody  :   "{" ( EnumConstant ( "," EnumConstant )* )? ( "," )? ( ";" ( ClassOrInterfaceBodyDeclaration )* )? "}" ;

EnumConstant  :   Modifiers IDENTIFIER ( Arguments )? ( ClassOrInterfaceBody )? ;

TypeParameters  :   "<" TypeParameter ( "," TypeParameter )* ">" ;

TypeParameter   :   IDENTIFIER ( TypeBound )? ;

TypeBound   :   "extends" ClassOrInterfaceType ( "&" ClassOrInterfaceType )* ;

ClassOrInterfaceBody  :   "{" ( ClassOrInterfaceBodyDeclaration )* "}" ;

ClassOrInterfaceBodyDeclaration   :   Initializer
  |   Modifiers ( ClassOrInterfaceDeclaration | EnumDeclaration | ConstructorDeclaration | FieldDeclaration | MethodDeclaration | AnnotationTypeDeclaration )
  |   ";" 
	;
	
FieldDeclaration  :   Type VariableDeclarator ( "," VariableDeclarator )* ";" ;

VariableDeclarator  :   VariableDeclaratorId ( "=" VariableInitializer )? ;

VariableDeclaratorId  :   IDENTIFIER ( "[" "]" )* ;

VariableInitializer   :   ArrayInitializer
  |   Expression
	;
	
ArrayInitializer  :   "{" ( VariableInitializer ( "," VariableInitializer )* )? ( "," )? "}" ;

MethodDeclaration   :   ( TypeParameters )? ResultType MethodDeclarator ( "throws" NameList )? ( Block | ";" ) 
	;
	
MethodDeclarator  :   IDENTIFIER FormalParameters ( "[" "]" )* ;

FormalParameters  :   "(" ( FormalParameter ( "," FormalParameter )* )? ")" ;

FormalParameter   :   Modifiers ( "final" | Annotation )? Type ( "..." )? VariableDeclaratorId ;

ConstructorDeclaration  :   ( TypeParameters )? IDENTIFIER FormalParameters ( "throws" NameList )? "{" ( ExplicitConstructorInvocation )? ( BlockStatement )* "}" 
	;

ExplicitConstructorInvocation   :   ( IDENTIFIER "." )* ( "this" "." )? ( TypeArguments )? ( "this" | "super" ) Arguments ";" ;

Initializer   :   ( "static" )? Block ;

/*
 * Type, name and expression syntax follows.
 */

Type  :   ReferenceType
  |   PrimitiveType
	;
	
ReferenceType   :   PrimitiveType ( "[" "]" )+
  |   ( ClassOrInterfaceType ) ( "[" "]" )*	;

ClassOrInterfaceType  :   IDENTIFIER ( TypeArguments )? ( "." IDENTIFIER ( TypeArguments )? )* ;

TypeArguments   :   "<" TypeArgument ( "," TypeArgument )* ">" ;

TypeArgument  :   ReferenceType
  	|   "?" ( WildcardBounds )?
	;
	
WildcardBounds  :   "extends" ReferenceType
  |   "super" ReferenceType
	;
	
PrimitiveType   :   "boolean"
  |   "char"
  |   "byte"
  |   "short"
  |   "int"
  |   "long"
  |   "float"
  |   "double"
	;
	
ResultType  :   "void"
  |   Type
	;
	
Name  :   IDENTIFIER ( "." IDENTIFIER )* ;

NameList  :   Name ( "," Name )* ;

/*
 * Expression syntax follows.
 */

Expression  :   ConditionalExpression ( AssignmentOperator Expression )? ;

AssignmentOperator  :   "="
  |   "*="
  |   "/="
  |   "%="
  |   "+="
  |   "-="
  |   "<<="
  |   ">>="
  |   ">>>="
  |   "&="
  |   "^="
  |   "|="
	;
	
ConditionalExpression   :   ConditionalOrExpression ( "?" Expression ":" Expression )? ;

ConditionalOrExpression   :   ConditionalAndExpression ( "||" ConditionalAndExpression )* ;

ConditionalAndExpression  :   InclusiveOrExpression ( "&&" InclusiveOrExpression )* ;

InclusiveOrExpression   :   ExclusiveOrExpression ( "|" ExclusiveOrExpression )* ;

ExclusiveOrExpression   :   AndExpression ( "^" AndExpression )* ;

AndExpression   :   EqualityExpression ( "&" EqualityExpression )* ;

EqualityExpression  :   InstanceOfExpression ( ( "==" | "!=" ) InstanceOfExpression )* ;

InstanceOfExpression  :   RelationalExpression ( "instanceof" Type )? ;

RelationalExpression  :   ShiftExpression ( ( "<" | ">" | "<=" | ">=" ) ShiftExpression )* ;

ShiftExpression   :   AdditiveExpression ( ( "<<" | RSIGNEDSHIFT | RUNSIGNEDSHIFT ) AdditiveExpression )* ;

AdditiveExpression  :   MultiplicativeExpression ( ( "+" | "-" ) MultiplicativeExpression )* ;

MultiplicativeExpression  :   UnaryExpression ( ( "*" | "/" | "%" ) UnaryExpression )* ;

UnaryExpression   :   ( "+" | "-" ) UnaryExpression
  |   PreIncrementExpression
  |   PreDecrementExpression
  |   UnaryExpressionNotPlusMinus
	;
	
PreIncrementExpression  :   "++" PrimaryExpression ;

PreDecrementExpression  :   "--" PrimaryExpression ;

UnaryExpressionNotPlusMinus   :   ( "~" | "!" ) UnaryExpression
  |   CastExpression
  |   PostfixExpression
	;
	
// This production is to determine lookahead only.  The LOOKAHEAD specifications
// below are not used, but they are there just to indicate that we know about
// this.
CastLookahead   :   "(" PrimitiveType
  |   "(" Type "[" "]"
  |   "(" Type ")" ( "~" | "!" | "(" | IDENTIFIER | "this" | "super" | "new" | Literal )
	;
	
PostfixExpression   :   PrimaryExpression ( "++" | "--" )? ;

CastExpression  :   "(" Type ")" UnaryExpression
  |   "(" Type ")" UnaryExpressionNotPlusMinus
	;
	
PrimaryExpression   :   PrimaryPrefix ( PrimarySuffix )* ;

MemberSelector  :   "." TypeArguments IDENTIFIER ;

PrimaryPrefix   :   Literal
  |   ( IDENTIFIER "." )* "this"
  |   "super" "." IDENTIFIER
  |   ClassOrInterfaceType "." "super" "." IDENTIFIER
  |   "(" Expression ")"
  |   AllocationExpression
  |   ResultType "." "class"
  |   Name
	;
	
PrimarySuffix   :   "." "super"
  |   "." "this"
  |   "." AllocationExpression
  |   MemberSelector
  |   "[" Expression "]"
  |   "." IDENTIFIER
  |   Arguments
	;
	
Literal   :   INTEGER_LITERAL
  |   FLOATING_POINT_LITERAL
  |   CHARACTER_LITERAL
  |   STRING_LITERAL
  |   BooleanLiteral
  |   NullLiteral
	;
	
BooleanLiteral  :   "true"
  |   "false"
	;
	
NullLiteral   :   "null" ;

Arguments   :   "(" ( ArgumentList )? ")" ;

ArgumentList  :   Expression ( "," Expression )* ;

AllocationExpression  :   "new" PrimitiveType ArrayDimsAndInits
  |   "new" ClassOrInterfaceType ( TypeArguments )? ( ArrayDimsAndInits | Arguments ( ClassOrInterfaceBody )? ) 
	;

/*
 * The third LOOKAHEAD specification below is to parse to PrimarySuffix
 * if there is an expression between the "[...]".
 */

ArrayDimsAndInits   :   ( "[" Expression "]" )+ ( "[" "]" )*
  |   ( "[" "]" )+ ArrayInitializer
	;
	
/*
 * Statement syntax follows.
 */

Statement   :   LabeledStatement
  |   AssertStatement
  |   Block
  |   EmptyStatement
  |   StatementExpression ";"
  |   SwitchStatement
  |   IfStatement
  |   WhileStatement
  |   DoStatement
  |   ForStatement
  |   BreakStatement
  |   ContinueStatement
  |   ReturnStatement
  |   ThrowStatement
  |   SynchronizedStatement
  |   TryStatement
	;
	
AssertStatement   :   "assert" Expression ( ":" Expression )? ";" ;

LabeledStatement  :   IDENTIFIER ":" Statement ;

Block   :   "{" ( BlockStatement )* "}" ;

BlockStatement  :   LocalVariableDeclaration ";"
  |   Statement
  |   ClassOrInterfaceDeclaration
	;
	
LocalVariableDeclaration  :   Modifiers Type VariableDeclarator ( "," VariableDeclarator )* ;

EmptyStatement  :   ";" ;

StatementExpression   :   PreIncrementExpression
  |   PreDecrementExpression
  |   PrimaryExpression ( "++" | "--" | AssignmentOperator Expression )?
	;
	
SwitchStatement   :   "switch" "(" Expression ")" "{" ( SwitchLabel ( BlockStatement )* )* "}" ;

SwitchLabel   :   "case" Expression ":"
  |   "default" ":"
	;
	
IfStatement   :   "if" "(" Expression ")" Statement ( "else" Statement )? ;

WhileStatement  :   "while" "(" Expression ")" Statement ;

DoStatement   :   "do" Statement "while" "(" Expression ")" ";" ;

ForStatement  :   "for" "(" ( Modifiers Type IDENTIFIER ":" Expression | ( ForInit )? ";" ( Expression )? ";" ( ForUpdate )? ) ")" Statement
	;
	
ForInit   :   LocalVariableDeclaration
  |   StatementExpressionList
	;
	
StatementExpressionList   :   StatementExpression ( "," StatementExpression )* ;

ForUpdate   :   StatementExpressionList ;

BreakStatement  :   "break" ( IDENTIFIER )? ";" ;

ContinueStatement   :   "continue" ( IDENTIFIER )? ";" ;

ReturnStatement   :   "return" ( Expression )? ";" ;

ThrowStatement  :   "throw" Expression ";" ;

SynchronizedStatement   :   "synchronized" "(" Expression ")" Block ;

TryStatement  :   "try" Block ( "catch" "(" FormalParameter ")" Block )* ( "finally" Block )? ;

/* We use productions to match >>>, >> and > so that we can keep the
 * type declaration syntax with generics clean
 */

RUNSIGNEDSHIFT  :   ( ">" ">" ">" ) ;
RSIGNEDSHIFT  :   ( ">" ">" ) ;

/* Annotation syntax follows. */

Annotation  :   NormalAnnotation
  |   SingleMemberAnnotation
  |   MarkerAnnotation
	;
	
NormalAnnotation  :   "@" Name "(" ( MemberValuePairs )? ")" ;

MarkerAnnotation  :   "@" Name ;

SingleMemberAnnotation  :   "@" Name "(" MemberValue ")" ;

MemberValuePairs  :   MemberValuePair ( "," MemberValuePair )* ;

MemberValuePair   :   IDENTIFIER "=" MemberValue ;

MemberValue   :   Annotation
  |   MemberValueArrayInitializer
  |   ConditionalExpression
	 ;
	
MemberValueArrayInitializer   :   "{" ( MemberValue ( "," MemberValue )* ( "," )? )? "}" ;

/* Annotation Types. */

AnnotationTypeDeclaration   :   "@" "interface" IDENTIFIER AnnotationTypeBody ;

AnnotationTypeBody  :   "{" ( AnnotationTypeMemberDeclaration )* "}" ;

AnnotationTypeMemberDeclaration   :   Modifiers ( Type IDENTIFIER "(" ")" ( DefaultValue )? ";" | ClassOrInterfaceDeclaration | EnumDeclaration | AnnotationTypeDeclaration | FieldDeclaration )
  |   ( ";" )
	;
	
DefaultValue  :   "default" MemberValue ;