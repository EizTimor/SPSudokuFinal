/*
 * solver.c
 *
 * Author: Timor Eizenman & Ido Lerer
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "solver.h"
#include <sys/ioctl.h>

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
#define FIXED_CELL "Cell is fixed, you don't need an hint\n"
#define FILLED_CELL "Cell is filled already, you don't need an hint\n"
#define NO_HINT_AVAILABLE "Board is unsolvable so no hint is available\n"

Board* board = NULL;
TurnsList* turns_list = NULL;

int is_finished(Board* game) {
	return (!num_of_empty_cells(game) && !is_there_errors(game));
}

void set_value_command(Board* game, int row, int col, int value,
		TurnsList* turns) {
	int prev_val;
	MovesList* moves;
	prev_val = game->current[row - 1][col - 1].value;
	set_value(game, row, col, value);
	moves = create_moves_list();
	insert_move(moves, row, col, prev_val, value);
	insert_turn(turns, moves);
}

int validate_board(Board* game) {
	int valid;
	Board* copy;
	copy = create_board_copy(game);
	valid = ilp(copy);
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
			set_value(game, stack->top->row + 1, stack->top->column + 1,
					stack->top->value);
			if (get_first_empty_cell(game, &row, &col)) {
				push(stack, row, col, 1);
			} else { /* board is complete */
				count += 1;
				set_value(game, stack->top->row + 1, stack->top->column + 1,
				DEFAULT);
				if (stack->top->value < game->board_size) /* we can try another value for this cell */
					stack->top->value += 1;
				else {
					while (!is_empty(stack)
							&& stack->top->value == game->board_size) { /* pop until we find a cell to modify */
						set_value(game, stack->top->row + 1,
								stack->top->column + 1,
								DEFAULT);
						pop(stack, node);
					}
					if (!is_empty(stack))
						stack->top->value += 1;
				}
			}
		} else {
			if (stack->top->value < game->board_size) { /* we can try another value for this cell */
				stack->top->value += 1;
			} else {
				while (!is_empty(stack) && stack->top->value == game->board_size) { /* pop until we find a cell to modify */
					set_value(game, stack->top->row + 1, stack->top->column + 1,
					DEFAULT);
					pop(stack, node);
				}
				if (!is_empty(stack))
					stack->top->value += 1;
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
			if (value == DEFAULT || is_value_valid(game, row, col, value))
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

int guess_solution(Board* game, TurnsList* turns, float t) {
	int i, j;
	MovesList* moves;
	Board* copy;
	int flag;
	if (is_there_errors(game)) {
		/* print error message */
		return 0;
	}

	copy = create_board_copy(game);
	lp(copy, t, 0, 0, 0);
	flag = num_of_empty_cells(copy);
	if (flag != 0) {
		destroy_board(copy);
		return 0;
	}

	moves = create_moves_list();
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			if (game->current[i][j].value != copy->current[i][j].value) {
				insert_move(moves, i + 1, j + 1, game->current[i][j].value,
						copy->current[i][j].value);
				set_value(game, i + 1, j + 1, copy->current[i][j].value);
			}
		}
	}

	insert_turn(turns, moves);
	destroy_board(copy);
	return 1;
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
	int r;
	OptionNode* node = cell->options->top;

	r = rand() % cell->options->length;
	for (; r >= 0; r--)
		node = node->next;
	return node->value;
}

