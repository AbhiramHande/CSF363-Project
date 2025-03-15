#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"
#include "symbol_table.h"

// Function declarations for all functions of the lexer
void print_token(token *t);
void buffer_init(FILE* file_ptr);
token* get_next_token(FILE *fileptr);
token_type string_to_token(const char* string);
const char* token_to_string(token_type token);
#endif
