#ifndef GAME_H_
#define GAME_H_
/*
 * game.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */
#include "parser.h"
#include "turns_list.h"

/*
 * a unique ID for each command, the parser returns the ID corresponding to the parsed command.
 */
enum command_id {
	INVALID_COMMAND, SOLVE, EDIT, MARK_ERORRS, PRINT_BOARD, SET, VALIDATE,
	GUESS, GENERATE, UNDO, REDO, SAVE, HINT, GUESS_HINT, NUM_SOLUTIONS, AUTOFILL,
	RESET, EXIT
};

/*
 * an external variable representing the current game mode.
 */
typedef enum game_mode {
	GAME_MODE_INIT, GAME_MODE_EDIT, GAME_MODE_SOLVE
} game_mode;

extern game_mode current_game_mode;

/*
 * Structure: OptionNode
 * ------------------
 * 	A structure used to represent a node in the options linked list
 *
 * 	value : an integer representing an option for the cell
 * 	next : a pointer to the next element in the list
 * 	prev : a pointer to the previous element in the list
 */
typedef struct option_node {
	int value;
	struct option_node* next;
	struct option_node* prev;
} OptionNode;

/*
 * Structure: OptionsList
 * ------------------
 * 	A structure used to represent a linked list
 *
 * 	top : a pointer to the first element of the list
 * 	length : an integer representing the length of the list
 */
typedef struct {
	OptionNode* top;
	int length;
} OptionsList;

/*
 * Structure: Cell
 * ------------------
 * 	A structure used to represent a cell on the board
 *
 * 	isFixed : an integer representing whether this cell is fixed in the user's board.
 * 	isError : an integer representing whether this cell is an error in the user's board.
 * 	value : an integer representing the cell's value
 * 	options : a linked list storing the value options of a cell.
 */
typedef struct {
	int isFixed;
	int isError;
	int value;
	OptionsList* options;
} Cell;

/*
 * Structure: Board
 * ------------------
 * 	A structure used to represent a sudoku board.
 *
 * 	block_row : an integer representing how many rows every block have.
 * 	block_col : an integer representing how many columns every block have.
 * 	board_size : an integer representing how many rows and columns the board have.
 * 	mark_errors : an integer representing the mark errors option.
 * 	current[] : an Cells array representing the current board's state.
 */
typedef struct {
	int block_row;
	int block_col;
	int board_size;
	Cell **current;
} Board;

/*
 * Function: insert_option
 * ----------------------
 * 	Receives a Cell and an integer representing a value.
 * 	It inserts that value to the Cell's options list.
 *
 * 	cell : a Cell which his options array should be changed.
 * 	value : an integer representing the value to be added to the Cell's options array.
 *
 * 	returns: ?
 */
int insert_option(Cell* cell, int value);

/*
 * Function: remove_option
 * ----------------------
 * 	Receives a Cell and an integer representing a value.
 * 	It removes that value from the Cell's options list.
 *
 * 	cell : a Cell which his options array should be changed.
 * 	value : an integer representing the value to be removed from the Cell's options list.
 *
 * 	returns: ?
 */
int remove_option(Cell* cell, int value);

/*
 * Function: print_board
 * ------------------------
 * 	prints the current board state to stdout.
 *
 * 	board : the board to be printed.
 *
 */
void print_board(Board* board);

/*
 * Function: create_board
 * ----------------------
 * 	Receives Receives three integers representing number of rows and columns in block,
 * 	then it creates a board and returns it.
 *
 *	rows : an integer representing the number of rows in a block.
 * 	cols : an integer representing the number of columns in a block.
 *
 * 	return : a Board object, ready for sudoku game.
 */
Board* create_board(int rows, int cols);

/*
 * Function: create_board_copy
 * ----------------------
 * 	Receives a board and returns a deep copy of it.
 *
 *	game : the board of the game.
 *
 * 	return : a deep copy of the board received.
 */
Board* create_board_copy(Board* game);

/*
 * Function: create_cell
 * ----------------------
 * 	Receives a cell and an integer representing the board size, then creates a new cell and returns it.
 *
 *	cell : the cell to initialize
 *	board_size : an integer representing size of side of the board.
 *
 * 	return : a cell of sudoku board.
 */
void create_cell(Cell* cell, int board_size);

/*
 * Function: destroy_cell
 * ------------------------
 * 	Used to destroy a cell structure and free the memory allocated to it.
 *
 * 	cell : a pointer to the cell structure.
 *
 */
void destroy_cell(Cell* cell);

/*
 * Function: destroy_board
 * ------------------------
 * 	Used to destroy a command structure and free the memory allocated to it.
 *
 * 	board : a pointer to the Board structure.
 *
 */
void destroy_board(Board* board);


/*
 * Function: create_board_copy
 * ----------------------
 * 	Receives a command as parsed by the parser and executes it.
 * 	Can execute any command which id is included in the command_id enum,
 * 	while modifying board andturns_list structures saved in game.c
 *
 *	cmd: the Command to be executed as returned by the parser.
 *
 * 	return : 1 if command was executed and game continues, 0 else.
 */
int execute_command(Command* cmd);

#endif /* GAME_H_ */
