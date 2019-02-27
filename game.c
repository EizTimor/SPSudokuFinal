/*
 * game.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */

#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "solver.h"
#include "parser.h"

#define DEFAULT 0
#define MAX_COMMAND 1024
#define INV_COMMAND_MSG "Error: invalid command\n"
#define EXIT_MSG "Exiting...\n"
#define SUCCESS_MSG "Puzzle solved successfully\n"
#define VALIDATION_PASSED "Validation passed: board is solvable\n"
#define VALIDATION_FAILED "Validation failed: board is unsolvable\n"
#define MALLOC_ERROR "Error: malloc has failed\n"
#define FGETS_ERROR "Error: fgets has failed\n"

game_mode current_game_mode = INIT;

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

	while (curr->value != value)
		curr = curr->next;
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
	int val = cell->value;
	if (cell->isFixed) {
		printf(" .%d", val);
	} else {
		if (val)
			printf("  %d", val);
		else
			printf("   ");
	}
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
	int row_length = board->block_row * (3 * board->block_col + 2) + 2;
	print_separator_row(row_length);
	while (index < board->board_size) {
		for (j = 0; j < board->block_row; j++) {
			print_row(board, index);
			index++;
		}
		print_separator_row(row_length);
	}
}

void fix_cells(Board* board, int amount) {
	int row, col;
	while (amount > 0) {
		col = rand() % board->board_size;
		row = rand() % board->board_size;
		if (board->complete[row][col].isFixed == 0) {
			board->complete[row][col].isFixed = 1;
			amount--;
		}
	}
}
/*
 * prints the exit message and frees all memory.
 */
void exit_game(Board* board) {
	printf("%s", EXIT_MSG);
	destroy_board(board);
}

/*
 * receives a board, resets its solution to its current state.
 */
void clear_solution(Board* board) {
	int i, j;
	for (i = 0; i < board->board_size; i++)
		for (j = 0; j < board->board_size; j++)
			board->complete[i][j].value = board->current[i][j].value;
}
/*
 * the function receives a command object as parsed by the parser and executes it.
 * return: 	0 - game continue, waiting for next command
 * 			1 - need to check if solved
 * 			RESTART - restart the game
 * 			EXIT - exit the game
 */
int execute_command(Command* cmd, Board* board) {
	int x, y, val;
	switch (cmd->id) {
	case SET:
		x = cmd->params[0] - 1;
		y = cmd->params[1] - 1;
		val = cmd->params[2];
		if (board->current[y][x].isFixed) {
			printf("Error: cell is fixed\n");
			return 0;
		}
		if (!is_value_valid(board, y, x, val, 1)) {
			printf("Error: value is invalid\n");
			return 0;
		}
		board->current[y][x].value = val;
		print_board(board);
		return 1;

	case HINT:
		x = cmd->params[0] - 1;
		y = cmd->params[1] - 1;
		printf("Hint: set cell to %d\n", board->complete[y][x].value);
		return 0;

	case VALIDATE:
		clear_solution(board);
		if (deterministic_backtrack(board)) {
			printf("%s", VALIDATION_PASSED);
		} else {
			printf("%s", VALIDATION_FAILED);
		}
		return 0;

	case RESTART:
		destroy_board(board);
		return RESTART;

	case EXIT:
		exit_game(board);
		return EXIT;
	}
	return 0;
}

