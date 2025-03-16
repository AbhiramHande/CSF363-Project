#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"

typedef struct SymbolTableEntry entry;

// Structure for an entry of the symbol table
struct SymbolTableEntry {
    char* name;
    token_type type;
};

// Functions for insertion and fetching from symbol table
entry* symbol_table_insert(const char* lexeme, token_type value);
entry* symbol_table_fetch(const char* key);
int sym_get_collision_count();

#endif