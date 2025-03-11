#ifndef PARSER_H
#define PARSER_H

#include "lexerDef.h"

typedef struct NonTerminal non_terminal;
typedef struct Production production;
typedef struct Symbol symbol;
typedef struct TreeNode node;

typedef union {
    non_terminal* nt;
    TokenName token_value;
} symbol_value;

typedef enum {
    SYM_NON_TERMINAL,
    SYM_TERMINAL,
    SYM_EPSILON
} symbol_type;

struct Symbol {
    symbol_value value;
    symbol_type type;
};

struct Production {
    symbol** symbols;
    int count;
};

struct NonTerminal {
    char* name;
    int prod_count;
    int first_size;
    int follow_size;
    production** productions;
    TokenName* first_set;
    TokenName* follow_set;
    bool has_epsilon_in_first;
};

struct TreeNode {
    symbol* stack_symbol;
    Token* token_value;
    node** children; 
    int children_count;
};

#endif