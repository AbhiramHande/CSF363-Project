#ifndef SYMBOLTABLEDEF_H
#define SYMBOLTABLEDEF_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// typedef enum
// {
//     INT,
//     REAL,
//     ERROR,
//     OTHER
// } tokenType;

typedef struct symNode
{
    struct symNode *next;
    TokenName tokentype;
    char *lexeme;

} symNode;

typedef struct symTable
{
    int no_lists;
    symNode **headList; // List of head nodes, which point to next node and so on
} symTable;

// struct for finding and returning lexeme in hash table
typedef struct nodeInfo
{
    bool is_present;
    symNode *node;
} nodeInfo;
#endif