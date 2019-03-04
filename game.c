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
#define EXIT_MSG "Exiting...\n"
#define SUCCESS_MSG "Puzzle solved successfully\n"
#define VALIDATION_PASSED "Validation passed: board is solvable\n"
#define VALIDATION_FAILED "Validation failed: board is unsolvable\n"
#define MALLOC_ERROR "Error: malloc has failed\n"
#define FOPEN_ERROR "Error: could not open file\n"
#define FGETS_ERROR "Error: fgets has failed\n"

game_mode current_game_mode = GAME_MODE_INIT;
Board* board = NULL;
int mark_errors = 1;
TurnsList* turns_list = NULL;

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

/*
 * the function receives a command object as parsed by the parser and executes it.
 */
int execute_command(Command* cmd) {
	int x = cmd->params[0], y = cmd->params[1], z = cmd->params[2];
	float float_param = cmd->float_param;
	char* path = cmd->string_param;
	switch (cmd->id) {
	case INVALID_COMMAND:
		printf(cmd->error_message);
		return 1;

	case SOLVE:
		board = load_board(path);
		if (!board) {
			printf(FOPEN_ERROR);
			return 1;
		}
		turns_list = create_turns_list();
		current_game_mode = GAME_MODE_SOLVE;
		print_board(board);
		break;

	case EDIT:
		if (path) {
			board = load_board(path);
			if (!board) {
				printf(FOPEN_ERROR);
				break;
			}
		} else {
			board = create_board(SIMPLE, SIMPLE);
			turns_list = create_turns_list();
		}
		current_game_mode = GAME_MODE_EDIT;
		print_board(board);
		return 1;

	case MARK_ERORRS:
		mark_errors = cmd->params[0];
		return 1;

	case PRINT_BOARD:
		print_board(board);
		return 1;

	case SET:
		if (x < 1 || x > board->board_size) {
			printf("Error: first parameter out of range\n");
			break;
		}
		if (y < 1 || y > board->board_size) {
			printf("Error: second parameter out of range\n");
			break;
		}
		if (z < 0 || z > board->board_size) {
			printf("Error: third parameter out of range\n");
			break;
		}
		set_value(board, x, y, z);
		print_board(board);
		return 1;

	case VALIDATE:
		if (is_there_errors(board)) {
			printf("Errors exist in board\n");
			break;
		}
		if (validate_board(board)) {
			printf("Board is solvable\n");
		} else {
			printf("Board is not solvable\n");
		}
		return 1;

	case GUESS:
		if (is_there_errors(board)) {
			printf("Errors exist in board\n");
			break;
		}
		if (!guess_solution(board, float_param)) {
			printf("Could not find a solution with given threshold parameter");
			break;
		}
		print_board(board);
		return 1;

	case GENERATE:
		if (is_there_errors(board)) {
			printf("Errors exist in board\n");
			return 1;
		}
		if (num_of_empty_cells(board) < x) {
			printf("Errors: not enough empty cells in board\n");
			return 1;
		}
		generate_board(board, turns_list, x, y);
		print_board(board);
		return 1;

	case UNDO:
		undo(board, turns_list);
		print_board(board);
		return 1;

	case REDO:
		redo(board, turns_list);
		print_board(board);
		return 1;

	case SAVE:
		if (current_game_mode == GAME_MODE_EDIT) {
			if (is_there_errors(board)) {
				printf("Errors exist in board, can not save\n");
				return 1;
			}
			if (!validate_board(board)) {
				printf("Board is not solvable, can not save\n");
				return 1;
			}
			if (!save_board(board, path, 1)){
				printf(FOPEN_ERROR);
			}
		} else {
			if(!save_board(board, path, 0)){
				printf(FOPEN_ERROR);
			}
		}
		return 1;

	case HINT:
		get_hint(board, x, y, 1);
		return 1;

	case GUESS_HINT:
		get_hint(board, x, y, 0);
		return 1;

	case NUM_SOLUTIONS:
		if (is_there_errors(board)) {
			printf("Errors exist in board\n");
			return 1;
		}
		printf("%d", number_of_solutions(board));
		return 1;

	case AUTOFILL:
		auto_fill(board, turns_list);
		print_board(board);
		return 1;

	case RESET:
		reset_board(board, turns_list);
		print_board(board);
		return 1;

	case EXIT:
		destroy_board(board);
		destroy_turns_list(turns_list);
		printf("Exiting...");
		return 0;
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
			create_cell(&current[i][j], board->board_size);
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

void create_cell(Cell* cell, int board_size) {

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
