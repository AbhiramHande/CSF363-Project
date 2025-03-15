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

// Function Declarations for all functions of the parser
non_terminal* find_non_terminal(const char* name, int end);
void add_to_first_set(non_terminal* nt, const token_type element);
void add_to_follow_set(non_terminal* nt, const token_type element);
void compute_first_set(non_terminal* nt);
void compute_follow_set(non_terminal* nt, non_terminal* A, production* aToAlpha, int nt_pos);
token_type* compute_first_of_sequence(symbol** sym_seq, int sym_seq_count, int* result_size);
bool contains_EPS(token_type* set, int size);
void add_to_set(token_type** set, int* size, const token_type element);
void print_first_and_follow_sets(bool print_first, bool print_follow);
void print_parse_tree_inorder();
void print_parse_map();
void print_parse_tree_inorder(node* root, node* parent);
void generate_parse_table();
void generate_parse_map();
node* generate_parse_tree(FILE* src_code);
node* parse_code(char* grammar_file, FILE* input_file);
void parse_tree_cleanup(node** root);

#endif