int generate_board(Board* game, TurnsList* turns, int x, int y) {
	int rRow, rCol;
	int i = 0, j, count = 0, k;
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
			if (game->current[rRow][rCol].value == DEFAULT) {
				if (game->current[rRow][rCol].options->length == 0) {
					for (k = 0; k < count; k++) {
						set_value(game, rows[k] + 1, cols[k] + 1, DEFAULT);
						rows[k] = 0;
						cols[k] = 0;
					}
					count = 0;
					break;
				}
				rows[j] = rRow;
				cols[j] = rCol;
				set_value(game, rRow + 1, rCol + 1,
						get_random_value(&game->current[rRow][rCol]));
				count += 1;
			} else
				j--;
		}
		if (x != 0 && !count)
			continue;
		k = ilp(game);
		printf("k is %d and i is %d\n", k, i);
		if (k == 0) {
			for (k = 0; k < count; k++) {
				set_value(game, rows[k] + 1, cols[k] + 1, DEFAULT);
				rows[k] = 0;
				cols[k] = 0;
			}
			count = 0;
		} else {
			break;
		}
	}

	if (i == MAX_ITERS) {
		free(rows);
		free(cols);
		destroy_board(copy);
		printf("MAX_ITERS\n");
		/* error message */
		return 0;
	}

	printf("Printing Copy...\n");
	print_board(game);
	moves = create_moves_list();
	/* clear all but y cells */
	for (i = 0; i < game->board_size * game->board_size - y; i++) {
		rRow = rand() % game->board_size;
		rCol = rand() % game->board_size;
		if (game->current[rRow][rCol].value == DEFAULT)
			i--;
		else {
			set_value(game, rRow + 1, rCol + 1, DEFAULT);
		}
	}

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			if (game->current[i][j].value != copy->current[i][j].value)
				insert_move(moves, i + 1, j + 1, copy->current[i][j].value,
						game->current[i][j].value);
		}
	}

	insert_turn(turns, moves);

	destroy_board(copy);
	free(rows);
	free(cols);
	return 1;
}

int get_hint(Board* game, int row, int col, int type) {
	Board* copy;
	int value;

	if (is_there_errors(game)) {
		printf("Errors exist in board\n");
		return -1;
	}

	if (game->current[row][col].isFixed) {
		printf("%s", FIXED_CELL);
		return -1;
	}
	if (game->current[row][col].value != DEFAULT) {
		printf("%s", FILLED_CELL);
		return -1;
	}

	copy = create_board_copy(game);
	if (type) {
		if (!ilp(copy)) {
			printf("%s", NO_HINT_AVAILABLE);
			destroy_board(copy);
			return 0;
		}
	} else {
		if (!lp(copy, 0, 1, row, col)) {
			printf("%s", NO_HINT_AVAILABLE);
			destroy_board(copy);
			return 0;
		}
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
			if (copy->current[row][col].value == DEFAULT
					&& copy->current[row][col].options->length == 1) {
				set_value(game, row + 1, col + 1,
						copy->current[row][col].options->top->value);
				insert_move(moves, row + 1, col + 1, DEFAULT,
						game->current[row][col].value);
				printf("Cell <%d,%d> has been auto-filled with the value %d\n",
						row + 1, col + 1,
						copy->current[row][col].options->top->value);
			}
		}

	insert_turn(turns, moves);
	destroy_board(copy);
	return 1;
}

void undo(Board* game, TurnsList* turns) {
	MoveNode* move;
	int amount;
	if (turns->pos == 0) {
		/* error message */
		return;
	}
	move = turns->current->changes->top;
	amount = turns->current->changes->length;
	while (amount > 0) {
		set_value(game, move->row, move->col, move->prev_val);
		printf("Cell <%d,%d> has been modified back to %d\n", move->row,
				move->col, move->prev_val);
		move = move->next;
		amount--;
	}
	turns->current = turns->current->prev;
	turns->pos -= 1;
	printf("turns pos undo is %d\n", turns->pos);
}

void redo(Board* game, TurnsList* turns) {
	MoveNode* move;
	int amount;
	if (turns->pos == turns->length) {
		/* error message */
		return;
	}

	if (turns->length > 0 && turns->pos == 0) {
		move = turns->top->changes->top;
		amount = turns->top->changes->length;
	} else {
		move = turns->current->next->changes->top;
		amount = turns->current->next->changes->length;
	}
	while (amount > 0) {
		set_value(game, move->row, move->col, move->new_val);
		printf("Cell <%d,%d> has been modified back to %d\n", move->row,
				move->col, move->new_val);
		move = move->next;
		amount--;
	}
	if (turns->pos != 0) {
		if (turns->current->next != NULL) {
			turns->current = turns->current->next;
		}
	} else
		turns->current = turns->top;
	turns->pos += 1;
}

