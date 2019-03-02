/*
 * stack.c
 *
 *  Created on: Feb 24, 2019
 *      Author: Timor Eizenman & Ido Lerer
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

Stack* init_stack() {
    Stack* stack = malloc(sizeof(Stack));
    if (stack == NULL) {
    	/* Error print needed */
		exit(0);
	}
    stack->top = NULL;
    stack->length=0;

    return stack;
}

void push(Stack* stack, int row, int col, int val) {
    StackNode* node = malloc(sizeof (StackNode));
    if (node == NULL) {
    	/* Error print needed */
        exit(0);
    }

    node->column = row;
    node->row = col;
    node->value = val;
    node->prev = stack->top;

    stack->top = node;
    stack->length = stack->length + 1;
}

void pop(Stack* stack, StackNode* popped) {
    StackNode* tmp = stack->top;
    popped->row = tmp->row;
    popped->column = tmp->column;
    popped->value = tmp->value;

    stack->top = stack->top->prev;
    stack->length = stack->length - 1;

    free(tmp);
}

StackNode* top(Stack* stack) {
    return stack->top;
}

int is_empty(Stack* stack) {
    return (stack->length == 0);
}

void destroy_stack(Stack* stack) {
	StackNode* tmp = NULL;
	while (!is_empty(stack)) {
		pop(stack, tmp);
	}
	free(stack);
}
