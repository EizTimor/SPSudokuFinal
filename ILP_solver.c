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
#define MALLOC_ERROR "Error: malloc has failed.\n"
#define CALLOC_ERROR "Error: calloc has failed.\n"

/*
 * Function: create_environment
 * ----------------------
 * 	Receives pointers to environment and model and creates the proper Gurobi items according to the type
 *
 */
int create_environment(GRBenv **env, GRBmodel **model, int type) {
	int e = 0;

	if (type == 0)
		e = GRBloadenv(env, "integerLinearProgram.log");
	else
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

/*
 * Function: free_all
 * ----------------------
 * 	Receives pointers to all allocated memory variables and frees them
 *
 */
void free_all(GRBenv* env, GRBmodel* model, double* sol, int* ind, double* obj,
		char* vtype, int* indexes) {
	free(sol);
	free(obj);
	free(ind);
	free(vtype);
	free(indexes);
	GRBfreemodel(model);
	GRBfreeenv(env);
}

/*
 * Function: add_variables
 * ----------------------
 * 	Receives environment, model, types array, indexes array, and relevant info regarding
 * 	the environment variables and add the variables to the Gurobi model according to the current
 * 	run (ILP/LP).
 *
 */
int add_variables(GRBenv **env, GRBmodel **model, char** vtype, int count,
		int type, int* indexes, int board_size) {
	int i, j = 0, k, e = 0, c, tmp, x;
	double* obj = (double*) malloc(sizeof(double) * count);
	if (!obj) {
		printf("%s", MALLOC_ERROR);
		exit(0);
	}

	if (type == 0)
		for (i = 0; i < count; i++) {
			(*vtype)[i] = GRB_BINARY;
			obj[i] = 1;
		}
	if (type == 1) {
		for (i = 0; i < board_size * board_size * board_size; i += board_size) {
			c = 0;
			tmp = 1;
			for (k = i; k < i + board_size; k++) {
				if (indexes[k] > 0) {
					c += 1;
				}
			}
			if (c != 0) {
				if (c >= 9 && c != board_size)
					x = (rand() % 4) * 2 - 1;
				else if (c == board_size)
					x = (rand() % 6) * 2 - 1;
				else
					x = board_size - c;
				while (x > 0) {
					tmp *= 5;
					x -= 1;
				}
				x = board_size - c;
				for (k = j; k < j + c; k++) {
					(*vtype)[k] = GRB_CONTINUOUS;
					obj[k] = (double) ((rand() % 7) + 1) / c * tmp;
				}
				j += c;
			}
		}
	}

	e = GRBaddvars(*model, count, 0, NULL, NULL, NULL, obj, NULL, NULL, *vtype,
	NULL);
	if (e) {
		printf("ERROR %d GRBaddvars(): %s\n", e, GRBgeterrormsg(*env));
		free(obj);
		return 0;
	}

	e = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (e) {
		printf("ERROR %d GRBsetintattr(): %s\n", e, GRBgeterrormsg(*env));
		free(obj);
		return 0;
	}

	e = GRBupdatemodel(*model);
	if (e) {
		printf("ERROR %d GRBupdatemodel(): %s\n", e, GRBgeterrormsg(*env));
		free(obj);
		return 0;
	}

	free(obj);
	return 1;
}

/*
 * Function: add_constraints
 * ----------------------
 * 	Receives environment, model, types, objectives and ind arrays, indexes array, and relevant info regarding
 * 	the environment variables and add the constraints to the Gurobi model.
 *
 */
int add_constraints(Board* game, GRBenv** env, GRBmodel** model, double** obj,
		int** ind, int* indexes, int count) {
	int i, j, k, f, g, e = 0, inde = 0, curr = 0;
	int single_ind[1] = { 0 };
	double single_obj[1] = { 1 };

	/*Adding cons. #0 each value of the program is at least 0...\n")*/
	for (i = 0; i < count; i++) {
		single_ind[0] = i;
		e = GRBaddconstr(*model, 1, single_ind, single_obj,
		GRB_GREATER_EQUAL, 0.0, "c0");
		if (e) {
			printf("ERROR %d constraint #0: %s\n", e, GRBgeterrormsg(*env));
			return 0;
		}
	}

	/*"Adding cons. #1 each cell has one value...\n")*/
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			if (game->current[i][j].value == DEFAULT) {
				for (k = 0; k < game->current[i][j].options->length; k++) {
					(*ind)[k] = inde++;
					(*obj)[k] = 1;
				}
				e = GRBaddconstr(*model, k, *ind, *obj,
				GRB_EQUAL, 1.0, "c1");
				if (e) {
					printf("ERROR %d constraint #1: %s\n", e,
							GRBgeterrormsg(*env));
					return 0;
				}
			}
		}
	}

	/*"Adding cons. #2 each row has one of each value...\n")*/
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			inde = i * game->board_size * game->board_size + j;
			curr = 0;
			for (k = 0; k < game->board_size; k++) {
				if (indexes[inde] > 0) {
					(*ind)[curr] = indexes[inde] - 1;
					(*obj)[curr++] = 1;
				}
				inde += game->board_size;
			}
			if (curr) {
				e = GRBaddconstr(*model, curr, *ind, *obj,
				GRB_EQUAL, 1.0, "c2");
				if (e) {
					printf("ERROR %d constraint #2: %s\n", e,
							GRBgeterrormsg(*env));
					return 0;
				}
			}
		}
	}

	/*"Adding cons. #3 each column has one of each value...\n")*/
	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			inde = i * game->board_size + j;
			curr = 0;
			for (k = 0; k < game->board_size; k++) {
				if (indexes[inde] > 0) {
					(*ind)[curr] = indexes[inde] - 1;
					(*obj)[curr++] = 1;
				}
				inde += game->board_size * game->board_size;
			}
			if (curr) {
				e = GRBaddconstr(*model, curr, *ind, *obj,
				GRB_EQUAL, 1.0, "c3");
				if (e) {
					printf("ERROR %d constraint #3: %s\n", e,
							GRBgeterrormsg(*env));
					return 0;
				}
			}
		}
	}

	/*"Adding cons. #4 each block has one of each value...\n");*/
	for (i = 0; i < game->block_col; i++) {
		for (j = 0; j < game->block_row; j++) {
			for (g = 0; g < game->board_size; g++) { /* value */
				curr = 0;
				for (k = 0; k < game->block_row; k++) {
					inde = (i * game->block_row + k) * game->board_size
							* game->board_size
							+ j * game->block_col * game->board_size + g;
					for (f = 0; f < game->block_col; f++) {
						if (indexes[inde] > 0) {
							(*ind)[curr] = indexes[inde] - 1;
							(*obj)[curr++] = 1;
						}
						inde += game->board_size;
					}
				}
				if (curr) {
					e = GRBaddconstr(*model, curr, *ind, *obj,
					GRB_EQUAL, 1.0, "c4");
					if (e) {
						printf(
								"ERROR %d constraint #4 i = %d, j = %d inde = %d: %s\n",
								e, i, j, inde, GRBgeterrormsg(*env));
						return 0;
					}
				}
			}
		}
	}

	return 1;
}

