/*
 * game.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */


#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT 0
#define SIMPLE 3
#define MALLOC_ERROR "Error: malloc has failed\n"
#define FOPEN_ERROR "Error: could not open file\n"
#define FGETS_ERROR "Error: fgets has failed\n"

game_mode current_game_mode = GAME_MODE_INIT;
int mark_errors = 1;

int insert_option(Cell* cell, int value) {
	int index = 0;
	OptionNode* last = cell->options->top->prev;
	OptionNode* tmp = cell->options->top;

	while (index < cell->options->length) {
		if (tmp->value == value)
			return 1;
		tmp = tmp->next;
		index++;
	}
	tmp = NULL;

	if ((tmp = (OptionNode *) malloc(sizeof(OptionNode))) == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	tmp->value = value;
	tmp->next = cell->options->top;
	tmp->prev = last;
	last->next = tmp;
	cell->options->top->prev = tmp;
	cell->options->length = cell->options->length + 1;

	return 1;
}

int remove_option(Cell* cell, int value) {
	OptionNode* curr = cell->options->top;
	int count = 1;

	while (curr->value != value) {
		curr = curr->next;
		if (count == cell->options->length) {
			free(curr);
			return 0;
		}
		count++;
	}
	if (curr == cell->options->top)
		cell->options->top = curr->next;
	curr->prev->next = curr->next;
	curr->next->prev = curr->prev;
	free(curr);

	cell->options->length = cell->options->length - 1;

	return 0;
}

void print_separator_row(int row_length) {
	int i;
	char *line = (char*) malloc(row_length * sizeof(char));
	if (line == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	for (i = 0; i < row_length - 1; i++) {
		line[i] = '-';
	}
	line[row_length - 1] = '\0';
	printf("%s\n", line);
	free(line);
}

void print_cell(Cell *cell) {
	char fix_sign = cell->isFixed ? '.' : ' ';
	char error_sign =
			(cell->isError
					&& (current_game_mode == GAME_MODE_EDIT || mark_errors)) ?
					'*' : ' ';
	int val = cell->value;
	if (val)
		printf(" %2d%c%c", val, fix_sign, error_sign);
	else
		printf("   %c%c", fix_sign, error_sign);
}

void print_row(Board *board, int index) {
	int j = 0, i;
	printf("|");
	while (j < board->board_size) {
		for (i = 0; i < board->block_col; i++) {
			print_cell(&board->current[index][j]);
			j++;
		}
		printf(" |");
	}
	printf("\n");
}

void print_board(Board* board) {
	int index = 0, j;
	int row_length = 4 * board->board_size + board->block_row + 1;
	print_separator_row(row_length);
	while (index < board->board_size) {
		for (j = 0; j < board->block_row; j++) {
			print_row(board, index);
			index++;
		}
		print_separator_row(row_length);
	}
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

void update_cell_options(Board* game, int row, int col) {
	int i = 0;

	while (game->current[row][col].options->length > 0) {
		remove_option(&game->current[row][col],
				game->current[row][col].options->top->value);

		if (game->current[row][col].value != DEFAULT)
			return;

		for (; i < game->board_size; i++) {
			if (is_value_valid(game, row, col, i))
				insert_option(&game->current[row][col], i);
		}
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

void set_value(Board* game, int row, int col, int value) {
	int prev_val = game->current[row][col].value;
	if (value != prev_val) {
		game->current[row][col].value = value;
		check_specific_error(game, row, col);
		update_options_after_set(game, row, col);
	}
}

Board* create_board(int rows, int cols) {
	int i, j;
	Board* board = (Board*) malloc(sizeof(Board));
	Cell **current;
	if (board == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	board->block_row = rows;
	board->block_col = cols;
	board->board_size = rows * cols;

	if ((current = (Cell **) malloc(sizeof(Cell *) * board->board_size)) == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	board->current = current;

	for (i = 0; i < board->board_size; i++) {
		if ((current[i] = (Cell *) malloc(sizeof(Cell) * board->board_size))
				== NULL) {
			printf(MALLOC_ERROR);
			exit(0);
		}
		for (j = 0; j < board->board_size; j++)
			create_cell(&current[i][j]);
	}
	return board;
}

Board* create_board_copy(Board* game) {
	Board* newGame = create_board(game->block_row, game->block_col);
	int row, col;

	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++) {
			newGame->current[row][col].isError =
					game->current[row][col].isError;
			newGame->current[row][col].isFixed =
					game->current[row][col].isFixed;
			newGame->current[row][col].options = memcpy(
					newGame->current[row][col].options,
					game->current[row][col].options,
					game->current[row][col].options->length
							* sizeof(OptionNode));
			newGame->current[row][col].value = game->current[row][col].value;
		}
	return newGame;
}

void create_cell(Cell* cell) {
	cell->isFixed = 0;
	cell->value = DEFAULT;
	if ((cell->options = (OptionsList *) malloc(sizeof(OptionsList))) == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
}

void destroy_cell(Cell* cell) {
	if (!cell)
		return;
	while (cell->options->length > 0) {
		remove_option(cell, cell->options->top->value);
	}
	free(cell->options);
	free(cell);
}

void destroy_board(Board* board) {
	int i, j;
	if (!board)
		return;
	for (i = board->board_size - 1; i >= 0; i--) {
		for (j = board->board_size - 1; j >= 0; j--) {
			destroy_cell(&board->current[i][j]);
		}
		if (board->current[i])
			free(board->current[i]);
	}
	free(board->current);
	free(board);
}
