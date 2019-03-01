/*
 * solver.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor
 */

#ifndef SOLVER_H_
#define SOLVER_H_
#include "game.h"
#include "turns_list.h"

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
 * 	Receives a Board, and three integers representing row number, column number, value.
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
 * Function: set_value
 * ----------------------
 * 	Receives a Board, and three integers representing row number, column number, value.
 * 	It assigns the value to the cell, mark errors, and modifying options lists.
 *
 * 	game : the Board which holds the current board.
 * 	row : an integer representing the row coordinate of a cell.
 * 	col : an integer representing the column coordinate of a cell.
 * 	value : an integer representing the value we are assigning.
 *
 * 	returns: ?
 */
void set_value(Board* game, int row, int col, int value);

/*
 * Function: update_options_after_set
 * ----------------------
 * 	Receives a Board, and two integers representing row number, column number.
 * 	It modifies options lists only for the affected cells.
 *
 * 	game : the Board which holds the current board.
 * 	row : an integer representing the row coordinate of a cell.
 * 	col : an integer representing the column coordinate of a cell.
 *
 * 	returns: ?
 */
void update_options_after_set(Board* game, int row, int col);

/*
 * Function: update_cell_options
 * ----------------------
 * 	Receives a Board, and two integers representing row number, column number.
 * 	It modifies options lists for it.
 *
 * 	game : the Board which holds the current board.
 * 	row : an integer representing the row coordinate of a cell.
 * 	col : an integer representing the column coordinate of a cell.
 *
 * 	returns: ?
 */
void update_cell_options(Board* game, int row, int col);

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
 * Function: check_specific_error
 * ----------------------
 * 	Receives a Board, and two integers indicating the cell's row and column, and check only the related cells for errors.
 *
 * 	game : the Board which holds the current board.
 * 	row : an integer indicating in which row the cell is located.
 * 	col : an integer indicating in which column the cell is located.
 *
 * 	returns: ?
 */
void check_specific_error(Board* game, int row, int col);

/*
 * Function: is_there_errors
 * ----------------------
 * 	Receives a Board, and checks if there any errors in it.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: 1 if there are errors, 0 otherwise.
 */
int is_there_errors(Board* game);

/*
 * Function: guess_solution
 * ----------------------
 * 	Receives a Board, and a threshold as a float, and uses LP to find a possible solution.
 *
 * 	game : the Board which holds the current board.
 * 	t : a float representing the threshold.
 *
 * 	returns: 1 if a solution was found, 0 otherwise.
 */
int guess_solution(Board* game, float t);

/*
 * Function: num_of_empty_cells
 * ----------------------
 * 	Receives a Board, and count the number of empty cells.
 *
 * 	game : the Board which holds the current board.
 *
 * 	returns: the amount of empty cells.
 */
int num_of_empty_cells(Board* game);

/*
 * Function: generate_board
 * ----------------------
 * 	Receives a Board, undo list, and two integers representing the x,y arguments of the generate command,
 * 	then it tries to generate a board to solve, while leaving only y cells revealed.
 *
 * 	game : the Board which holds the current board.
 * 	turns : the undo list of this game.
 * 	x : amount of cells to allocate random values to.
 * 	y : the amount of cells to leave revealed at the end.
 *
 * 	returns: 1 if function created a new board, 0 if it's the same as before.
 */
int generate_board(Board* game, turnsList* turns, int x, int y);

/*
 * Function: get_hint
 * ----------------------
 * 	Receives a Board, and two integers representing the coordinates requested cell and what kinf of hint requested,
 * 	and returns a hint to the user for the value he should insert to that cell.
 *
 * 	game : the Board which holds the current board.
 * 	row : the row of the requested cell.
 * 	col : the column of the requested cell.
 * 	type : 0 is for guess_hint (using LP), 1 is for hint (using ILP).
 *
 * 	returns: the hint as an integer, or 0 if no hint is available.
 */
int get_hint(Board* game, int row, int col, int type);

/*
 * Function: auto_fill
 * ----------------------
 * 	Receives a Board, and the undo list, and auto-fills obvious cells.
 *
 * 	game : the Board which holds the current board.
 * 	undo : the undo list of this game.
 *
 * 	returns: 0 if board is unsolvable, 1 otherwise.
 */
int auto_fill(Board* game, turnsList* turns);

/*
 * Function: undo
 * ----------------------
 * 	Receives a Board, and the undo list, and undo one move.
 *
 * 	game : the Board which holds the current board.
 * 	turns : the undo list of this game.
 *
 * 	returns: ?
 */
void undo(Board* game, turnsList* turns);

/*
 * Function: redo
 * ----------------------
 * 	Receives a Board, and the undo list, and undo one move.
 *
 * 	game : the Board which holds the current board.
 * 	undo : the undo list of this game.
 *
 * 	returns: ?
 */
void redo(Board* game, turnsList* turns);

/*
 * Function: reset_board
 * ----------------------
 * 	Receives a Board, and the undo list, and reset the board by undoing all turns.
 *
 * 	game : the Board which holds the current board.
 * 	undo : the undo list of this game.
 *
 * 	returns: ?
 */
void reset_board(Board* game, turnsList* turns);

#endif /* SOLVER_H_ */
