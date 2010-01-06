#ifndef CBNF_H
#define CBNF_H

#include "scanner.h"	// For token_info

// WARNING: If you add something here, you need to add it in the tokens[] array
//			(IN THE SAME ORDER) in cbnf.c!!
typedef enum token_type_tag	{
	UNKNOWN = 0,		/* Keep at value zero for easy if checks */
	SEMICOLON,
	COLON,
	PIPE,
	EQUALS,
	LPAREN,
	RPAREN,
	LBRACKET,
	RBRACKET,
	LBRACE,
	RBRACE,
	LANGLE,
	RANGLE,
	HYPHEN,
	COMMA,
	STAR,
	PLUS,
	QUESTION,
	STRING,
	ID,
	COMMENT,
	COMMENT_BLOCK,
	GRAMMAR_KEYWORD,
	EOT
} token_type;



#define UNKNOWN_NODE			0
#define GRAMMAR_NODE			1
#define GRAMMAR_DECL_NODE		2
#define RULE_NODE				3
#define RULEGROUP_NODE			4
#define RULEPART_NODE			5
#define OPTION_NODE				6
#define NONTERMINAL_NODE		7
#define TERMINAL_NODE			8

/**
 * Returns the english-readable name of the node given the node type.  Used mainly as a 
 * CB function to ast_pretty_print()
 */
const char* cbnf_node_type_to_string(int type);

#endif // CBNF_H