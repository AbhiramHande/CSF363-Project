#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexerDef.h"
#include "parserDef.h"

/*****************************************************************************
 *                                                                           *
 *                           HASH MAP FUNCTIONS                              *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Inserts a production rule into the internal hash table for the LL(1) parser.
 * 
 * @param key A unique index generated using a combination of a non-terminal 
 *            and the terminal token that selects the rule.
 * @param value A pointer to the production rule to be stored.
 * 
 * @note The key must be uniquely derived from both the non-terminal producing 
 *       the rule and the token determining its selection.
 * 
 * @warning The caller is responsible for freeing the memory allocated for 
 *          the production rule. The hash table will **NOT** free these values on exit.
 */
void map_insert(const unsigned int key, production* value);

/**
 * @brief Retrieves the production rule associated with a given key from the internal hash table.
 * 
 * @param key A unique index used to identify the corresponding production rule.
 * 
 * @retval NULL if no production rule is found for the given key.
 * @retval A pointer to the corresponding production rule if found.
 * 
 * @note The key must be uniquely derived from both the non-terminal producing 
 *       the rule and the token determining its selection.
 */
production* map_fetch(const unsigned int key);

/*****************************************************************************
 *                                                                           *
 *                    DEBUGGING AND OPTIMIZATION FUNCTIONS                   *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Returns the total number of hash collisions encountered so far.
 * 
 * @return The total number of collisions that have occurred in the hash table.
 */
int get_collision_count();

#endif