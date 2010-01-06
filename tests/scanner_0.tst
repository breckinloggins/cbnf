/*
 * This is a multiline comment, it should read as a "COMMENT_BLOCK" to the
 * scanner.  None of the special symbols in this comment should be treated as
 * such! Special symbols: ({[|;=-,*+?]})
 */

// This is a rule
start = rule1 ;

// AAANND... this is another rule
rule1 = 
	'B' |
	'C' | 
	rule2
	;
	
rule2 = "a double quoted string" 
	| a_rule_with_underscores ;
	
rule3="a rule without any spaces between stuff"|/*comment here*/rule4;