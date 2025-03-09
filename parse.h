#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "parserDef.h"

non_terminal* find_non_terminal(const char* name);
void add_to_first_set(non_terminal* nt, const char* element);
void add_to_follow_set(non_terminal* nt, const char* element);
void compute_first_set(non_terminal* nt);
void compute_first_sets();
void compute_follow_sets();
char** compute_first_of_sequence(symbol** beta, int beta_count, int* result_size);
int contains_EPS(char** set, int size);
void add_to_set(char*** set, int* size, const char* element);
void print_first_sets();
void print_follow_sets();
void print_first_and_follow_sets();
void first_and_follow_cleanup(void);
void print_parse_tree();
void generate_parse_table();

#endif