#include "solver.h"
#include "game.h"
/*
 * Function: save_board
 * ----------------------
 * 	Receives a board and a string containing a  file path.
 * 	Saves the current board state to file in provided path, if it doesn't
 *  exist creates a new file.
 *
 * 	board : a Board of which current state should be saved.
 * 	path : the location of the file which will be created or saved to.
 * 	all_fixed : if not 0, all current values are saved fixed (used when saving in edit mode).
 *
 * 	returns: 1 if save succeeded, 0 else.
 */
int save_board(Board* board, const char* path, int all_fixed);

/*
 * Function: load_board
 * ----------------------
 * 	Receives a path to a file containg a board (formatted as saved board).
 * 	Reads the file and creates a board as described in it.
 *
 * 	path : the location of the file which contains the board.
 *
 * 	returns: a pointer to the loaded board, NULL if an error occurred.
 */
Board* load_board(char* path);
