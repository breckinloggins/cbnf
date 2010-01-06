/*
 * AST.H - A very small set of routines and structures for working with homogeneous abstract
 *			syntax trees.  The ASTs are "homogeneous" because all tree nodes have the same "shape", 
 *			being differentiated by the first child of each node being the type code
 */
#ifndef AST_H
#define AST_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Represents a node in the CBNF abstract syntax tree.  All AST nodes look like:
 * (NODETYPE [args]).  The args are the node_values and can be simple scalar types like
 * integers or strings, or themselves AST nodes.  Examples:
 *
 * 3 + 4 := (BINOP '+' 3 4)
 * 3 + (4 + x) := (BINOP '+' 3 *)
 *                             |
 *                             *-> (BINOP '+' 4 *)
 *                                              |
 *                                              *-> (VARIABLE 'x')
 * Or the more "lisp-y" looking:
 *    (BINOP '+' 3 (BINOP '+' 4 (VARIABLE 'x')))
 */
typedef struct ast_node_tag	{
	int type;					// NODE Types are defined externally
	struct ast_node_tag* parent;
	struct node_value_tag* first_value;
} ast_node;

typedef struct node_iter_tag node_iter;

/**
 * Makes a new AST node with the given type
 */
ast_node* ast_new_node(int type);

/**
 * Appends the given AST Node to the list
 */
void ast_append_node(ast_node* node, ast_node* child);

/** 
 * Appends the given string to the list
 */
void ast_append_string(ast_node* node, const char* string);

/**
 * Begin iterating through a node's children
 */
node_iter* ast_iter_begin(ast_node* node);

/**
 * To avoid resource leaks, always call ast_iter_free on the last non-zer
 * iterator you hold
 */
void ast_iter_free(node_iter* iter);

/**
 * Starts a query to find all nodes of a given type in a depth-first search starting at the given
 * node.  Returns the first result
 * as an iterator.  Find the next result by calling ast_iter_next with the iterator.
 *
 * Returns 0 if no results were found
 */ 
node_iter* ast_query_begin(ast_node* node, int node_type);

/**
 * Iterate to the next child given the current iterator.  
 * 
 * This method returns 0 when at the end of the list
 *
 * NOTE: After calling this method, do NOT assume the previous iterator is still valid!
 */
node_iter* ast_iter_next(node_iter* iter);

/**
 * Given a child iterator, returns that child's value as a string
 *
 * Returns 0 if the child is not a string
 */
const char* ast_get_string(node_iter* iter);

/*
 * Given a child iterator, returns that child's value as an AST_NODE
 *
 * Returns 0 if the child is not an AST_NODE
 */
ast_node* ast_get_node(node_iter* iter);

/*
 * Reclaims all memory occupied by the given ast node
 */
void ast_free_node(ast_node* node);

/*
 * Print the complete AST starting from the given node in LISP format.
 * Ex: (BINOP '+' 3 
			(BINOP '*' 1 2)
		)
 */
void ast_pretty_print(FILE* fd, ast_node* node, const char* (*get_type_name_cb)(int type));

#endif // AST_H