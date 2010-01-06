#include "ast.h"

/**
 * Represents one node value in an AST Node.  The tag field specifies which type the 
 * variant "val" contains
 */
typedef struct node_value_tag	{
	// STOP!  Don't put a field before the tag!  See the note on node_query_iter below
	enum { UNKNOWN_TYPE, NODE_TYPE, STRING_TYPE } tag;
	
	union	{
		char*					string;
		struct ast_node_tag*	node;
	} val;
	
	struct ast_node_tag* parent;
	struct node_value_tag* next;
} node_value;

/**
 * Structure recording an iterator search into the tree
 */
typedef struct node_iter_tag	{
	enum { NODE_QUERY_ITER = -1 } tag;					// This is BLATANT abuse of the C "type" system
														// but what we're doing is making sure the 
														// node_query_iter structure has an identical
														// field to the node_value structure.  This way
														// we can do a relatively safe runtime type check
	ast_node* node;
	node_value* value;
	
	struct node_iter_tag *prev;
	int visited_hint;
	int query_node_type;
} node_query_iter;

static node_iter* ast_query_next(node_query_iter* iter);

/** 
 * Helper function: Appends the given node_value to the end of the node list
 */
static void ast_append_node_value(ast_node* node, node_value* v)	{
	node_value* cur;
	
	/* Find the end of the value list */
	cur = node->first_value;
	while (cur && cur->next)	{
		cur = cur->next;
	}
	
	if (cur)	{
		cur->next = v;
	} else {
		node->first_value = v;
	}
}

/**
 * Makes a new AST node with the given type
 */
ast_node* ast_new_node(int type)	{
	ast_node* node;
	
	node = (ast_node*)malloc(sizeof(ast_node));
	memset(node, 0, sizeof(ast_node));
	node->type = type;
	
	return node;
}

/** 
 * Appends a string to the list node
 */
void ast_append_string(ast_node* node, const char* string)	{
	node_value* v;
		
	v = (node_value*)malloc(sizeof(node_value));
	memset(v, 0, sizeof(node_value));
	
	v->tag = STRING_TYPE;
	v->parent = node;
	v->val.string = (char*)malloc(strlen(string)+1);
	strcpy(v->val.string, string);
	
	ast_append_node_value(node, v);
}

/** 
 * Appends an ast_node to the parent list node
 */
void ast_append_node(ast_node* node, ast_node* child)	{
	node_value* v;
		
	v = (node_value*)malloc(sizeof(node_value));
	memset(v, 0, sizeof(node_value));
	
	v->tag = NODE_TYPE;
	v->parent = node;
	v->val.node = child;
	child->parent = node;
	
	ast_append_node_value(node, v);
}

/**
 * Begin iterating through a node's children
 */
node_iter* ast_iter_begin(ast_node* node)	{
	if (!node)	{
		return 0;
	}
	
	return (node_iter*)node->first_value;
}

/**
 * Helper function - actual implementation of ast_query_begin but with extra parameter to 
 * 					point to previous iterator
 */
node_iter* ast_query_begin_internal(ast_node* node, int node_type, node_query_iter* prev)	{
	node_query_iter* qi;
	
	if (!node)	{
		return 0;
	}
	
	// The sole purpose of this function is to allocate a new query_iter and 
	// call query next on it.  ALL of the actual iteration logic is performed in
	// ast_query_next
	qi = (node_query_iter*)malloc(sizeof(node_query_iter));
	qi->tag = NODE_QUERY_ITER;
	qi->query_node_type = node_type;
	qi->node = node;
	qi->prev = prev;
	qi->visited_hint = 0;
	qi->value = 0;
		
	return ast_query_next(qi);	
}

/**
 * Starts a query to find all nodes of a given type.  Returns the first result
 * as an iterator.  Find the next result by calling ast_query_next with the iterator
 *
 * Returns 0 if no results were found
 */ 
node_iter* ast_query_begin(ast_node* node, int node_type)	{
	return ast_query_begin_internal(node, node_type, 0);
}

/**
 * Continues a query at the next result by the given iterator
 *
 * Returns 0 if no more results were found
 */
node_iter* ast_query_next(node_query_iter* qi)	{
	node_query_iter* new_qi;
	
	if (!qi->visited_hint) {
		do {
			if (!qi->value)	{
				// This is a "fresh" iterator, straight from the ast_begin_query oven, 
				// so let's give it a place to start, 'eh?
				qi->value = qi->node->first_value;
			} else {
				qi->value = qi->value->next;
			}
		} while (qi->value && qi->value->tag != NODE_TYPE);	// This skips over scalar node_values
	}
	
	if (!qi->value)	{
		// We're at the end.  It's time to go back up to the previous iterator...
		if (!qi->prev)	{
			// ... unless we're already at the top, in which case we're done, no more!
			free(qi);
			return 0;
		} else {
			new_qi = ast_query_next(qi->prev);
			free(qi);
			return new_qi;
		}
	}
	
	// If we got this far, we still have at least one more node_value we need to examine
	if (!qi->visited_hint && qi->value->val.node->type == qi->query_node_type)	{
		// This one matches, but before we go, set up a "hint" that we need to just go down next time
		qi->visited_hint = 1;
		return qi;
	}
	
	// It's time to recurse down a level
	qi->visited_hint = 0;
	return ast_query_begin_internal(qi->value->val.node, qi->query_node_type, qi);
}

