/*
Group Number : 14
Name : Avyakth Krishna Kumar   ID : 2021B3A71111P
Name : Suchit Chebolu          ID : 2021B1A72281P
Name : Abhiram H               ID : 2021B4A71134P
Name : Ankur Renduchintala     ID : 2021B5A71159P
Name : Vikram Hariharan        ID : 2022A7PS0013P
*/
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