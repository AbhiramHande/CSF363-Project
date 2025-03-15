/*
Group Number : 14
Name : Avyakth Krishna Kumar   ID : 2021B3A71111P
Name : Suchit Chebolu          ID : 2021B1A72281P
Name : Abhiram H               ID : 2021B4A71134P
Name : Ankur Renduchintala     ID : 2021B5A71159P
Name : Vikram Hariharan        ID : 2022A7PS0013P
*/
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
