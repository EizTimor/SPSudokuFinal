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
#include "turns_list.h"

#define DEFAULT 0
#define MAX_ITERS 1000

int is_finished(Board* game) {
	return (!num_of_empty_cells(game) && !is_there_errors(game));
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

void set_value(Board* game, int row, int col, int value) {
	int prev_val = game->current[row][col].value;
	if (value != prev_val) {
		game->current[row][col].value = value;
		check_specific_error(game, row, col);
		update_options_after_set(game, row, col, value, prev_val);
	}
}

void update_options_after_set(Board* game, int row, int col) {
	int i, j;
	int rows_to_add, cols_to_add;

	/* modify the row and column */
	for (i = 0; i < game->board_size; i++) {
		update_cell_options(game, row, i);
		update_cell_options(game, i, col);
	}

	/* modify the block */
	rows_to_add = (row / game->block_row) * game->block_row;
	cols_to_add = (col / game->block_col) * game->block_col;

	for (i = rows_to_add; i < rows_to_add + game->block_row; i++)
		for (j = cols_to_add; j < cols_to_add + game->block_col; j++) {
			update_cell_options(game, i, j);
		}
}

void update_cell_options(Board* game, int row, int col) {
	int i = 0, j = 0;
	int rows_to_add = 0, cols_to_add = 0;

	while (game->current[row][col]->options->length > 0) {
		remove_option(game->current[row][col], game->current[row][col]->options->top->value);

	if (game->current[row][col] != DEFAULT)
		return;

	for (; i < game->board_size; i++) {
		if (is_value_valid(game, row, col, i))
			insert_option(game->current[row][col], i);
	}
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
			set_value(game->current[stack->top->row][stack->top->column].value, stack->top->row, stack->top->column, stack->top->value);
			if (!get_first_empty_cell(game, &row, &col))
				push(stack, row, col, 1);
			else { /* board is complete */
				count += 1;
				set_value(game->current[stack->top->row][stack->top->column].value, stack->top->row, stack->top->column, DEFAULT);
				if (stack->top->value < game->board_size) /* we can try another value for this cell */
					stack->top->value = stack->top->value + 1;
				else {
					while (!is_empty(stack) && stack->top->value == game->board_size) { /* pop until we find a cell to modify */
						pop(stack, node);
						set_value(game->current[stack->top->row][stack->top->column].value, stack->top->row, stack->top->column, DEFAULT);
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
					set_value(game->current[stack->top->row][stack->top->column].value, stack->top->row, stack->top->column, DEFAULT);
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
			game->current[row][col].value = DEFAULT;
			if (is_value_valid(game, row, col, value))
				game->current[row][col].isError = 0;
			else
				game->current[row][col].isError = 1;
			game->current[row][col].value = value;
		}
}

void check_specific_error(Board* game, int row, int col) {
	int i, j, value;
	int rows_to_add, cols_to_add;

	/* check the row */
	for (i = 0; i < game->board_size; i++) {
		value = game->current[row][i].value;
		game->current[row][i].value = DEFAULT;
		if (is_value_valid(game, row, i, value))
			game->current[row][i].isError = 0;
		else
			game->current[row][i].isError = 1;
		game->current[row][i].value = value;
	}

	/* check the column */
	for (i = 0; i < game->board_size; i++) {
		value = game->current[i][col].value;
		game->current[i][col].value = DEFAULT;
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
			game->current[i][j].value = DEFAULT;
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

int guess_solution(Board* game, float t) {
	if (is_there_errors(game)) {
		/* print error message */
		return 0;
	}
	lp(game);
	return 1;
}

int num_of_empty_cells(Board* game) {
	int count, row, col;

	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++)
			if (game->current[row][col].value == DEFAULT)
				count++;
	return count;
}

int get_random_value(Cell* cell) {
	int r = rand() % cell->options->length;
	OptionNode* node = cell->options->top;

	for (; r >= 0; r--)
		node = node->next;
	return node->value;
}

int generate_board(Board* game, turnsList* turns, int x, int y) {
	int rRow, rCol;
	int i = 0, j, count, k;
	int* rows, cols;
	movesList* moves;
	Board* copy;

	if (!validate_board(game)) {
		/* error message */
		return 0;
	}
	if (num_of_empty_cells(game) < x) {
		/* error message */
		return 0;
	}

	copy = create_board_copy(game);
	rows = (int *)malloc(sizeof(int) * x);
	cols = (int *)malloc(sizeof(int) * x);
	if (!rows || !cols) {
		/* error message */
		exit(0);
	}
	for (; i < MAX_ITERS; i++) {
		for (j = 0; j < x; j++) {
			/* find and allocate values to x random cells */
			rRow = rand() % game->board_size;
			rCol = rand() % game->board_size;
			if (game->current[rRow][rCol] != DEFAULT) {
				if (game->current[rRow][rCol].options->length != 0) {
					for (k = 0; k < count; k++) {
						set_value(game->current[rows[k]][cols[k]], DEFAULT);
						rows[k] = 0;
						cols[k] = 0;
					}
					count = 0;
					break;
				}
				rows[j] = rRow;
				cols[j] = rCol;
				set_value(game->current[rRow][rCol], get_random_value(game->current[rRow][rCol]));
			}
			else
				j--;
		}
		if (!count)
			continue;

		if (!ilp(game)) {
			for (k = 0; k < count; k++) {
				set_value(game->current[rows[k]][cols[k]], DEFAULT);
				rows[k] = 0;
				cols[k] = 0;
			}
			count = 0;
		}
		else
			break;
	}

	if (i == MAX_ITERS) {
		free(rows);
		free(cols);
		destroy_board(copy);
		/* error message */
		return 0;
	}

	moves = create_moves_list();
	/* clear all but y cells */
	for (i = 0; i < game->board_size * game->board_size - y; i++) {
		rRow = rand() % game->board_size;
		rCol = rand() % game->board_size;
		if (game->current[rRow][rCol] == DEFAULT)
			i--;
		game->current[rRow][rCol] = DEFAULT;
		if (copy->current[rRow][rCol] != DEFAULT)
			insert_move(moves, rRow, rCol, copy->current[rRow][rCol].value, DEFAULT);
	}

	for (k = 0; k < count; k++)
		insert_move(moves, rows[k], cols[k], DEFAULT, game->current[rows[k]][cols[k]].value);

	insert_turn(turns, moves);

	destroy_board(copy);
	free(rows);
	free(cols);
	return 1;
}

int get_hint(Board* game, int row, int col, int type) {
	Board* copy = create_board_copy(game);;
	int value;

	if (type) {
		if (!ilp(copy)) {
			/* error message */
			return 0;
		}
	}
	else {
		if (!lp(copy)) {
			/* error message */
			return 0;
		}
	}

	if (game->current[row][col].isFixed) {
		/* error message */
		return 0;
	}
	if (game->current[row][col].value != DEFAULT) {
		/* error message */
		return 0;
	}

	value = copy->current[row][col].value;
	destroyBoard(copy);
	return value;
}

int auto_fill(Board* game, turnsList* turns) {
	Board* copy;
	int row, col;
	movesList* moves;

	if (!validate_board) {
		/* error message */
		return 0;
	}

	copy = create_board_copy(game);
	moves = create_moves_list();

	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++) {
			if (copy->current[row][col].options->length == 1) {
				set_value(game->current[row][col], copy->current[row][col].options->top->value);
				insert_move(moves, row, col, DEFAULT, game->current[row][col].value);
				printf("Cell <%d,%d> has been auto-filled with the value %d\n", row, col, copy->current[row][col].options->top->value);
			}
		}

	insert_turn(turns, moves);
	destroy_board(copy);
	return 1;
}

void undo(Board* game, turnsList* turns) {
	moveNode move;
	int amount;

	if (turns->top == turns->current) {
		/* error message */
		return;
	}

	move = turns->current->prev->changes->top;
	amount = turns->current->prev->changes->length;

	while(amount > 0) {
		set_value(game->current[move->row][move->col], move->row, move->col, move->prev_val);
		printf("Cell <%d,%d> has been modified back to %d/n", move->row, move->col, move->prev_val);
		move = move->next;
		amount--;
	}

	turns->current = turns->current->prev;
}

void redo(Board* game, turnsList* turns) {
	moveNode move;
	int amount;

	if (turns->top->prev == turns->current) {
		/* error message */
		return;
	}

	move = turns->current->next->changes->top;
	amount = turns->current->next->changes->length;

	while(amount > 0) {
		set_value(game->current[move->row][move->col], move->row, move->col, move->prev_val);
		printf("Cell <%d,%d> has been modified back to %d/n", move->row, move->col, move->prev_val);
		move = move->next;
		amount--;
	}

	turns->current = turns->current->next;
}

void reset_board(Board* game, turnsList* turns) {
	while (turns->current != turns->top) {
		undo(game, turns);
	}
}