/*
 * Function: ilp_solution_to_board
 * ----------------------
 * 	Receives the Gurobi output and allocates it to the game board.
 *
 */
void ilp_solution_to_board(Board* game, double* sol, int* indexes) {
	int i, j, k, index;

	for (i = 0; i < game->board_size; i++)
		for (j = 0; j < game->board_size; j++)
			for (k = 0; k < game->board_size; k++) {
				index = i * game->board_size * game->board_size
						+ j * game->board_size + k;
				if (indexes[index] > 0 && sol[indexes[index] - 1] >= 1.0) {
					set_value(game, i + 1, j + 1, k + 1);
					break;
				}
			}
}

int ilp(Board* game) {
	int* indexes;
	int count = 0, i, j, k, curr = 0, index = 1;
	OptionNode* node;
	GRBenv* env = NULL;
	GRBmodel* model = NULL;
	int e = 0;
	double* sol;
	int* ind;
	double* obj;
	char* vtype;
	int optimstatus = 0;
	int status = 1;

	for (i = 0; i < game->board_size; i++)
		for (j = 0; j < game->board_size; j++)
			if (game->current[i][j].value == DEFAULT)
				if (game->current[i][j].options->length != 0)
					count += game->current[i][j].options->length;
				else
					return 0;

	indexes = (int*) calloc(
			game->board_size * game->board_size * game->board_size,
			sizeof(int));
	if (!indexes) {
		printf("%s", CALLOC_ERROR);
		return 0;
	}

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			curr = i * game->board_size * game->board_size
					+ j * game->board_size;
			if (game->current[i][j].value == DEFAULT) {
				node = game->current[i][j].options->top;
				while (indexes[curr + node->value - 1] == 0) {
					indexes[curr + node->value - 1] = index++;
					node = node->next;
				}
			} else {
				indexes[curr + game->current[i][j].value - 1] = -1;
			}
		}
	}

	ind = (int*) malloc(game->board_size * sizeof(int));
	sol = (double*) malloc(count * sizeof(double));
	obj = (double*) malloc(game->board_size * sizeof(double));
	vtype = (char*) malloc(count * sizeof(char));
	if (!ind || !sol || !obj || !vtype) {
		printf("%s", MALLOC_ERROR);
		exit(0);
	}

	status = create_environment(&env, &model, 0);
	status = status
			&& add_variables(&env, &model, &vtype, count, 0, indexes,
					game->board_size);
	status = status
			&& add_constraints(game, &env, &model, &obj, &ind, indexes, count);

	if (status) {
		e = GRBoptimize(model);
		if (e) {
			printf("ERROR %d GRBoptimize(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
		e = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
		if (e) {
			printf("ERROR %d GRBgetintattr(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
	}

	if (optimstatus == GRB_OPTIMAL) {
		e = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, count, sol);
		if (e) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", e,
					GRBgeterrormsg(env));
			status = 0;
		}
	} else
		status = 0;

	if (status) {
		ilp_solution_to_board(game, sol, indexes);
	}

	e = GRBwrite(model, "integerLinearProgram.lp");
	if (e) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", e, GRBgeterrormsg(env));
		status = 0;
	}
	free_all(env, model, sol, ind, obj, vtype, indexes);

	return status;
}

