#include <stdio.h>

#include "ngtemplate.h"
#include "ast.h"
#include "cbnf.h"

/**
 * Converts the AST into code by applying the code templates
 */
int codegen_run(ast_node* root, char** output)    {
    ngt_template* template;
    ngt_dictionary* dictionary, *section;
    node_iter* it, *val_it;
    char* val;
    
    template = ngt_new();
    dictionary = ngt_dictionary_new();
    
    it = ast_query_begin(root, GRAMMAR_DECL_NODE);
    val_it = ast_iter_begin(ast_get_node(it));
    ngt_set_string(dictionary, "Grammar", ast_get_string(val_it));
    
    it = ast_query_begin(root, RULE_NODE);
	while (it)	{
        val_it = ast_iter_begin(ast_get_node(it));
		val = (char*)ast_get_string(val_it);
        
        /* Prototype Declarations */
        section = ngt_dictionary_new();
        ngt_set_string(section, "Name", val);
        ngt_add_dictionary(dictionary, "Rule_Prototype", section, NGT_SECTION_VISIBLE);
        
        /* Definitions */
        section = ngt_dictionary_new();
        ngt_set_string(section, "Name", val);
        ngt_add_dictionary(dictionary, "Rule", section, NGT_SECTION_VISIBLE);
		
		it = ast_iter_next(it);
	
	}
    
    /* TODO: Use ngtembed */
    if (ngt_load_from_filename(template, "../src/templates/parser.tpl") != 0)   {
        fprintf(stderr, "FATAL: Could not load template file\n");
        return -1;
    }
    
    ngt_set_dictionary(template, dictionary);
    ngt_set_delimiters(template, "@", "@");
    ngt_expand(template, output);
        
    ngt_destroy(template);
    ngt_dictionary_destroy(dictionary);
    
    return 0;
}
