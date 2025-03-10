#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "hash_map.h"
#include "parserDef.h"

non_terminal* find_non_terminal(const char* name);
void add_to_first_set(non_terminal* nt, const TokenName element);
void add_to_follow_set(non_terminal* nt, const TokenName element);
void compute_first_set(non_terminal* nt);
void compute_follow_set(non_terminal* nt, non_terminal* A, production* aToAlpha, int nt_pos);
TokenName* compute_first_of_sequence(symbol** sym_seq, int sym_seq_count, int* result_size);
bool contains_EPS(TokenName* set, int size);
void add_to_set(TokenName** set, int* size, const TokenName element);
void print_first_and_follow_sets(bool print_first, bool print_follow);
void print_parse_tree();
void print_parse_map();
void generate_parse_table();
void generate_parse_map();

#endif