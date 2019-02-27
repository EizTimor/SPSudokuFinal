/*
 * moves_list.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "turns_list.h"

movesList* create_moves_list() {
	movesList* list = (movesList*)malloc(sizeof(movesList));
	if (list == NULL) {
		/* error message */
		exit(0);
	}

	list->length = 0;
	list->top = NULL;

	return list;
}

void insert_move(movesList* moves, int row, int col, int prev_val, int new_val) {
	moveNode* node = (moveNode*)malloc(sizeof(moveNode));

	if (node == NULL) {
		/* error message */
		exit(0);
	}

	node->row = row;
	node->col = col;
	node->prev_val = prev_val;
	node->new_val = new_val;

	node->prev = moves->top->prev;
	moves->top->prev->next = node;
	moves->top->prev = node;
	node->next = moves->top;
	moves->length = moves->length + 1;
}

void destroy_moves_list(movesList* moves) {
	moveNode* node;

	while (!moves->length) {
		node = moves->top->next;
		free(moves->top);
		moves->top = node;
		moves->length = moves->length - 1;
	}

	free(moves);
}

turnsList* create_turns_list() {
	turnsList* list = (turnsList*)malloc(sizeof(turnsList));

	if (list == NULL) {
		/* error message */
		exit(0);
	}

	list->length = 0;
	list->top = NULL;

	return list;
}

void insert_turn(turnsList* turns, movesList* changes) {
	turnNode* node = (turnNode*)malloc(sizeof(turnNode));

	if (node == NULL) {
		/* error message */
		exit(0);
	}

	node->changes = changes;

	node->prev = turns->top->prev;
	turns->top->prev->next = node;
	turns->top->prev = node;
	node->next = turns->top;
	turns->length = turns->length + 1;
}

void destroy_turns_list(turnsList* turns) {
	turnNode* node;

	while (!turns->length) {
		node = turns->top->next;
		destroy_moves_list(turns->top->changes);
		free(turns->top);
		turns->top = node;
		turns->length = turns->length - 1;
	}

	free(turns);
}
