#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parserDef.h"

#define STACK_SIZE 1024

typedef struct Stack {
    node** data;
    int top;
    int capacity;
} stack;

stack* stack_create(void);
bool stack_push(stack* s, node* nt);
node* stack_pop(stack* s);
node* stack_top(stack* s);
bool stack_empty(stack* s);
void stack_cleanup(stack** s);

#endif