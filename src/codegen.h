/**
 * Generates the parser code from the AST
 */
#ifndef CODEGEN_H
#define CODEGEN_H

/**
 * Converts the AST into code by applying the code templates
 */
int codegen_run(ast_node* root, char** output);

#endif // CODEGEN_H