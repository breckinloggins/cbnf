#include "cbnf.h"
#include "parser.h"
#include "analyzer.h"
#include "ast.h"
#include "codegen.h"
#include <string.h>

int main(int argc, char** argv)	{
    char* output;
    
	if (argc != 2)	{
		fprintf(stderr, "USAGE: cbnf <grammar file>\n");
		return 1;
	}
	
	FILE* fp = fopen(argv[1], "r");
	
	ast_node* root = parse_grammar(fp);
    ast_pretty_print(stdout, root, cbnf_node_type_to_string);
	
	analyze(root);
	
    if (codegen_run(root, &output) == 0)    {
	    fprintf(stdout, "\n\n%s\n", output);
    }
	
	ast_free_node(root);
	
	fclose(fp);
	return 0;
}