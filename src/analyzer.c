/** 
 * Semantic Analyzer for the Homogeneous AST built by the parser
 *
 */

#include <ctype.h>

#include "cbnf.h"
#include "ast.h"
#include "analyzer.h"
#include "../lib/libuseful/src/include/hashtable.h"

static hashtable* rules = 0;

int is_uppercase(const char* str)	{
	char* ptr;
	
	ptr = (char*)str;
	while (*ptr != '\0')	{
		if (!isupper(*ptr) && *ptr != '_')	{
			return 0;
		}
		
		ptr++;
	}
	
	return 1;
}

void check_rules(ast_node* root)	{
	ast_node* cur;
	int rule_count;
	
	// We know the structure of the language.  All RULE nodes will be children of the root node
	cur = root;
	if (cur->type != GRAMMAR_NODE)	{
		fprintf(stderr, "Fatal Error: Root node was not a GRAMMAR node\n");
		exit(1);
	}
	
	rule_count = 0;
	node_iter* it = ast_iter_begin(cur);
	while (it)	{
		ast_node* node = ast_get_node(it);
		if (node && node->type == RULE_NODE)	{
			node_iter* rule_iter = ast_iter_begin(node);
			
			rule_count++;
			if (ht_insert(rules, ast_get_string(rule_iter)) != 0)	{
				fprintf(stdout, "Error: duplicate rule in grammar (%s)\n", ast_get_string(rule_iter));
				exit(1);
			}
		}
		
		it = ast_iter_next(it);
	}	
}

void check_nonterminals(ast_node* root)	{
	node_iter* it;
	node_iter* nt_iter;
	char* val;
	
	it = ast_query_begin(root, NONTERMINAL_NODE);
	while (it)	{
		nt_iter = ast_iter_begin(ast_get_node(it));
		val = (char*)ast_get_string(nt_iter);
		if (ht_lookup(rules, (void*)&val) != 0 && !is_uppercase(val))	{
			fprintf(stdout, "Error: nonterminal %s is not defined\n", val);
			//exit(1);
		}
		
		it = ast_iter_next(it);
	
	}
}

void analyze(struct ast_node_tag* node)	{
	rules = (hashtable*)malloc(sizeof(hashtable));
	ht_init(rules, 1699, 0, 0, 0);
	
	check_rules(node);
	check_nonterminals(node);
	
	ht_destroy(rules);
	free(rules);
}