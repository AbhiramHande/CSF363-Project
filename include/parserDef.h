#ifndef PARSER_H
#define PARSER_H

#include "lexerDef.h"

typedef struct NonTerminal non_terminal;
typedef struct Production production;
typedef struct Symbol symbol;
typedef struct TreeNode node;

// Union data structure defined for symbol_value
typedef union {
    non_terminal* nt;
    token_type token_value;
} symbol_value;

// Enum data structure defined for symbol_type
typedef enum {
    SYM_NON_TERMINAL,
    SYM_TERMINAL,
    SYM_EPSILON
} symbol_type;

// Structure for a symbol
struct Symbol {
    symbol_value value;
    symbol_type type;
};

// Structure for a Production
struct Production {
    symbol** symbols;
    int count;
};

// Structure for a non terminal symbol
struct NonTerminal {
    char* name;
    int prod_count;
    int first_size;
    int follow_size;
    production** productions;
    token_type* first_set;
    token_type* follow_set;
    bool has_epsilon_in_first;
};

// Structure for the node of the parse tree
struct TreeNode {
    symbol* stack_symbol;
    token* token_value;
    node** children; 
    int children_count;
};

#endif