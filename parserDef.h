#ifndef PARSER_H
#define PARSER_H

#include "lexerDef.h"

typedef union {
    char* name;
    TokenName token_value;
} symbol_value;

typedef enum {
    SYM_NON_TERMINAL,
    SYM_TERMINAL,
    SYM_EPSILON
} symbol_type;

typedef struct Symbol {
    symbol_value value;
    symbol_type type;
} symbol;

typedef struct Production {
    symbol** symbols;
    int count;
} production;

typedef struct NonTerminal {
    char* name;
    int prod_count;
    int first_size;
    int follow_size;
    production** productions;
    TokenName* first_set;
    TokenName* follow_set;
    bool has_epsilon_in_first;
} non_terminal;

#endif