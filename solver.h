/*
 * solver.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor
 */

#ifndef SOLVER_STRUCT_H_
#define SOLVER_STRUCT_H_
#include "game.h"

/*
 * Function: is_finished
 * ----------------------
 * 	Receives a Board, and an integer representing which board to check, and checks whether it is complete.
 *
 * 	game : the Board to check if done
 * 	type : determines which board to check.
 *
 * 	returns: 1 if all the cells in the board are filled with numbers that are not 0, 0 otherwise
 */
int is_finished(Board* game, int type);

/*
 * Function: is_value_valid
 * ----------------------
 * 	Receives a Board, and four integers representing row number, column number, value, and which board to check.
 * 	It checks if the value is legal in the board at coordinates (row, col).
 *
 * 	game : the Board which holds the current board.
 * 	row : an integer representing the row coordinate of a cell.
 * 	col : an integer representing the column coordinate of a cell.
 * 	value : an integer representing the value we are trying to verify.
 * 	type : determines which board to check.
 *
 * 	returns: 1 if the value is legal, 0 otherwise.
 */
int is_value_valid(Board* game, int row, int col, int value, int type);

/*
 * Function: deterministic_backtrack
 * ----------------------
 * 	Receives a Board, and uses backtracking algorithm to try and solve the sudoku.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: 1 if the sudoku is solvable, 0 otherwise.
 */
int deterministic_backtrack(Board* game);

/*
 * Function: find_options
 * ----------------------
 * 	Receives a Board, and two integers representing row number and column number.
 * 	It calculates all the valid options for a specific cell in the board, and inserts them to the cell options array.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: 1 if the sudoku is solvable, 0 otherwise.
 */
void find_options(Board* game, int row, int col);

/*
 * Function: randomized_backtrack
 * ----------------------
 * 	Receives a Board, and uses randomized backtracking algorithm to try and solve the sudoku.
 *
 * 	game : the Board which holds the current board.
 * 	row : an integer representing the row coordinate of a cell.
 * 	col : an integer representing the column coordinate of a cell.
 *
 */
int randomized_backtrack(Board* game);

#endif /* SOLVER_STRUCT_H_ */
