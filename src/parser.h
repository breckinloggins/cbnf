#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include "ast.h"

ast_node* parse_grammar(FILE* in);

#endif // PARSER_H