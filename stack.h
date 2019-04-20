/*
 * stack.h
 *
 *  Created on: Feb 24, 2019
 *      Author: Timor Eizenman & Ido Lerer
 *
 */

#ifndef STACK_H_
#define STACK_H_

/*
 * Structure: node
 * ------------------
 * 	A structure used to represent a node in the stack
 *
 * 	column : an integer representing the changed cell's column.
 * 	row : an integer representing the changed cell's row.
 * 	value : an integer representing the changed cell's value.
 * 	prev : a pointer to the previous node in the stack.
 */
typedef struct node {
    int column;
    int row;
    int value;
    struct node* prev;
} StackNode;

/*
 * Structure: stack
 * ------------------
 * 	A structure used to represent a stack
 *
 * 	top : a pointer to the top element of the stack.
 * 	length : an integer representing the stack's length.
 */
typedef struct stack {
	StackNode* top;
	int length;
} Stack;


/*
 * Function: init_Stack
 * ----------------------
 * 	Initializes a new empty stack
 *
 * 	returns: Pointer to the new stack
 */
Stack* init_stack();

/*
 * Function: push
 * ----------------------
 * 	Receives a stack, row, col and value and pushes it to the stack
 *
 *	stack : a pointer to the stack
 * 	row : an integer representing the changed cell's row.
 * 	col : an integer representing the changed cell's column.
 * 	val : an integer representing the changed cell's value.
 *
 * 	returns: ?
 */
void push(Stack* stack, int row, int col, int val);

/*
 * Function: pop
 * ----------------------
 * 	Receives a stack and a pointer to node, popping the top from the stack and changing the received
 * 	pointer to the popped node
 *
 *	stack : a pointer to the stack
 * 	popped : a pointer to be changed in the function
 *
 * 	returns: ?
 */
void pop(Stack* stack, StackNode* popped);

/*
 * Function: top
 * ----------------------
 * 	Receives a stack and returns a pointer to it's head
 *
 *	stack : a pointer to the stack
 *
 * 	returns: a pointer to the top element of the stack
 */
StackNode* top(Stack* stack);

/*
 * Function: is_empty
 * ----------------------
 * 	Receives a stack and returns whether the stack is empty
 *
 *	stack : a pointer to the stack
 *
 * 	returns: an integer indicating whether the stack is empty
 */
int is_empty(Stack* stack);

/*
 * Function: is_empty
 * ----------------------
 * 	Receives a stack and frees all related memory
 *
 *	stack : a pointer to the stack
 *
 * 	returns: ?
 */
void destroy_stack(Stack* stack);

#endif /* STACK_H_ */
