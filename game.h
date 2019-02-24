/*
 * game.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */

#ifndef GAME_H_
#define GAME_H_

/*
 * Structure: Cell
 * ------------------
 * 	A structure used to represent a cell on the board
 *
 * 	isFixed : an integer representing whether this cell is fixed in the user's board.
 * 	value : an integer representing the cell's value
 * 	options[] : an integers array storing the value options of a cell.
 * 	countOptions : an integer representing the length of options[]
 */
typedef struct one_cell{
	int isFixed;
	int value;
	int *options;
	int countOptions;
} Cell;

/*
 * Structure: Board
 * ------------------
 * 	A structure used to represent a sudoku board.
 *
 * 	block_row : an integer representing how many rows every block have.
 * 	block_col : an integer representing how many columns every block have.
 * 	board_size : an integer representing home many rows and columns the board have.
 * 	current[] : an Cells array representing the current board's state.
 * 	complete[] : an Cells array representing the board's solution.
 */
typedef struct sudoku_board{
	int block_row;
	int block_col;
	int board_size;
	Cell **current;
	Cell **complete;
} Board;

/*
 * Function: insert_option
 * ----------------------
 * 	Receives a Cell and two integers representing a value, and the board size.
 * 	It inserts that value to the Cell's options array.
 *
 * 	cell : a Cell which his options array should be changed.
 * 	value : an integer representing the value to be added to the Cell's options array.
 * 	board_size : an integer representing the board size.
 *
 * 	returns: ?
 */
int insert_option(Cell* cell, int value, int board_size);

/*
 * Function: remove_option
 * ----------------------
 * 	Receives a Cell and two integers representing an index of value, and the board size.
 * 	It removes that value from the Cell's options array.
 *
 * 	cell : a Cell which his options array should be changed.
 * 	index : an integer representing the index of the value to be removed from the Cell's options array.
 * 	board_size : an integer representing the board size.
 *
 * 	returns: ?
 */
int remove_option(Cell* cell, int index, int board_size);

/*
 * Function: printBoard
 * ------------------------
 * 	prints the current board state to stdout.
 *
 * 	board : the board to be printed.
 *
 */
void printBoard(Board* board);

/*
 * Function: fix_cells
 * ----------------------
 * 	Receives a board, and an integer representing the amount of cells to be fixed, randomize which cells to fix and fixed them.
 *
 *	board : the board of the game.
 * 	amount : an integer representing the amount of cells to be fixed.
 */
void fix_cells(Board* board, int amount);

/*
 * Function: clear_solution
 * ----------------------
 * 	Receives a board, and changes the complete version to be exactly like current for validation.
 *
 *	board : the board of the game.
 */
void clear_solution(Board* board);

/*
 * Function: create_board
 * ----------------------
 * 	Receives Receives three integers representing number of rows and columns in block, and amount of fixed cells,
 * 	then it creates a board and returns it.
 *
 *	rows : an integer representing the number of rows in a block.
 * 	cols : an integer representing the number of columns in a block.
 * 	fixed : an integer representing the number of fixed cells to be fixed.
 *
 * 	return : a Board object, ready for sudoku game.
 */
Board* create_board(int rows, int cols, int fixed);

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
 * Function: start_game
 * ----------------------
 * 	Receives a board, and manages the game until it is finished.
 *
 *	board : the board of the game.
 *
 *	return : -1 if EOF comes up
 */
int start_game(Board* board);

/*
 * Function: exit_game
 * ----------------------
 * 	Receives a board, and frees it's resources.
 *
 *	board : the board of the game.
 */
void exit_game(Board* board);

#endif /* GAME_H_ */
