/*
Group Number : 14
Name : Avyakth Krishna Kumar   ID : 2021B3A71111P
Name : Suchit Chebolu          ID : 2021B1A72281P
Name : Abhiram H               ID : 2021B4A71134P
Name : Ankur Renduchintala     ID : 2021B5A71159P
Name : Vikram Hariharan        ID : 2022A7PS0013P
*/
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