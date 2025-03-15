/*
Group Number : 14
Name : Avyakth Krishna Kumar   ID : 2021B3A71111P
Name : Suchit Chebolu          ID : 2021B1A72281P
Name : Abhiram H               ID : 2021B4A71134P
Name : Ankur Renduchintala     ID : 2021B5A71159P
Name : Vikram Hariharan        ID : 2022A7PS0013P
*/
#include "stack.h"

stack* stack_create(void){
    stack* s = (stack*)calloc(1, sizeof(stack));
    if(!s)
        return NULL;
    
    s->capacity = STACK_SIZE;
    s->top = -1;
    s->data = (node**)calloc(s->capacity, sizeof(node*));
    
    if(!(s->data)){
        free(s);
        return NULL;
    }
    return s;
}

bool stack_push(stack* s, node* nt){
    if(s->top == s->capacity - 1)
        return false;
    s->data[++(s->top)] = nt;
    return true;
}

node* stack_pop(stack* s){
    if(stack_empty(s))
        return NULL;
    node* nt = s->data[s->top];
    s->data[(s->top)--] = NULL;
    return nt;
}

node* stack_top(stack* s){
    if(stack_empty(s))
        return NULL;
    node* nt = s->data[s->top];
    return nt;
}

bool stack_empty(stack* s){
    if(s->top == -1)
        return true;
    return false;
}

void stack_cleanup(stack** s){
    free((*s)->data);
    free(*s);
    *s = NULL;
    
    return;
}