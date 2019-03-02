/*
 * ILP_solver.c
 *
 *  Created on: Feb 25, 2019
 *      Author: Timor
 */

#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "game.h"

int create_environment_and_variables(GRBenv **env, GRBmodel **model,
		char** vtype, int board_size) {
	int error = 0;
	int i, j, k;

	error = GRBloadenv(env, "ilp.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(*env));
		return 0;
	}

	error = GRBsetintparam(*env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(*env));
		return 0;
	}

	error = GRBnewmodel(*env, model, "ilp", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(*env));
		return 0;
	}

	for (i = 0; i < board_size; i++) {
		for (j = 0; j < board_size; j++) {
			for (k = 0; k < board_size; k++) {
				(*vtype)[i * (board_size * board_size) + (j * board_size) + k] =
				GRB_BINARY;
			}
		}
	}

	error = GRBaddvars(*model, board_size * board_size * board_size, 0, NULL,
	NULL, NULL, NULL, NULL, NULL, *vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(*env));
		return 0;
	}

	error = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(*env));
		return 0;
	}

	error = GRBupdatemodel(*model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(*env));
		return 0;
	}

	return 1;
}

int add_constraints(Board* game, GRBenv** env, GRBmodel** model, double** obj,
		int** ind) {
	int error = 0;
	int i, j, k;
	int out_row, out_col, in_row, in_col, index;
	int c_ind[1];
	double c_val[1] = { 1 };

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			for (k = 0; k < game->board_size; k++) {
				(*ind)[k] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[k] = 1;
			}
			error = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d constraint #1: %s\n", error,
						GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	for (k = 0; k < game->board_size; k++) {
		for (i = 0; i < game->board_size; i++) {
			for (j = 0; j < game->board_size; j++) {
				(*ind)[j] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[j] = 1;
			}
			error = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d constraint #2: %s\n", error,
						GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	for (k = 0; k < game->board_size; k++) {
		for (j = 0; j < game->board_size; j++) {
			for (i = 0; i < game->board_size; i++) {
				(*ind)[i] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[i] = 1;
			}
			error = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0,
			NULL);
			if (error) {
				printf("ERROR %d constraint #3: %s\n", error,
						GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			k = game->current[i][j].value;
			if (k != 0) {
				c_ind[0] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k - 1;
				error = GRBaddconstr(*model, 1, c_ind, c_val,
				GRB_EQUAL, 1.0, NULL);
				if (error) {
					printf("ERROR %d constraint #4: %s\n", error,
							GRBgeterrormsg(*env));
					return 0;
				}
			}
		}
	}

	for (k = 0; k < game->board_size; k++) {
		for (out_row = 0; out_row < game->block_row; out_row++) {
			for (out_col = 0; out_col < game->block_col; out_col++) {
				for (in_row = game->block_row * out_col;
						in_row < game->block_row + game->block_row * out_col;
						in_row++) {
					for (in_col = game->block_col * out_row;
							in_col < game->block_col + game->block_col * in_row;
							in_col++) {
						(*ind)[index] = in_col
								* (game->board_size * game->board_size)
								+ (in_row * game->board_size) + k;
						(*obj)[index] = 1;
						index++;
					}
				}
				error = GRBaddconstr(*model, game->board_size, *ind, *obj,
				GRB_EQUAL, 1.0,
				NULL);
				if (error) {
					printf("ERROR %d constraint #5: %s\n", error,
							GRBgeterrormsg(*env));
					return 0;
				}
				index = 0;
			}
		}
	}

	return 1;
}

void solution_to_board(Board* game, double* sol) {
	int i, j, k;

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			for (k = 0; k < game->board_size; k++) {
				if (sol[i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k] == 1.0) {
					game->current[i][j].value = k + 1;
				}
			}
		}
	}
}

int ilp(Board* game) {
	GRBenv* env = NULL;
	GRBmodel* model = NULL;
	int error = 0;
	double* sol;
	int* ind;
	double* obj;
	char* vtype;
	int optimstatus;
	int status = 1;

	ind = (int*) malloc(game->board_size * sizeof(int));
	sol = (double*) malloc(
			game->board_size * game->board_size * game->board_size
					* sizeof(double));
	obj = (double*) malloc(game->board_size * sizeof(double));
	vtype = (char*) malloc(
			game->board_size * game->board_size * game->board_size
					* sizeof(char));

	if (ind == NULL || sol == NULL || obj == NULL || vtype == NULL) {
		printf("Error: malloc has failed\n");
		exit(1);
	}

	status = create_environment_and_variables(&env, &model, &vtype,
			game->board_size);
	status = status
			&& add_constraints(game, &env, &model, &obj, &ind);

	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		status = 0;
	}

	if (status) {
		error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
		if (error) {
			printf("ERROR %d GRBgetintattr(): %s\n", error,
					GRBgeterrormsg(env));
			status = 0;
		}
	}

	if (optimstatus == GRB_OPTIMAL) {
		error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,
				game->board_size * game->board_size * game->board_size, sol);
		if (error) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", error,
					GRBgeterrormsg(env));
			status = 0;
		}
	} else
		status = 0;

	if (status)
		solution_to_board(game, sol);

	free(sol);
	free(obj);
	free(ind);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);

	return status;
}
