/*
 * solver.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "stack.h"

#define DEFAULT 0
#define MAX_ITERS 1000

int is_finished(Board* game) {
	int i = 0, j = 0;
	for (; i < game->board_size; i++)
		for (j = 0; j < game->board_size; j++)
			if (game->current[i][j].value == DEFAULT)
				return 0;
	return 1;
}

int is_value_valid(Board* game, int row, int col, int value) {
	int i = 0, j = 0;
	int rows_to_add = 0, cols_to_add = 0;

	for (; i < game->board_size; i++) {
		if (game->current[row][i].value == value)
			return 0;
		if (game->current[i][col].value == value)
			return 0;
	}

	rows_to_add = (row / game->block_row) * game->block_row;
	cols_to_add = (col / game->block_col) * game->block_col;

	for (i = rows_to_add; i < rows_to_add + game->block_row; i++)
		for (j = cols_to_add; j < cols_to_add + game->block_col; j++)
			if (game->current[i][j].value == value)
				return 0;

	return 1;
}

int validate_board(Board* game) {
	Board* copy = create_board_copy(game);
	int valid = ilp(copy);
	destroyBoard(copy);
	return valid;
}

/*
 * Function: get_first_empty_cell
 * ----------------------
 * 	Receives a Board, and two pointers. Finds the first empty cell and change the pointers value accordingly.
 *
 * 	game : the Board which holds the current board.
 * 	row : a pointer representing the row coordinate of a cell.
 * 	col : a pointer representing the column coordinate of a cell.
 *
 * 	returns: 1 if there's an empty cell, 0 otherwise.
 */
int get_first_empty_cell(Board* game, int* row, int* col) {
	int i = 0, j = 0;
	for (; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			if (game->current[i][j].value == DEFAULT && game->current[i][j].isFixed == 0) {
				*row = i;
				*col = j;
				return 1;
			}
		}
	}
	return 0;
}

int number_of_solutions(Board* game) {
	int count = 0;
	int row = 0, col = 0;
	Stack* stack = init_stack();
	StackNode* node = (StackNode*)malloc(sizeof(StackNode));
	if (node == NULL) {
		/* print error */
		exit(0);
	}

	if (!validate_board(game))
		return 0;

	if (!get_first_empty_cell(game, &row, &col))
		return 1;

	push(stack, row, col, 1);
	while(!is_empty(stack)) {
		if (is_value_valid(game, stack->top->row, stack->top->column, stack->top->value)) {
			game->current[stack->top->row][stack->top->column].value = stack->top->value;
			if (!get_first_empty_cell(game, &row, &col))
				push(stack, row, col, 1);
			else { /* board is complete */
				count += 1;
				game->current[stack->top->row][stack->top->column].value = DEFAULT;
				if (stack->top->value < game->board_size) /* we can try another value for this cell */
					stack->top->value = stack->top->value + 1;
				else {
					while (!is_empty(stack) && stack->top->value == game->board_size) { /* pop until we find a cell to modify */
						pop(stack, node);
						game->current[node->row][node->column].value = DEFAULT;
					}
					if (!is_empty(stack))
						stack->top->value = stack->top->value + 1;
				}
			}
		}
		else {
			if (stack->top->value < game->board_size) /* we can try another value for this cell */
				stack->top->value = stack->top->value + 1;
			else {
				while (!is_empty(stack) && stack->top->value == game->board_size) { /* pop until we find a cell to modify */
					pop(stack, node);
					game->current[node->row][node->column].value = DEFAULT;
				}
				if (!is_empty(stack))
					stack->top->value = stack->top->value + 1;
			}
		}
	}

	destroy_stack(stack);
	free(node);

	return count;
}

void check_errors_in_board(Board* game) {
	int row, col, value;

	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++) {
			value = game->current[row][col].value;
			game->current[row][col].value = 0;
			if (is_value_valid(game, row, col, value))
				game->current[row][col].isError = 0;
			else
				game->current[row][col].isError = 1;
			game->current[row][col].value = value;
		}
}

void check_specific_error(Board* game, Cell* cell, int row, int col) {
	int i, j, value;
	int rows_to_add, cols_to_add;

	/* check the row */
	for (i = 0; i < game->board_size; i++) {
		value = game->current[row][i].value;
		game->current[row][i].value = 0;
		if (is_value_valid(game, row, i, value))
			game->current[row][i].isError = 0;
		else
			game->current[row][i].isError = 1;
		game->current[row][i].value = value;
	}

	/* check the column */
	for (i = 0; i < game->board_size; i++) {
		value = game->current[i][col].value;
		game->current[i][col].value = 0;
		if (is_value_valid(game, i, col, value))
			game->current[i][col].isError = 0;
		else
			game->current[i][col].isError = 1;
		game->current[i][col].value = value;
	}

	/* check the block */
	rows_to_add = (row / game->block_row) * game->block_row;
	cols_to_add = (col / game->block_col) * game->block_col;

	for (i = rows_to_add; i < rows_to_add + game->block_row; i++)
		for (j = cols_to_add; j < cols_to_add + game->block_col; j++) {
			value = game->current[i][j].value;
			game->current[i][j].value = 0;
			if (is_value_valid(game, i, col, value))
				game->current[i][j].isError = 0;
			else
				game->current[i][j].isError = 1;
			game->current[i][j].value = value;
		}
}

int is_there_errors(Board* game) {
	int row, col;

	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++)
			if (game->current[row][col].isError)
				return 1;
	return 0;
}
