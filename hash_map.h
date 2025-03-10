#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parserDef.h"

typedef struct Entry {
    unsigned int key;
    production* value;
    bool is_occupied;
} entry;

typedef struct HashMap {
    entry* entries;
    int capacity;
    int size;
} hash_map;

hash_map* map_create(int total_entries);
void map_insert(hash_map* map, const unsigned int key, production* value);
production* map_fetch(hash_map* map, const unsigned int key);
void map_cleanup(hash_map* map);

#endif