#include <stdio.h>
#include <stdlib.h>
#include "game_utils.h"

int save_board(Board* board, const char* path){
  char* val, fixd, space;
  int i, j;
  FILE* file = fopen(path, "w");
  // TODO: better error handling
  if (file == NULL || board == NULL) {
    printf("Error opening file!\n");
    return 0;
  }
  fprintf(file, "%d %d\n", board->block_row, board->block_col);
  for (i = 0; i < board->board_size; i++){
    for (j = 0; j < board->board_size; j++){
      val = sprintf("%d", board->current[i][j].value);
      fixd = board->current[i][j].isFixed ? "." : "";
      space = j == board->board_size - 1 ? "" : " ";
      fprintf(file, "%s%s%s", val, fixd, space);
    }
    fprintf(file, "\n");
  }
  return 1;
}
