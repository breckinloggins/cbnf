#include "test_utils.h"
#include "../ast.h"
#include "../cbnf.h"

DEFINE_TEST_FUNCTION {	
	ast_node* node = ast_new_node(GRAMMAR_NODE);
	ast_append_string(node, "EBNF");
	ast_append_string(node, "FOO");
	
	ast_node* rule = ast_new_node(RULE_NODE);
	ast_append_string(rule, "Rule1");
	
	ast_append_node(node, rule);
	
	ast_pretty_print(stdout, node, cbnf_node_type_to_string);
	ast_free_node(node);
	
	return 0;
}

int main(int argc, char** argv)	{
	RUN_TEST;
}
