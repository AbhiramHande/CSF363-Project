#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexerDef.h"
#include "parserDef.h"

// Function declarations for all the functions of the hash map
void map_insert(const unsigned int key, production* value);
production* map_fetch(const unsigned int key);
int get_collision_count();

#endif