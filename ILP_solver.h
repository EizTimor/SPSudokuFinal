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

int ilp(Board* game);

int lp(Board* game, float th, int type, int row, int col);

#endif /* ILP_SOLVER_H_ */
