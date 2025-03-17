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

/*****************************************************************************
 *                                                                           *
 *                         SYMBOL TABLE FUNCTIONS                            *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Inserts a lexeme into the internal symbol table.
 * 
 * @param lexeme A lexeme to be inserted into the symbol table (acts as the key). 
 * @param value The token type the lexer assumes it to be. 
 *              If a conflict occurs, the existing entry is preserved, ensuring 
 *              keywords remain correctly classified (e.g., the lexer may assume 
 *              a keyword lexeme is a Field ID, but the symbol table corrects it).
 * 
 * @return A pointer to the entry made in the symbol table.
 * 
 * @warning The caller should **NOT** free the returned pointer.
 *          The symbol table will free all entries upon program exit.
 */
entry* symbol_table_insert(const char* lexeme, token_type value);

/**
 * @brief Retrieves the symbol table entry for a given lexeme.
 * 
 * @param key The lexeme string used as a key to retrieve the corresponding symbol table entry.
 * 
 * @return A pointer to the entry in the symbol table, or `NULL` if the lexeme is not found.
 * 
 * @warning The caller should **NOT** free the returned pointer.
 *          The symbol table will free all entries upon program exit.
 */
entry* symbol_table_fetch(const char* key);

/*****************************************************************************
 *                                                                           *
 *                    DEBUGGING AND OPTIMIZATION FUNCTIONS                   *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Returns the total number of hash collisions encountered so far.
 * 
 * @return The total number of collisions that have occurred in the symbol table's hash map.
 */
int sym_get_collision_count();


#endif