void reset_board(Board* game, TurnsList* turns) {
	while (turns->current != turns->top) {
		undo(game, turns);
	}
	undo(game, turns);
}

void print_image() {
	FILE *fptr;
	char c;
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	if (w.ws_col >= 130) {
		fptr = fopen("image.txt", "r");
		if (!fptr){
			return;
		}
		c = fgetc(fptr);
		while (c != EOF) {
			printf("%c", c);
			c = fgetc(fptr);
		}

		fclose(fptr);
	}
}

/*
 * the function receives a command object as parsed by the parser and executes it.
 */
int execute_command(Command* cmd) {
	int x = cmd->params[0], y = cmd->params[1], z = cmd->params[2];
	float float_param = cmd->float_param;
	char* path = cmd->string_param;
	int tmp;
	switch (cmd->id) {
	case INVALID_COMMAND:
		printf("%s\n", cmd->error_message);
		return 1;

	case SOLVE:
		destroy_board(board);
		board = NULL;
		destroy_turns_list(turns_list);
		turns_list = NULL;
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
		destroy_board(board);
		board = NULL;
		destroy_turns_list(turns_list);
		turns_list = NULL;
		if (path) {
			board = load_board(path);
			if (!board) {
				printf(FOPEN_ERROR);
				break;
			}
		} else {
			board = create_board(SIMPLE, SIMPLE);
		}
		turns_list = create_turns_list();
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
		if (current_game_mode == GAME_MODE_SOLVE
				&& board->current[x - 1][y - 1].isFixed) {
			printf("Cannot change fixed cells while in solve mode\n");
			break;
		}
		set_value_command(board, x, y, z, turns_list);
		print_board(board);
		if (is_finished(board)) {
			printf("Board Solved!\n");
			print_image();
		}
		return 1;

	case VALIDATE:
		printf("Validation started...\n");
		if (is_there_errors(board)) {
			printf("Errors exist in board\n");
			break;
		}
		printf("No errors in board, checking if solvable...\n");
		if (validate_board(board)) {
			printf("Board is solvable\n");
		} else {
			printf("Board is not solvable\n");
		}
		print_board(board);
		return 1;

	case GUESS:
		if (is_there_errors(board)) {
			printf("Errors exist in board\n");
			break;
		}
		if (!guess_solution(board, turns_list, float_param)) {
			printf(
					"Could not find a solution with given threshold parameter\n");
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
			if (!save_board(board, path, 1)) {
				printf(FOPEN_ERROR);
			}
		} else {
			if (!save_board(board, path, 0)) {
				printf(FOPEN_ERROR);
			}
		}
		return 1;

	case HINT:
		if (x < 1 || x > board->board_size) {
			printf("Error: first parameter out of range\n");
			break;
		}
		if (y < 1 || y > board->board_size) {
			printf("Error: second parameter out of range\n");
			break;
		}
		tmp = get_hint(board, x - 1, y - 1, 1);
		if (tmp > 0)
			printf("You should set this cell to %d\n", tmp);
		return 1;

	case GUESS_HINT:
		if (x < 1 || x > board->board_size) {
			printf("Error: first parameter out of range\n");
			break;
		}
		if (y < 1 || y > board->board_size) {
			printf("Error: second parameter out of range\n");
			break;
		}
		get_hint(board, x - 1, y - 1, 0);
		return 1;

	case NUM_SOLUTIONS:
		if (is_there_errors(board)) {
			printf("Errors exist in board\n");
			return 1;
		}
		printf("Number of possible solutions: %d\n",
				number_of_solutions(board));
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
		printf("Exiting...\n");
		return 0;
	}
	return 1;
}