/**
 * Iterate to the next child given the current iterator.  
 * 
 * This method returns 0 when at the end of the list
 */
node_iter* ast_iter_next(node_iter* iter)	{
	if (((node_query_iter*)iter)->tag == NODE_QUERY_ITER)	{
		// We have a query iterator, not a normal iterator
		return ast_query_next((node_query_iter*)iter);
	}
	
	return (node_iter*)((node_value*)iter)->next;
}

/**
 * To avoid resource leaks, always call ast_iter_free on the last non-zer
 * iterator you hold
 */
void ast_iter_free(node_iter* iter)	{
	if (!iter)	{
		return;
	}
	
	if (((node_query_iter*)iter)->tag != NODE_QUERY_ITER)	{
		// We have  a normal iterator
		return;
	}
	
	ast_iter_free(((node_query_iter*)iter)->prev);
	
	free((node_query_iter*)iter);
}


/**
 * Given a child iterator, returns that child's value as a string
 *
 * Returns 0 if the child is not a string
 */
const char* ast_get_string(node_iter* iter)	{
	node_value* v;
	v = 0;
	
	// It could be a node_value
	if (((node_value*)iter)->tag == -1)	{
		// Not possible for a node_value, it's a node_query_iter
		v = ((node_query_iter*)iter)->value;
	} else {
		// Gotta be a node_value
		v = (node_value*)iter;
	}
		
	if (v->tag != STRING_TYPE)	{
		return 0;
	}
	
	return v->val.string;
}

/*
 * Given a child iterator, returns that child's value as an AST_NODE
 *
 * Returns 0 if the child is not an AST_NODE
 */
ast_node* ast_get_node(node_iter* iter)	{
	node_value* v;
	v = 0;
	
	// It could be a node_value
	if (((node_value*)iter)->tag == -1)	{
		// Not possible for a node_value, it's a node_query_iter
		v = ((node_query_iter*)iter)->value;
	} else {
		// Gotta be a node_value
		v = (node_value*)iter;
	}
	
	if (v->tag != NODE_TYPE)	{
		return 0;
	}
	
	return v->val.node;
}

/*
 * Reclaims all memory occupied by the given node value including, if applicable
 * the next node in the link
 */
void ast_free_node_value(node_value* value)	{
	node_value* next = value->next;
	if (next)	{
		ast_free_node_value(next);
	}
	
	if (value->tag == STRING_TYPE)	{
		free(value->val.string);
	} else if (value->tag == NODE_TYPE)	{
		ast_free_node(value->val.node);
	}
	
	free(value);
}

/*
 * Reclaims all memory occupied by the given ast node
 */
void ast_free_node(ast_node* node)	{
	if (node->first_value)	{
		ast_free_node_value(node->first_value);
	}
	
	free(node);
}

#define TAB(x) for(i = 0; i < x; i++) fprintf(fd, "\t")

/* Prototypes for mutually recursive helper functions */
static void ast_pretty_print_node(FILE* fd, ast_node* node, int indent_level, const char* (*get_type_name_cb)(int type));
static void ast_pretty_print_node_value(FILE* fd, node_value* val, int indent_level, const char* (*get_type_name_cb)(int type));

/*
 * Print the complete AST starting from the given node in LISP format.
 * Ex: (BINOP '+' 3 
			(BINOP '*' 1 2)
		)
 */
void ast_pretty_print(FILE* fd, ast_node* node, const char* (*get_type_name_cb)(int type))	{
	ast_pretty_print_node(fd, node, 0, get_type_name_cb);
	fprintf(fd, "\n");
}

/**
 * Pretty-prints an AST-NODE at the given indention level 
 */
static void ast_pretty_print_node(FILE* fd, ast_node* node, int indent_level, const char* (*get_type_name_cb)(int type))	{
	int i;	// For TAB()
	const char* name;
	node_value* val;
	
	if (get_type_name_cb)	{
		name = get_type_name_cb(node->type);
	} else {
		name = "(unknown)";
	}
	
	fprintf(fd, "(%s", name);
	
	val = node->first_value;
	while (val)	{
		if (val->tag == NODE_TYPE)	{
			fprintf(fd, "\n");
			TAB(indent_level+1);
		} else {
			fprintf(fd, " ");
		}
		ast_pretty_print_node_value(fd, val, indent_level+1, get_type_name_cb);
		val = val->next;
	}
	fprintf(fd, ")");
}

/**
 * Pretty-prints an AST node value at the given indention level
 */
static void ast_pretty_print_node_value(FILE* fd, node_value* val, int indent_level, const char* (*get_type_name_cb)(int type))	{
	switch(val->tag)	{
		case STRING_TYPE:
			fprintf(fd, "%s", val->val.string);
			break;
		case NODE_TYPE:
			ast_pretty_print_node(fd, val->val.node, indent_level, get_type_name_cb);
			break;
		default: fprintf(fd, "(unknown) ");
	}
}


