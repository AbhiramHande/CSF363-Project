#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"

void print_token(token *t);
token* get_next_token(FILE *fileptr);
token_type string_to_token(const char* string);
const char* token_to_string(token_type token);
#endif
