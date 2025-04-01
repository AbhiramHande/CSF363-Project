#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "parserDef.h"

/**
 * @brief Defines the initial capacity of the stack.
 * 
 * @warning The stack starts with this size, and if it is full, no further elements can be pushed.
 */
#define STACK_SIZE 1024

/**
 * @brief Data structure representing a stack.
 * 
 * @details 
 * - A simple stack implementation with a fixed maximum size.
 * - Used for storing pointers to syntax tree nodes.
 * - Supports standard push, pop, and top operations.
 * 
 * @note
 * - **Lookup Complexity:** @f$ \mathcal{O}(1) @f$ for accessing the top element.
 * - **Insertion Complexity:** @f$ \mathcal{O}(1) @f$ for pushing an element.
 * - **Deletion Complexity:** @f$ \mathcal{O}(1) @f$ for popping an element.
 */
typedef struct Stack {
    node** data; /**< An array of pointers to tree nodes */
    int top; /**< Index of the top element, -1 if the stack is empty */
    int capacity; /**< Maximum capacity of the stack (fixed at initialization) */
} stack;


/*****************************************************************************
 *                                                                           *
 *                             STACK FUNCTIONS                               *
 *                                                                           *
 *****************************************************************************/

/**
 * @ingroup Stack functions
 * 
 * @brief Initializes a stack.
 * 
 * @return `NULL` if memory allocation fails.
 *         A pointer to a newly created stack with a capacity of 1024 otherwise.
 */
stack* stack_create(void);

/**
 * @ingroup Stack functions
 * 
 * @brief Adds a node to the top of the stack.
 * 
 * @param s A pointer to the stack.
 * @param nt The node to be inserted.
 * 
 * @return `true` if inserted.
 *         `false` if the stack is at full capacity.
 */
bool stack_push(stack* s, node* nt);

/**
 * @ingroup Stack functions
 * 
 * @brief Removes and returns the topmost node from the stack.
 * 
 * @param s A pointer to the stack.
 * 
 * @return `NULL` if the stack is empty.
 *         A pointer to the removed node otherwise.
 */
node* stack_pop(stack* s);

/**
 * @ingroup Stack functions
 * 
 * @brief Returns the topmost node from the stack without removing it.
 * 
 * @param s A pointer to the stack.
 * 
 * @return `NULL` if the stack is empty.
 *         A pointer to the topmost node otherwise.
 */
node* stack_top(stack* s);

/**
 * @ingroup Stack functions
 * 
 * @brief Determines if the stack is empty.
 * 
 * @param s A pointer to the stack.
 * 
 * @return `true` if the stack is empty.
 *         `false` otherwise.
 */
bool stack_empty(stack* s);

/**
 * @ingroup Stack functions
 * 
 * @brief Deallocates all memory associated with the stack.
 * 
 * @param s A pointer to the pointer of the stack.
 * 
 * @return void 
 * 
 * @warning All nodes inserted by the caller must be freed by the caller.
 *          The stack itself is freed, and the pointer is set to `NULL` to prevent
 *          accidental dereference in the future.
 */
void stack_cleanup(stack** s);

#endif