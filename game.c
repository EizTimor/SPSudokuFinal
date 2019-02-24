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
#define INV_COMMAND "Error: invalid command\n"
#define EXIT_MSG "Exiting...\n"
#define SUCCESS_MSG "Puzzle solved successfully\n"
#define VALIDATION_PASSED "Validation passed: board is solvable\n"
#define VALIDATION_FAILED "Validation failed: board is unsolvable\n"
#define MALLOC_ERROR "Error: malloc has failed\n"
#define FGETS_ERROR "Error: fgets has failed\n"
/*
 * a unique ID for each command, the parser returns the ID corresponding to the parsed command.
 */
enum commandID {
	SET, HINT, VALIDATE, RESTART, EXIT
};

int insert_option(Cell* cell, int value, int board_size) {
	int index = 0;

	for (; index < board_size; index++)
		if (cell->options[index] == DEFAULT) {
			cell->options[index] = value;
			break;
		}
	cell->countOptions++;

	return 1;
}

int remove_option(Cell* cell, int index, int board_size) {
	for (; index < board_size - 1; index++)
		cell->options[index] = cell->options[index + 1];
	cell->options[board_size - 1] = 0;
	cell->countOptions--;

	return 0;
}

void printSeparatorRow(int rowLength) {
	int i;
	char *line = (char*) malloc(rowLength * sizeof(char));
	if (line == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	for (i = 0; i < rowLength - 1; i++) {
		line[i] = '-';
	}
	line[rowLength - 1] = '\0';
	printf("%s\n", line);
	free(line);
}

void printCell(Cell *cell) {
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

void printRow(Board *board, int index) {
	int j = 0, i;
	printf("|");
	while (j < board->board_size) {
		for (i = 0; i < board->block_col; i++) {
			printCell(&board->current[index][j]);
			j++;
		}
		printf(" |");
	}
	printf("\n");
}

void printBoard(Board* board) {
	int index = 0, j;
	int rowLength = board->block_row * (3 * board->block_col + 2) + 2;
	printSeparatorRow(rowLength);
	while (index < board->board_size) {
		for (j = 0; j < board->block_row; j++) {
			printRow(board, index);
			index++;
		}
		printSeparatorRow(rowLength);
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
int executeCommand(Command* cmd, Board* board) {
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
		printBoard(board);
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

void create_cell(Cell* cell, int board_size) {
	int i;

	cell->countOptions = 0;
	cell->isFixed = 0;
	cell->value = DEFAULT;
	if ((cell->options = (int *) malloc(sizeof(int) * board_size)) == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	for (i = 0; i < board_size; i++)
		cell->options[i] = 0;
}

void destroy_cell(Cell* cell) {
	if (!cell)
		return;
	if (cell->options) {
		free(cell->options);
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
	printBoard(board);
	while (!is_done) {
		while (current == NULL) {
			if (fgets(in, MAX_COMMAND, stdin) == NULL) {
				if (ferror(stdin)) {
					printf(FGETS_ERROR);
					exit(0);
				}
				exit_game(board);
				destroyCommand(current);
				return 0;
			}
			if (in[0] != '\n') {
				current = parseCommand(in);
				if (!current)
					printf("%s", INV_COMMAND);
			}
		}
		to_check = executeCommand(current, board);
		if (to_check == EXIT) {
			destroyCommand(current);
			return 0;
		}
		if (to_check == RESTART) {
			destroyCommand(current);
			return 1;
		}
		if (to_check)
			is_done = is_finished(board, 1);
		destroyCommand(current);
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
		current = parseCommand(in);
		if (!current || !(current->id == RESTART || current->id == EXIT)) {
			printf("%s", INV_COMMAND);
		} else {
			switch (executeCommand(current, board)) {
			case RESTART:
				destroyCommand(current);
				return 1;

			case EXIT:
				destroyCommand(current);
				return 0;
			}
		}
		destroyCommand(current);
		current = NULL;
	}
	return 1;
}