/*
 * Function: lp_solution_to_board
 * ----------------------
 * 	Receives the Gurobi output and allocates it to the game board.
 *
 */
void lp_solution_to_board(Board* game, double* sol, int* indexes, float th) {
	int i, j, k, index, c, r, b;
	int* tmp = (int*) malloc(sizeof(int) * game->board_size);
	if (!tmp) {
		printf("%s", MALLOC_ERROR);
		exit(0);
	}

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			c = 0;
			b = 0;
			if (game->current[i][j].value == DEFAULT) {
				for (k = 0; k < game->board_size; k++) {
					index = i * game->board_size * game->board_size
							+ j * game->board_size + k;
					if (indexes[index] > 0 && sol[indexes[index] - 1] >= th
							&& is_value_valid(game, i, j, k + 1)) {
						tmp[c] = k + 1;
						b += sol[indexes[index] - 1];
						c += 1;
					}
				}
				r = (double) rand() / RAND_MAX * b;
				if (c > 0) {
					while (c > 0) {
						b -= tmp[c - 1];
						if (b <= r)
							break;
						c -= 1;
					}
					if (c == 0)
						tmp[c++] = 0;
					set_value(game, i + 1, j + 1, tmp[c - 1]);
				}
			}
		}
	}
}

int lp(Board* game, float th, int type, int row, int col) {
	int* indexes;
	int count = 0, i, j, k, curr = 0, index = 1;
	OptionNode* node;
	GRBenv* env = NULL;
	GRBmodel* model = NULL;
	int e = 0;
	double* sol;
	int* ind;
	double* obj;
	char* vtype;
	int optimstatus = 0;
	int status = 1;

	for (i = 0; i < game->board_size; i++)
		for (j = 0; j < game->board_size; j++)
			if (game->current[i][j].value == DEFAULT)
				if (game->current[i][j].options->length != 0)
					count += game->current[i][j].options->length;
				else
					return 0;

	indexes = (int*) calloc(
			game->board_size * game->board_size * game->board_size,
			sizeof(int));
	if (!indexes) {
		printf("%s", CALLOC_ERROR);
		exit(0);
	}

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			curr = i * game->board_size * game->board_size
					+ j * game->board_size;
			if (game->current[i][j].value == DEFAULT) {
				node = game->current[i][j].options->top;
				while (indexes[curr + node->value - 1] == 0) {
					indexes[curr + node->value - 1] = index++;
					node = node->next;
				}
			} else {
				indexes[curr + game->current[i][j].value - 1] = -1;
			}
		}
	}

	ind = (int*) malloc(game->board_size * sizeof(int));
	sol = (double*) malloc(count * sizeof(double));
	obj = (double*) malloc(game->board_size * sizeof(double));
	vtype = (char*) malloc(count * sizeof(char));
	if (!ind || !sol || !obj || !vtype) {
		printf("%s", MALLOC_ERROR);
		exit(0);
	}

	status = create_environment(&env, &model, 1);
	status = status
			&& add_variables(&env, &model, &vtype, count, 1, indexes,
					game->board_size);
	status = status
			&& add_constraints(game, &env, &model, &obj, &ind, indexes, count);

	if (status) {
		e = GRBoptimize(model);
		if (e) {
			printf("ERROR %d GRBoptimize(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
		e = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
		if (e) {
			printf("ERROR %d GRBgetintattr(): %s\n", e, GRBgeterrormsg(env));
			status = 0;
		}
	}

	if (optimstatus == GRB_OPTIMAL) {
		e = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, count, sol);
		if (e) {
			printf("ERROR %d GRBgetdblattrarray(): %s\n", e,
					GRBgeterrormsg(env));
			status = 0;
		}
	} else
		status = 0;

	if (type) {
		if (status) {
			for (k = 0; k < game->board_size; k++) {
				index = row * game->board_size * game->board_size
						+ col * game->board_size + k;
				if (indexes[index] > 0 && sol[indexes[index] - 1] >= 0.000001)
					printf("Value %d has %f%%\n", k + 1,
							sol[indexes[index] - 1] * 100);
			}
		}
	} else if (status) {
		lp_solution_to_board(game, sol, indexes, th);
	}

	e = GRBwrite(model, "linearProgram.lp");
	if (e) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", e, GRBgeterrormsg(env));
		status = 0;
	}

	free_all(env, model, sol, ind, obj, vtype, indexes);

	return status;
}
