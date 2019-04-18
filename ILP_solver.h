/*
 * ILP_solver.h
 *
 *  Created on: Feb 25, 2019
 *      Author: Timor
 */

#ifndef ILP_SOLVER_H_
#define ILP_SOLVER_H_
#include "game.h"
#include "gurobi_c.h"
#include <time.h>

/*
 * Function: ilp
 * ----------------------
 * 	Receives a board and uses ILP to fill it.
 *
 * 	game : the board to be filled using ILP.
 *
 * 	returns: 1 if the ILP was able to fill the board, 0 otherwise.
 */
int ilp(Board* game);

/*
 * Function: lp
 * ----------------------
 * 	Receives a board and uses LP to fill it.
 *
 * 	game : the board to be filled using LP.
 *
 * 	returns: 1 if the LP was able to fill the board, 0 otherwise.
 */
int lp(Board* game, float th, int type, int row, int col);

#endif /* ILP_SOLVER_H_ */
