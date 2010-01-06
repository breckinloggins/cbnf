#include "cbnf.h"
#include "scanner.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>

#define DBG_ENABLED 	0
#define DBG(x)	if(DBG_ENABLED)	{fprintf(stderr, x);}

static token* this = 0;
static FILE* fd = 0;

/**
 * Allocates and initializes a new token on the heap
 */
static token* new_token()	{
	// TODO: We're taking up way too much time doing allocations here.  We should have a 
	// "token pool" so we can allocate a bunch of them at a time, and only make more when 
	// we're starting to run low
	token* t = (token*)malloc(sizeof(token));
	memset(t, 0, sizeof(token));
	
	return t;
}

/**
 * Helper function.  Makes a new token. 
 * fills it from the scanner, and sets it to the current token, ignoring the token 
 * types the parser doesn't currently care about
 */
static void next_token()	{
	//TODO: We are not keeping track of any of these tokens as we malloc them, we should 
	//		do so so we can free the memory they consumed
	
	token* t;
	while (1)	{
		t = new_token();
		get_token(fd, t);	
		if (t->type != COMMENT && t->type != COMMENT_BLOCK)	{
			// We've found a token we care about
			break;
		}
		
		// We don't care about this one
		free(t);
	}
	this = t;
}

/**
 * Unconditionally accepts the current token and fetches a new one
 */
static token* accept_this()	{
	token* last = this;
	next_token();
	
	return last;
}

/**
 * Accepts the current token iff it is of the given type, returns token ptr if the token 
 * is accepted, zero otherwise
 */
static token* accept(token_type type)	{
	token* last;
	if (this != 0 && this->type == type)	{
		last = this;
		next_token();
		return last;
	}
	
	return 0;
}

/**
 * Accepts the token if it is of the given type, or throws a syntax error otherwise
 */
static token* expect(token_type type)	{
	token* ret;
	if (!(ret = accept(type)))	{
		ret = 0;
		fprintf(stderr, "Syntax error: Expected %s but got %s (%s)\n", 
			get_token_name(type), get_token_name(this->type), this->string);
		exit(1);
	}
	
	return ret;
}

/* Prototypes defined first, some functions are mutually recursive */
static ast_node* parse_grammar_decl();
static ast_node* parse_rulepart();
static ast_node* parse_option();

ast_node* parse_grammar(FILE* in)	{
	ast_node* root, * tmp;
	token* id;	// For the rules
	
	fd = in;
	next_token();	// Starts the read
	
	root = ast_new_node(GRAMMAR_NODE);
		
	tmp = parse_grammar_decl();
	ast_append_node(root, tmp);
	
	while ((id = accept(ID)) != 0)	{
		expect(COLON);
		
		ast_node* rule = ast_new_node(RULE_NODE);
		do {
			tmp = parse_rulepart();
			ast_append_string(rule, id->string);
			ast_append_node(rule, tmp);
		} while (accept(PIPE)) ;
		expect(SEMICOLON);
	
		ast_append_node(root, rule);
		
		DBG("Parsed RULE\n");
	}
	
	expect(EOT);
	
	DBG("Parsed GRAMMAR\n");
	return root;
}

static ast_node* parse_grammar_decl()	{
	token* id_token;
	ast_node* node;
	
	expect(GRAMMAR_KEYWORD);
	id_token = expect(ID);
	expect(SEMICOLON);
	
	node = ast_new_node(GRAMMAR_DECL_NODE);
	ast_append_string(node, id_token->string);
	
	DBG("Parsed GRAMMAR_DECL\n");	
	return node;
}

static ast_node* parse_rulepart()	{
	token* t;
	ast_node* node, *tmp, *rgroup, *opt;
	
	node = ast_new_node(RULEPART_NODE);
	while ((t = accept(ID)) || (t = accept(STRING)) || (t = accept(LPAREN)))	{
		if (t->type == ID)	{
			tmp = ast_new_node(NONTERMINAL_NODE);
			ast_append_string(tmp, t->string);
			
			opt = parse_option();
			if (opt)	{
				ast_append_node(tmp, opt);
			}

			ast_append_node(node, tmp);
			
		} else if (t->type == STRING) {
			tmp = ast_new_node(TERMINAL_NODE);
			ast_append_string(tmp, t->string);
			
			opt = parse_option();
			if (opt)	{
				ast_append_node(tmp, opt);
			}
			
			ast_append_node(node, tmp);
			
		} else if (t->type == LPAREN)	{
			rgroup = ast_new_node(RULEGROUP_NODE);
			
			do {
				tmp = parse_rulepart();
				ast_append_node(rgroup, tmp);
				
			} while (accept(PIPE));
			
			expect(RPAREN);	// Better get a matchine PAREN!
			
			opt = parse_option();
			if (opt)	{
				ast_append_node(rgroup, opt);
			}
			
			ast_append_node(node, rgroup);
		} else {
			fprintf(stderr, "Fatal: Forgot to define a case in parse_rulepart()\n");
			exit(1);
		}
	}
	
	DBG("Parsed RULEPART\n");
	return node;
}

static ast_node* parse_option()	{
	ast_node* node;
	token* t;
	
	if ((t = accept(STAR)) || 
		(t = accept(QUESTION)) ||
		(t = accept(PLUS)))	{
		
		node = ast_new_node(OPTION_NODE);
		ast_append_string(node, t->string);	
	} else if ((t = accept(LANGLE)))	{
		
		node = ast_new_node(OPTION_NODE);
		
		// TODO: Be able to accept multiple tokens here
		t = accept_this();
		
		expect(RANGLE);
		
		ast_append_string(node, "+");
		ast_append_string(node, t->string);
		
		if (accept(QUESTION))	{
			// You can have an optional question mark at the end of a separator
			ast_append_string(node, "?");
		}
	} else {
		node = 0;
	}
	
	return node;
}




