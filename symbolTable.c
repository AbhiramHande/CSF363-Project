#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#ifndef __lexerDef_H_INCLUDED__
#define __lexerDef_H_INCLUDED__
#include "lexerDef.h"
#endif

#ifndef __symbolTabledef_H_INCLUDED__
#define __symbolTabledef_H_INCLUDED__
#include "symbolTableDef.h"
#endif

int hashFunction(char *value, int no_entries)
{
    int hash = 0;

    while (*value != '\0')
    {
        char c = *value++;
        if(c=='_'){
            hash = hash*15;
        }
        else{
            hash = hash * 15 + c - '0';
            hash = hash%(no_entries*10);
        }
    }

    if(hash<0){
        hash = 0;
    }

    return hash % no_entries;
}

// last node in each list is always empty(check by next is NULL)
symTable *createEmptyTable(int no_lists)
{

    symTable *lookupTable = (symTable *)malloc(sizeof(symTable));

    lookupTable->no_lists = no_lists;

    lookupTable->headList = (symNode **)malloc(sizeof(symNode *) * no_lists);
    for (int i = 0; i < no_lists; i++)
    {
        lookupTable->headList[i] = (symNode *)malloc(sizeof(symNode));
        lookupTable->headList[i]->next = NULL;
    }

    return lookupTable;
}
void addSymbol(symTable *lookupTable, Token *t, int lineno)
{
    char *lexeme = lexeme = t->lexeme;
    TokenName tokenName = t->name;

    int list_no = hashFunction(lexeme, lookupTable->no_lists);

    symNode *temp = (symNode *)malloc(sizeof(symNode));

    temp->lexeme = lexeme;
    temp->tokentype = tokenName;

    temp->next = lookupTable->headList[list_no];
    lookupTable->headList[list_no] = temp;
}
// void addSymbol(symTable *lookupTable, char *tokenName, tokenType type, char *lexeme, int rowNumber)
// {

//     int list_no = hashFunction(lexeme, lookupTable->no_lists);

//     symNode *temp = (symNode *)malloc(sizeof(symNode));

//     temp->lexeme = lexeme;
//     temp->rowNumber = rowNumber;
//     temp->tokenName = tokenName;
//     temp->type = type;

//     temp->next = lookupTable->headList[list_no];
//     lookupTable->headList[list_no] = temp;
// }

void addKeywords(symTable *lookupTable)
{

    Token *t;

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "with";
    t->name = TK_WITH;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "parameters";
    t->name = TK_PARAMETERS;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "end";
    t->name = TK_END;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "while";
    t->name = TK_WHILE;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "union";
    t->name = TK_UNION;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "endunion";
    t->name = TK_ENDUNION;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "definetype";
    t->name = TK_DEFINETYPE;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "as";
    t->name = TK_AS;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "type";
    t->name = TK_TYPE;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "_main";
    t->name = TK_MAIN;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "global";
    t->name = TK_GLOBAL;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "parameter";
    t->name = TK_PARAMETER;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "list";
    t->name = TK_LIST;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "input";
    t->name = TK_INPUT;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "output";
    t->name = TK_OUTPUT;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "int";
    t->name = TK_INT;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "real";
    t->name = TK_REAL;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "endwhile";
    t->name = TK_ENDWHILE;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "if";
    t->name = TK_IF;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "then";
    t->name = TK_THEN;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "endif";
    t->name = TK_ENDIF;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "read";
    t->name = TK_READ;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "write";
    t->name = TK_WRITE;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "return";
    t->name = TK_RETURN;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "call";
    t->name = TK_CALL;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "record";
    t->name = TK_RECORD;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "endrecord";
    t->name = TK_ENDRECORD;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "else";
    t->name = TK_ELSE;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "and";
    t->name = TK_AND;
    addSymbol(lookupTable, t, -1);

    t = (Token *)malloc(sizeof(Token));
    t->isint = false;
    t->lexeme = "or";
    t->name = TK_OR;
    addSymbol(lookupTable, t, -1);
}

nodeInfo *getInfo(symTable *lookupTable, char *lexeme)
{

    int list_no = hashFunction(lexeme, lookupTable->no_lists);

    symNode *temp = lookupTable->headList[list_no];

    nodeInfo *info = (nodeInfo *)malloc(sizeof(nodeInfo));
    info->is_present = false;

    while (temp != NULL && temp->next != NULL && !(info->is_present))
    {
        if (temp->lexeme != NULL && strcmp(lexeme, temp->lexeme) == 0)
        {
            info->is_present = true;
            info->node = temp;
        }

        temp = temp->next;
    }

    return info;
}