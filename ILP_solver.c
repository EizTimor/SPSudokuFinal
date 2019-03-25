/*
 * ILP_solver.c
 *
 *  Created on: Feb 25, 2019
 *      Author: Timor
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ILP_solver.h"

#define DEFAULT 0

int create_environment(GRBenv **env, GRBmodel **model) {
	int e = 0;

	e = GRBloadenv(env, "linearProgram.log");
	if (e) {
		printf("ERROR %d GRBloadenv(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	e = GRBsetintparam(*env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (e) {
		printf("ERROR %d GRBsetintparam(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	e = GRBnewmodel(*env, model, "linearProgram", 0, NULL, NULL, NULL, NULL,
	NULL);
	if (e) {
		printf("ERROR %d GRBnewmodel(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	return 1;
}

void free_all(GRBenv* env, GRBmodel* model, double* sol, int* ind, double* obj,
		char* vtype) {
	free(sol);
	free(obj);
	free(ind);
	free(vtype);
	GRBfreemodel(model);
	GRBfreeenv(env);
}

int ilp_add_variables(GRBenv **env, GRBmodel **model, char** vtype,
		int board_size) {
	int i, j, k, e = 0;

	for (i = 0; i < board_size; i++) {
		for (j = 0; j < board_size; j++) {
			for (k = 0; k < board_size; k++) {
				(*vtype)[i * (board_size * board_size) + (j * board_size) + k] =
				GRB_BINARY;
			}
		}
	}

	e = GRBaddvars(*model, board_size * board_size * board_size, 0, NULL,
	NULL, NULL, NULL, NULL, NULL, *vtype, NULL);
	if (e) {
		printf("ERROR %d GRBaddvars(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	e = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (e) {
		printf("ERROR %d GRBsetintattr(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	e = GRBupdatemodel(*model);
	if (e) {
		printf("ERROR %d GRBupdatemodel(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	return 1;
}

int ilp_add_constraints(Board* game, GRBenv** env, GRBmodel** model,
		double** obj, int** ind) {
	int i, j, k, e = 0;
	int out_row, out_col, in_row, in_col, index = 0;
	int c_ind[1];
	double c_val[1] = { 1 };

	printf("Adding cons. #1...\n");
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			for (k = 0; k < game->board_size; k++) {
				(*ind)[k] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[k] = 1;
			}
			e = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (e) {
				printf("ERROR %d constraint #1: %s\n", e, GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	printf("Adding cons. #2...\n");
	for (k = 0; k < game->board_size; k++) {
		for (i = 0; i < game->board_size; i++) {
			for (j = 0; j < game->board_size; j++) {
				(*ind)[j] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[j] = 1;
			}
			e = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (e) {
				printf("ERROR %d constraint #2: %s\n", e, GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	printf("Adding cons. #3...\n");
	for (k = 0; k < game->board_size; k++) {
		for (j = 0; j < game->board_size; j++) {
			for (i = 0; i < game->board_size; i++) {
				(*ind)[i] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[i] = 1;
			}
			e = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (e) {
				printf("ERROR %d constraint #3: %s\n", e, GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	printf("Adding cons. #4...\n");
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			k = game->current[i][j].value;
			if (k != DEFAULT) {
				c_ind[0] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k - 1;
				e = GRBaddconstr(*model, 1, c_ind, c_val,
				GRB_EQUAL, 1.0, NULL);
				if (e) {
					printf("ERROR %d constraint #4: %s\n", e,
							GRBgeterrormsg(*env));
					return 0;
				}
			}
		}
	}

	printf("Adding cons. #5...\n");
	for (k = 0; k < game->board_size; k++) {
		for (out_col = 0; out_col < game->block_col; out_col++) {
			for (out_row = 0; out_row < game->block_row; out_row++) {
				for (in_col = game->block_col * out_col;
						in_col < game->block_col + game->block_col * out_col;
						in_col++) {
					for (in_row = game->block_row * out_row;
							in_row < game->block_row + game->block_row * out_row;
							in_row++) {
						(*ind)[index] = in_row
								* (game->board_size * game->board_size)
								+ (in_col * game->board_size) + k;
						(*obj)[index] = 1;
						index++;
					}
				}
				e = GRBaddconstr(*model, game->board_size, *ind, *obj,
				GRB_EQUAL, 1.0, NULL);
				if (e) {
					printf("ERROR %d constraint #5: %s\n", e,
							GRBgeterrormsg(*env));
					return 0;
				}
				index = 0;
			}
		}
	}
	printf("Finished cons...\n");

	return 1;
}

void ilp_solution_to_board(Board* game, double* sol) {
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
	int e = 0;
	double* sol;
	int* ind;
	double* obj;
	char* vtype;
	int optimstatus;
	double objval;
	int status = 1;
	printf("Starting ilp...\n");

	ind = (int*) malloc(game->board_size * sizeof(int));
	sol = (double*) malloc(
			game->board_size * game->board_size * game->board_size
					* sizeof(double));
	obj = (double*) malloc(game->board_size * sizeof(double));
	vtype = (char*) malloc(
			game->board_size * game->board_size * game->board_size
					* sizeof(char));
	if (ind == NULL || sol == NULL || obj == NULL || vtype == NULL) {
		printf("Error: ilp malloc has failed\n");
		return 0;
	}

	printf("Creating Env...\n");
	status = create_environment(&env, &model);
	printf("Adding Variables...\n");
	status = status
			&& ilp_add_variables(&env, &model, &vtype, game->board_size);
	printf("Adding constraints...\n");
	status = status && ilp_add_constraints(game, &env, &model, &obj, &ind);

	printf("Optimizing...\n");
	e = GRBoptimize(model);
	if (e) {
		printf("ERROR %d GRBoptimize(): %s\n", e, GRBgeterrormsg(env));
		status = 0;
	}

	if (status) {
		e = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
		if (e) {
			printf("ERROR %d GRBgetintattr(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
		e = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
		if (e) {
			printf("ERROR %d GRBgetdoubleattr(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
		printf("Objval is %f\n", objval);
	}

	if (optimstatus == GRB_OPTIMAL) {
		e = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,
				game->board_size * game->board_size * game->board_size, sol);
		if (e) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", e,
					GRBgeterrormsg(env));
			status = 0;
		}
	} else
		status = 0;

	printf("Copying sol to board...\n");
	if (status)
		ilp_solution_to_board(game, sol);

	printf("Clearing Env...\n");
	free_all(env, model, sol, ind, obj, vtype);

	printf("Printing ILP\n");
	print_board(game);
	return status;
}

int lp_add_variables(GRBenv **env, GRBmodel **model, char** vtype,
		int board_size) {
	int i, j, k, e = 0;

	for (i = 0; i < board_size; i++) {
		for (j = 0; j < board_size; j++) {
			for (k = 0; k < board_size; k++) {
				(*vtype)[i * (board_size * board_size) + (j * board_size) + k] =
				GRB_CONTINUOUS;
			}
		}
	}

	e = GRBaddvars(*model, board_size * board_size * board_size, 0, NULL,
	NULL, NULL, NULL, NULL, NULL, *vtype, NULL);
	if (e) {
		printf("ERROR %d GRBaddvars(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	e = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (e) {
		printf("ERROR %d GRBsetdblattr(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	e = GRBupdatemodel(*model);
	if (e) {
		printf("ERROR %d GRBupdatemodel(): %s\n", e, GRBgeterrormsg(*env));
		return 0;
	}

	return 1;
}

int lp_add_constraints(Board* game, GRBenv** env, GRBmodel** model,
		double** obj, int** ind) {
	int i, j, k, e = 0;
	int out_row, out_col, in_row, in_col, index = 0;
	int c_ind[1];
	double c_val[1] = { 1.0 };

	printf("Adding cons. #1...\n");
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			for (k = 0; k < game->board_size; k++) {
				(*ind)[k] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[k] = 1.0;
			}
			e = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (e) {
				printf("ERROR %d constraint #1: %s\n", e, GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	printf("Adding cons. #2...\n");
	for (k = 0; k < game->board_size; k++) {
		for (i = 0; i < game->board_size; i++) {
			for (j = 0; j < game->board_size; j++) {
				(*ind)[j] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[j] = 1.0;
			}
			e = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (e) {
				printf("ERROR %d constraint #2: %s\n", e, GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	printf("Adding cons. #3...\n");
	for (k = 0; k < game->board_size; k++) {
		for (j = 0; j < game->board_size; j++) {
			for (i = 0; i < game->board_size; i++) {
				(*ind)[i] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k;
				(*obj)[i] = 1.0;
			}
			e = GRBaddconstr(*model, game->board_size, *ind, *obj,
			GRB_EQUAL, 1.0, NULL);
			if (e) {
				printf("ERROR %d constraint #3: %s\n", e, GRBgeterrormsg(*env));
				return 0;
			}
		}
	}

	printf("Adding cons. #4...\n");
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			k = game->current[i][j].value;
			if (k != DEFAULT) {
				c_ind[0] = i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k - 1;
				e = GRBaddconstr(*model, 1, c_ind, c_val,
				GRB_EQUAL, 1.0, NULL);
				if (e) {
					printf("ERROR %d constraint #4: %s\n", e,
							GRBgeterrormsg(*env));
					return 0;
				}
			}
		}
	}

	printf("Adding cons. #5...\n");
	for (k = 0; k < game->board_size; k++) {
		for (out_col = 0; out_col < game->block_col; out_col++) {
			for (out_row = 0; out_row < game->block_row; out_row++) {
				for (in_col = game->block_col * out_col;
						in_col < game->block_col + game->block_col * out_col;
						in_col++) {
					for (in_row = game->block_row * out_row;
							in_row < game->block_row + game->block_row * out_row;
							in_row++) {
						(*ind)[index] = in_row
								* (game->board_size * game->board_size)
								+ (in_col * game->board_size) + k;
						(*obj)[index] = 1;
						index++;
					}
				}
				e = GRBaddconstr(*model, game->board_size, *ind, *obj,
				GRB_EQUAL, 1.0, NULL);
				if (e) {
					printf("ERROR %d constraint #5: %s\n", e,
							GRBgeterrormsg(*env));
					return 0;
				}
				index = 0;
			}
		}
	}

	printf("Finished cons...\n");
	return 1;
}

void lp_solution_to_board(Board* game, double* sol) {
	int i, j, k;
	double r;
	double low, high;

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			for (k = 1; k < game->board_size; k++) {
				sol[i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k] += sol[i
						* (game->board_size * game->board_size)
						+ (j * game->board_size) + k - 1];
			}
		}
	}

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			r = (double) rand() / (double) RAND_MAX;
			printf("r is %f\n", r);
			high = 1;
			for (k = game->board_size - 1; k >= 0; k--) {
				low = sol[i * (game->board_size * game->board_size)
						+ (j * game->board_size) + k];
				printf("%f %f - ", low, high);
				if (low <= r && r <= high) {
					game->current[i][j].value = k + 2;
					break;
				}
				high = low;
			}
			if (game->current[i][j].value == DEFAULT)
				game->current[i][j].value = 1;
			printf("\n");
		}
	}
}

int lp(Board* game) {
	GRBenv* env = NULL;
	GRBmodel* model = NULL;
	int e = 0;
	double* sol;
	int* ind;
	double* obj;
	char* vtype;
	int optimstatus;
	double objval;
	int status = 1;
	printf("Statring lp...\n");

	ind = (int*) malloc(game->board_size * sizeof(int));
	sol = (double*) malloc(
			game->board_size * game->board_size * game->board_size
					* sizeof(double));
	obj = (double*) malloc(game->board_size * sizeof(double));
	vtype = (char*) malloc(
			game->board_size * game->board_size * game->board_size
					* sizeof(char));

	if (ind == NULL || sol == NULL || obj == NULL || vtype == NULL) {
		printf("Error: lp malloc has failed\n");
		return 0;
	}

	printf("Creating Env...\n");
	status = create_environment(&env, &model);
	printf("Adding Variables...\n");
	status = status && lp_add_variables(&env, &model, &vtype, game->board_size);
	printf("Adding constraints...\n");
	status = status && lp_add_constraints(game, &env, &model, &obj, &ind);

	printf("Optimizing...\n");
	e = GRBoptimize(model);
	if (e) {
		printf("ERROR %d GRBoptimize(): %s\n", e, GRBgeterrormsg(env));
		status = 0;
	}

	if (status) {
		e = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
		if (e) {
			printf("ERROR %d GRBgetintattr(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
		e = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
		if (e) {
			printf("ERROR %d GRBgetdoubleattr(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
		printf("Objval is %f\n", objval);
	}

	if (optimstatus == GRB_OPTIMAL) {
		e = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,
				game->board_size * game->board_size * game->board_size, sol);
		if (e) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", e,
					GRBgeterrormsg(env));
			status = 0;
		}
	} else {
		status = 0;
	}

	printf("Copying sol to board...%d\n", status);
	if (status)
		lp_solution_to_board(game, sol);

	printf("Clearing Env...\n");
	free_all(env, model, sol, ind, obj, vtype);

	printf("Printing LP\n");
	print_board(game);

	return status;
}