Board* create_board(int rows, int cols, int fixed) {
	int i, j;
	Board* board = (Board*) malloc(sizeof(Board));
	Cell **complete;
	Cell **current;
	if (board == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	board->block_row = rows;
	board->block_col = cols;
	board->board_size = rows * cols;
	board->mark_errors = 0;
	if ((complete = (Cell **) malloc(sizeof(Cell *) * board->board_size)) == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	if ((current = (Cell **) malloc(sizeof(Cell *) * board->board_size)) == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	board->complete = complete;
	board->current = current;

	for (i = 0; i < board->board_size; i++) {
		if ((complete[i] = (Cell *) malloc(sizeof(Cell) * board->board_size)) == NULL) {
			printf(MALLOC_ERROR);
			exit(0);
		}
		if ((current[i] = (Cell *) malloc(sizeof(Cell) * board->board_size)) == NULL) {
			printf(MALLOC_ERROR);
			exit(0);
		}
		for (j = 0; j < board->board_size; j++) {
			create_cell(&complete[i][j], board->board_size);
			create_cell(&current[i][j], board->board_size);
		}
	}

	randomized_backtrack(board);
	fix_cells(board, fixed);

	for (i = 0; i < board->board_size; i++)
		for (j = 0; j < board->board_size; j++)
			if (complete[i][j].isFixed) {
				current[i][j].value = complete[i][j].value;
				current[i][j].isFixed = 1;
			}

	return board;
}

Board* create_board_copy(Board* game) {
	Board* newGame = create_board(game->block_row, game->block_col, 0);
	int row, col;

	newGame->mark_errors = game->mark_errors;
	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++) {
			newGame->current[row][col].isError = game->current[row][col].isError;
			newGame->current[row][col].isFixed = game->current[row][col].isFixed;
			newGame->current[row][col].options = memcpy(newGame->current[row][col].options, game->current[row][col].options,\
					game->current[row][col].options->length * sizeof(OptionNode));
			newGame->current[row][col].value = game->current[row][col].value;
		}
	return newGame;
}

void create_cell(Cell* cell, int board_size) {
	int i;

	cell->isFixed = 0;
	cell->value = DEFAULT;
	if ((cell->options = (OptionsList *) malloc(sizeof(OptionsList))) == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	for (i = 1; i <= board_size; i++)
		insert_option(cell, i);
}

void destroy_cell(Cell* cell) {
	if (!cell)
		return;
	while (cell->options->length > 0) {
		remove_option(cell, cell->options->top->value);
	}
}

void destroy_board(Board* board) {
	int i, j;
	if (!board)
		return;
	for (i = board->board_size - 1; i >= 0; i--) {
		for (j = board->board_size - 1; j >= 0; j--) {
			destroy_cell(&board->current[i][j]);
			destroy_cell(&board->complete[i][j]);
		}
		if (board->complete[i])
			free(board->complete[i]);
		if (board->current[i])
			free(board->current[i]);
	}
	free(board->complete);
	free(board->current);
	free(board);
}

int start_game(Board* board) {
	int is_done = 0, to_check = 0;
	char in[MAX_COMMAND];
	Command* current = NULL;
	print_board(board);
	while (!is_done) {
		while (current == NULL) {
			if (fgets(in, MAX_COMMAND, stdin) == NULL) {
				if (ferror(stdin)) {
					printf(FGETS_ERROR);
					exit(0);
				}
				exit_game(board);
				destroy_command(current);
				return 0;
			}
			if (in[0] != '\n') {
				current = parse_command(in);
				if (current->id == INVALID_COMMAND)
					printf("%s", current->error_message);
			}
		}
		to_check = execute_command(current, board);
		if (to_check == EXIT) {
			destroy_command(current);
			return 0;
		}
		if (to_check == RESTART) {
			destroy_command(current);
			return 1;
		}
		if (to_check)
			is_done = is_finished(board, 1);
		destroy_command(current);
		current = NULL;
	}
	printf("%s", SUCCESS_MSG);

	/*
	 * game is complete, waiting to either "restart" or "exit" command
	 */
	while (1) {
		if (fgets(in, MAX_COMMAND, stdin) == NULL) {
			exit_game(board);
			return 0;
		}
		current = parse_command(in);
		//TODO: replace !current with "current->id == INVALID_COMMAND" and print reason
		if (!current || !(current->id == RESTART || current->id == EXIT)) {
			printf("%s", INV_COMMAND_MSG);
		} else {
			switch (execute_command(current, board)) {
			case RESTART:
				destroy_command(current);
				return 1;

			case EXIT:
				destroy_command(current);
				return 0;
			}
		}
		destroy_command(current);
		current = NULL;
	}
	return 1;
}
