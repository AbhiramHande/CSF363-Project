#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "stack.h"
#include "lexer.h"
#include "hash_map.h"
#include "parserDef.h"

/*****************************************************************************
 *                                                                           *
 *                            PARSER FUNCTIONS                               *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Parses the given code based on the specified grammar.
 * 
 * @param grammar_file A string containing the path to the grammar file.
 * @param input_file A pointer to an opened file containing the source code.
 * 
 * @return A pointer to the root node of the parse tree.
 * 
 * @warning The caller must invoke `parse_tree_cleanup` to free all allocated memory.
 */
node* parse_code(char* grammar_file, FILE* input_file);

/**
 * @brief Deallocates all memory associated with the parse tree.
 * 
 * @param root A pointer to the pointer of the root node.
 * 
 * @post After execution, `*root` will be set to `NULL`.
 */
void parse_tree_cleanup(node** root);

/**
 * @brief Prints the parse tree using in-order traversal in the specified file.
 * 
 * @param root A pointer to the current node.
 * @param parent A pointer to the parent of the current node, NULL for the root.
 * @param output_file A pointer to an opened file where the tree will be printed.
 */
void print_parse_tree_inorder(node* root, node* parent, FILE* output_file);

#endif