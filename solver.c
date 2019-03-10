/*
 * solver.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "solver.h"

#define DEFAULT 0
#define SIMPLE 3
#define MAX_ITERS 1000
#define EXIT_MSG "Exiting...\n"
#define SUCCESS_MSG "Puzzle solved successfully\n"
#define VALIDATION_PASSED "Validation passed: board is solvable\n"
#define VALIDATION_FAILED "Validation failed: board is unsolvable\n"
#define MALLOC_ERROR "Error: malloc has failed\n"
#define FOPEN_ERROR "Error: could not open file\n"
#define FGETS_ERROR "Error: fgets has failed\n"

Board* board = NULL;
TurnsList* turns_list = NULL;

int is_finished(Board* game) {
	return (!num_of_empty_cells(game) && !is_there_errors(game));
}

void set_value_command(Board* game, int row, int col, int value, TurnsList* turns) {
	int prev_val = game->current[row][col].value;
	MovesList* moves;
	set_value(game, row, col, value);
	moves = create_moves_list();
	insert_move(moves, row, col, prev_val, value);
	insert_turn(turns, moves);
}

int validate_board(Board* game) {
	Board* copy = create_board_copy(game);
	int valid = ilp(copy);
	destroy_board(copy);
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
			if (game->current[i][j].value == DEFAULT
					&& game->current[i][j].isFixed == 0) {
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
	StackNode* node = (StackNode*) malloc(sizeof(StackNode));
	if (node == NULL) {
		/* print error */
		exit(0);
	}

	if (!validate_board(game))
		return 0;

	if (!get_first_empty_cell(game, &row, &col))
		return 1;

	push(stack, row, col, 1);
	while (!is_empty(stack)) {
		if (is_value_valid(game, stack->top->row, stack->top->column,
				stack->top->value)) {
			set_value(game,stack->top->row, stack->top->column, stack->top->value);
			if (!get_first_empty_cell(game, &row, &col))
				push(stack, row, col, 1);
			else { /* board is complete */
				count += 1;
				set_value(game, stack->top->row, stack->top->column, DEFAULT);
				if (stack->top->value < game->board_size) /* we can try another value for this cell */
					stack->top->value = stack->top->value + 1;
				else {
					while (!is_empty(stack)
							&& stack->top->value == game->board_size) { /* pop until we find a cell to modify */
						pop(stack, node);
						set_value(game, stack->top->row, stack->top->column, DEFAULT);
					}
					if (!is_empty(stack))
						stack->top->value = stack->top->value + 1;
				}
			}
		} else {
			if (stack->top->value < game->board_size) /* we can try another value for this cell */
				stack->top->value = stack->top->value + 1;
			else {
				while (!is_empty(stack) && stack->top->value == game->board_size) { /* pop until we find a cell to modify */
					pop(stack, node);
					set_value(game, stack->top->row, stack->top->column, DEFAULT);
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

int is_there_errors(Board* game) {
	int row, col;

	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++)
			if (game->current[row][col].isError)
				return 1;
	return 0;
}

int guess_solution(Board* game, float t) {
	printf("%f", t);
	if (is_there_errors(game)) {
		/* print error message */
		return 0;
	}

	return lp(game);
}

int num_of_empty_cells(Board* game) {
	int count = 0, row, col;

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

int generate_board(Board* game, TurnsList* turns, int x, int y) {
	int rRow, rCol;
	int i = 0, j, count, k;
	int* rows, *cols;
	MovesList* moves;
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
	rows = (int *) malloc(sizeof(int) * x);
	cols = (int *) malloc(sizeof(int) * x);
	if (!rows || !cols) {
		/* error message */
		exit(0);
	}
	for (; i < MAX_ITERS; i++) {
		for (j = 0; j < x; j++) {
			/* find and allocate values to x random cells */
			rRow = rand() % game->board_size;
			rCol = rand() % game->board_size;
			if (game->current[rRow][rCol].value != DEFAULT) {
				if (game->current[rRow][rCol].options->length != 0) {
					for (k = 0; k < count; k++) {
						set_value(game, rows[k], cols[k], DEFAULT);
						rows[k] = 0;
						cols[k] = 0;
					}
					count = 0;
					break;
				}
				rows[j] = rRow;
				cols[j] = rCol;
				set_value(game, rRow, rCol, get_random_value(&game->current[rRow][rCol]));
			} else
				j--;
		}
		if (!count)
			continue;

		if (!ilp(game)) {
			for (k = 0; k < count; k++) {
				set_value(game, rows[k], cols[k], DEFAULT);
				rows[k] = 0;
				cols[k] = 0;
			}
			count = 0;
		} else
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
		if (game->current[rRow][rCol].value == DEFAULT)
			i--;
		game->current[rRow][rCol].value = DEFAULT;
		if (copy->current[rRow][rCol].value != DEFAULT)
			insert_move(moves, rRow, rCol, copy->current[rRow][rCol].value,
			DEFAULT);
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
	Board* copy = create_board_copy(game);
	int value;

	if (type) {
		if (!ilp(copy)) {
			/* error message */
			return 0;
		}
	} else {
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
	destroy_board(copy);
	return value;
}

int auto_fill(Board* game, TurnsList* turns) {
	Board* copy;
	int row, col;
	MovesList* moves;

	if (!validate_board(game)) {
		/* error message */
		return 0;
	}

	copy = create_board_copy(game);
	moves = create_moves_list();

	for (row = 0; row < game->board_size; row++)
		for (col = 0; col < game->board_size; col++) {
			if (copy->current[row][col].options->length == 1) {
				set_value(game, row, col, copy->current[row][col].options->top->value);
				insert_move(moves, row, col, DEFAULT,
						game->current[row][col].value);
				printf("Cell <%d,%d> has been auto-filled with the value %d\n",
						row, col, copy->current[row][col].options->top->value);
			}
		}

	insert_turn(turns, moves);
	destroy_board(copy);
	return 1;
}

void undo(Board* game, TurnsList* turns) {
	MoveNode* move;
	int amount;

	if (turns->top == turns->current) {
		/* error message */
		return;
	}

	move = turns->current->prev->changes->top;
	amount = turns->current->prev->changes->length;

	while (amount > 0) {
		set_value(game, move->row, move->col, move->prev_val);
		printf("Cell <%d,%d> has been modified back to %d/n", move->row,
				move->col, move->prev_val);
		move = move->next;
		amount--;
	}

	turns->current = turns->current->prev;
}

void redo(Board* game, TurnsList* turns) {
	MoveNode* move;
	int amount;

	if (turns->top->prev == turns->current) {
		/* error message */
		return;
	}

	move = turns->current->next->changes->top;
	amount = turns->current->next->changes->length;

	while (amount > 0) {
		set_value(game, move->row, move->col,
				move->prev_val);
		printf("Cell <%d,%d> has been modified back to %d/n", move->row,
				move->col, move->prev_val);
		move = move->next;
		amount--;
	}

	turns->current = turns->current->next;
}

void reset_board(Board* game, TurnsList* turns) {
	while (turns->current != turns->top) {
		undo(game, turns);
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
		printf("%s/n", cmd->error_message);
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
	return 1;
}
