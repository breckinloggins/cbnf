#include "../useful/test_utils.h"
#include "../cbnf.h"
#include "../scanner.h"
#include "../parser.h"

DEFINE_TEST_FUNCTION {	
	ast_node* root = parse_grammar(in);
	ast_pretty_print(out, root, cbnf_node_type_to_string);
	ast_free_node(root);
	
	return 0;
}

int main(int argc, char** argv)	{
	RUN_TEST;
}