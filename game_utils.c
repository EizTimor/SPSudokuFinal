#include <stdio.h>
#include <stdlib.h>
#include "game_utils.h"

int save_board(Board* board, const char* path, int all_fixed) {
	char *fixed, *space;
	int i, j, val;
	FILE* file = fopen(path, "w");
	/* TODO: better error handling */
	if (file == NULL || board == NULL) {
		printf("Error opening file!\n");
		return 0;
	}
	fprintf(file, "%d %d\n", board->block_row, board->block_col);
	for (i = 0; i < board->board_size; i++) {
		for (j = 0; j < board->board_size; j++) {
			val = board->current[i][j].value;
			fixed = ((board->current[i][j].isFixed || all_fixed) && val) ?
					"." : "";
			space = (j == board->board_size - 1) ? "" : " ";
			fprintf(file, "%d%s%s", val, fixed, space);
		}
		fprintf(file, "\n");
	}

	fclose(file);
	return 1;
}

Board* load_board(char* path) {
	int block_row, block_col, val, is_fixed, i, j;
	char dot;
	Board* board;
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Error opening file!\n");
		return NULL;
	}
	printf("FILE OPENED\n");
	fscanf(file, "%d", &block_row);
	fscanf(file, "%d", &block_col);
	board = create_board(block_row, block_col);
	printf("BOARD CREATED\n");
	for (i = 0; i < board->board_size; i++) {
		for (j = 0; j < board->board_size; j++) {
			printf("i: %d, j: %d, board->size: %d\n", i, j, board->board_size);
			fscanf(file, "%d", &val);
			printf("Value scanned: %d\n", val);
			set_value(board, i + 1, j + 1, val);
			printf("Value was set.\n");
			fscanf(file, "%c", &dot);
			printf("Dot scanned: %c\n", dot);
			is_fixed = dot == '.' ? 1 : 0;
			printf("is_fixed: %d\n", is_fixed);
			if (is_fixed) {
				board->current[i][j].isFixed = 1;
			}
		}
	}
	fclose(file);
	return board;
}
