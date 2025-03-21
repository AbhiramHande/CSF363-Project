#ifndef LEXER_H
#define LEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"
#include "symbol_table.h"

/*****************************************************************************
 *                                                                           *
 *                             LEXER FUNCTIONS                               *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Initializes an internal twin buffer and loads the active buffer with the first chunk of the file.
 * 
 * @param file_ptr A pointer to an opened file containing the source code.
 * 
 * @warning Initializes the internal twin buffer, overwriting any existing contents.
 */
void buffer_init(FILE* file_ptr);

/**
 * @brief A wrapper function that fetches the next token from the file.
 * 
 * @param file_ptr A pointer to an opened file containing the source code.
 * 
 * @return A pointer to the next token. 
 *         Returns an End-of-File token if no more source code is available.
 * 
 * @warning The caller is responsible for freeing the memory allocated for the token.
 */
token* get_next_token(FILE* file_ptr);

/**
 * @brief Retrieves the token type based on the given string.
 * 
 * @param string A string representing the token name.
 * 
 * @return The corresponding token type.
 */
token_type string_to_token(const char* string);

/**
 * @brief Retrieves a string representation of a token type (for printing purposes).
 * 
 * @param token The token type.
 * 
 * @return A constant string representing the token type.
 */
const char* token_to_string(token_type token);

#endif
