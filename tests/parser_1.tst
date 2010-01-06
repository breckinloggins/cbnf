grammar EBNF;

rule1: ;	/* Blank rules are allowed */

rule2:	 	foo bar "baz" 
		|	bat "fat"
		;
		
rule3: abc "def" g ;