#include "cbnf.h"

token_info tokens[] = {
	/* IMPORTANT: These MUST be in the same order as they are defined in the 
				  enum!
	*/
	/*TYPE                  TYPE NAME           TOKEN LITERAL */
	{ UNKNOWN,				"UNKNOWN",			0			},
	{ SEMICOLON,			"SEMICOLON",		";"			},
	{ COLON,				"COLON",			":"			},
	{ PIPE,					"PIPE",				"|"			},
	{ EQUALS,				"EQUALS",			"="			},
	{ LPAREN,				"LPAREN",			"("			},
	{ RPAREN,				"RPAREN",			")"			},
	{ LBRACKET,				"LBRACKET",			"["			},
	{ RBRACKET,				"RBRACKET",			"]"			},
	{ LBRACE,				"LBRACE",			"{"			},
	{ RBRACE,				"RBRACE",			"}"			},
	{ LANGLE, 				"LANGLE",			"<"			},
	{ RANGLE,				"RANGLE",			">"			},
	{ HYPHEN,				"HYPHEN",			"-"			},
	{ COMMA,				"COMMA",			","			},
	{ STAR,					"STAR",				"*"			},
	{ PLUS,					"PLUS",				"+"			},
	{ QUESTION,				"QUESTION",			"?"			},
	{ STRING,				"STRING",			0			},
	{ ID,					"ID",				0			},	
	{ COMMENT,				"COMMENT",			"//"		},
	{ COMMENT_BLOCK,		"COMMENT_BLOCK",	"/*"		},
	{ GRAMMAR_KEYWORD,		"GRAMMAR_KEYWORD",	"grammar"	},
	{ EOT,					"EOT",				0			}	// MUST be last in array!
};


/**
 * Returns the english-readable name of the node given the node type.  Used mainly as a 
 * CB function to ast_pretty_print()
 */
const char* cbnf_node_type_to_string(int type)	{

	switch(type)	{
		case UNKNOWN_NODE: 				return "UNKNOWN";
		case GRAMMAR_NODE: 				return "GRAMMAR";
		case GRAMMAR_DECL_NODE:			return "GRAMMAR_DECL";
		case RULE_NODE:					return "RULE"	; 
		case RULEGROUP_NODE:			return "RULEGROUP";
		case RULEPART_NODE:				return "RULEPART";
		case OPTION_NODE:				return "OPTION";
		case NONTERMINAL_NODE:			return "NONTERMINAL";	
		case TERMINAL_NODE:				return "TERMINAL";	
		default:						return "(unhandled)";
	}
}
