#include <stdio.h>
#include <stdlib.h>
#include "game_utils.h"

int save_board(Board* board, const char* path){
  char fixed, space;
  int i, j, val;
  FILE* file = fopen(path, "w");
  // TODO: better error handling
  if (file == NULL || board == NULL) {
    printf("Error opening file!\n");
    return 0;
  }
  fprintf(file, "%d %d\n", board->block_row, board->block_col);
  for (i = 0; i < board->board_size; i++){
    for (j = 0; j < board->board_size; j++){
      val = board->current[i][j].value;
      fixed = (board->current[i][j].isFixed) ? '.' : NULL;
      space = (j == board->board_size - 1) ? NULL : ' ';
      fprintf(file, "%d%c%c", val, fixed, space);
    }
    fprintf(file, "\n");
  }

  fclose(file);
  return 1;
}

Board* load_board(char* path){
  int block_row, block_col, val, is_fixed, i, j;
  char dot;
  Board* board;
  FILE* file = fopen(path, "r");
  // TODO: better error handling
  if (file == NULL) {
    printf("Error opening file!\n");
    return NULL;
  }
  fscanf(file, "%d", &block_row);
  fscanf(file, "%d", &block_col);
  board = create_board(block_row, block_col);
  for (i = 0; i < board->board_size; i++){
    for (j = 0; j < board->board_size; j++){
      fscanf(file, "%d", &val);
      set_value(board, block_row, block_col, val);
      fscanf(file, "%c", &dot);
      is_fixed = dot == '.' ? 1 : 0;
      if (is_fixed) {
        board->current[i][j].isFixed = 1;
      }
    }
  }
  fclose(file);
  return board;
}
