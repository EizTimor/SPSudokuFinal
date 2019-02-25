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
 * 	Receives a Board and checks whether it is complete.
 *
 * 	game : the Board to check if done
 *
 * 	returns: 1 if all the cells in the board are filled with numbers that are not 0, 0 otherwise
 */
int is_finished(Board* game);

/*
 * Function: is_value_valid
 * ----------------------
 * 	Receives a Board, and four integers representing row number, column number, value.
 * 	It checks if the value is legal in the board at coordinates (row, col).
 *
 * 	game : the Board which holds the current board.
 * 	row : an integer representing the row coordinate of a cell.
 * 	col : an integer representing the column coordinate of a cell.
 * 	value : an integer representing the value we are trying to verify.
 *
 * 	returns: 1 if the value is legal, 0 otherwise.
 */
int is_value_valid(Board* game, int row, int col, int value);

/*
 * Function: validate_board
 * ----------------------
 * 	Receives a Board, and uses ILP solver to determine whether it is solvable.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: 1 if the board is solvable, 0 otherwise.
 */
int validate_board(Board* game);

/*
 * Function: number_of_solutions
 * ----------------------
 * 	Receives a Board, and uses backtracking algorithm to count number of possible solutions.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: an integer indicating the amount of different solutions.
 */
int number_of_solutions(Board* game);

/*
 * Function: check_errors_in_board
 * ----------------------
 * 	Receives a Board, and marks all errors on the it.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: ?
 */
void check_errors_in_board(Board* game);

/*
 * Function: deterministic_backtrack
 * ----------------------
 * 	Receives a Board, a cell and two integers indicating the cell's row and column, and check only the related cells for errors.
 *
 * 	game : the Board which holds the current board.
 * 	cell : the cell that was changed.
 * 	row : an integer indicating in which row the cell is located.
 * 	col : an integer indicating in which column the cell is located.
 *
 * 	returns: ?
 */
void check_specific_error(Board* game, Cell* cell, int row, int col);

/*
 * Function: deterministic_backtrack
 * ----------------------
 * 	Receives a Board, and checks if there any errors in it.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: 1 if there are errors, 0 otherwise.
 */
int is_there_errors(Board* game);

#endif /* SOLVER_STRUCT_H_ */
