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
#include <unistd.h>

#define DEFAULT 0

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

int add_variables(GRBenv **env, GRBmodel **model, char** vtype, int count,
		int type) {
	int i, j, k, e = 0;

	if (type == 0)
		for (i = 0; i < count; i++)
			(*vtype)[i] = GRB_BINARY;
	if (type == 1)
		for (i = 0; i < count; i++)
			(*vtype)[i] = GRB_CONTINUOUS;

	e = GRBaddvars(*model, count, 0, NULL, NULL, NULL, NULL, NULL, NULL, *vtype,
	NULL);
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

int add_constraints(Board* game, GRBenv** env, GRBmodel** model, double** obj,
		int** ind, int* indexes) {
	int i, j, k, f, g, e = 0, inde = 0, curr = 0;

	printf("Adding cons. #1 each cell has one value...\n");
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

	printf("Adding cons. #2 each row has one of each value...\n");
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

	printf("Adding cons. #3 each column has one of each value...\n");
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

	printf("Adding cons. #4 each block has one of each value...\n");
	for (i = 0; i < game->block_row; i++) {
		for (j = 0; j < game->block_col; j++) {
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

	printf("Finished cons...\n");

	return 1;
}

void solution_to_board(Board* game, double* sol, int* indexes, float th) {
	int i, j, k, index;

	for (i = 0; i < game->board_size; i++) {
		for (j = 0; j < game->board_size; j++) {
			for (k = 0; k < game->board_size; k++) {
				index = i * game->board_size * game->board_size
						+ j * game->board_size + k;
				if (sol[indexes[index] - 1] >= th)
					set_value(game, i + 1, j + 1, k + 1);
			}
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
	printf("Starting ilp...\n");

	printf("Counting...\n");
	for (i = 0; i < game->board_size; i++)
		for (j = 0; j < game->board_size; j++)
			if (game->current[i][j].value == DEFAULT)
				if (game->current[i][j].options->length != 0)
					count += game->current[i][j].options->length;
				else
					return 0;

	printf("Marking indexes...\n");
	indexes = (int*) calloc(
			game->board_size * game->board_size * game->board_size,
			sizeof(int));
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
	if (indexes == NULL || ind == NULL || sol == NULL || obj == NULL
			|| vtype == NULL) {
		printf("Error: ilp malloc has failed\n");
		return 0;
	}

	printf("count is %d\n", count);
	printf("Creating Env...\n");
	status = create_environment(&env, &model);
	printf("Adding Variables...\n");
	status = status && add_variables(&env, &model, &vtype, count, 0);
	printf("Adding constraints...\n");
	status = status && add_constraints(game, &env, &model, &obj, &ind, indexes);

	printf("Optimizing with status %d...\n", status);
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

	printf("Status %d\n", status);
	if (status) {
		printf("Copying to board...\n");
		solution_to_board(game, sol, indexes, 1.0);
	}

	e = GRBwrite(model, "integerLinearProgram.lp");
	if (e) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", e, GRBgeterrormsg(env));
		status = 0;
	}
	printf("Clearing Env...\n");
	free_all(env, model, sol, ind, obj, vtype, indexes);

	printf("Printing ILP\n");
	print_board(game);
	return status;
}

int lp(Board* game, float th) {
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
	printf("Starting lp...\n");

	printf("Counting...\n");
	for (i = 0; i < game->board_size; i++)
		for (j = 0; j < game->board_size; j++)
			if (game->current[i][j].value == DEFAULT)
				if (game->current[i][j].options->length != 0)
					count += game->current[i][j].options->length;
				else
					return 0;

	printf("Marking indexes...\n");
	indexes = (int*) calloc(
			game->board_size * game->board_size * game->board_size,
			sizeof(int));
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
	if (indexes == NULL || ind == NULL || sol == NULL || obj == NULL
			|| vtype == NULL) {
		printf("Error: ilp malloc has failed\n");
		return 0;
	}

	printf("count is %d\n", count);
	printf("Creating Env...\n");
	status = create_environment(&env, &model);
	printf("Adding Variables...\n");
	status = status && add_variables(&env, &model, &vtype, count, 1);
	printf("Adding constraints...\n");
	status = status && add_constraints(game, &env, &model, &obj, &ind, indexes);

	printf("Optimizing with status %d...\n", status);
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

	printf("Status %d\n", status);
	if (status) {
		printf("Copying to board...\n");
		solution_to_board(game, sol, indexes, th);
	}

	e = GRBwrite(model, "linearProgram.lp");
	if (e) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", e, GRBgeterrormsg(env));
		status = 0;
	}
	printf("Clearing Env...\n");
	free_all(env, model, sol, ind, obj, vtype, indexes);

	printf("Printing LP\n");
	print_board(game);
	return status;
}
