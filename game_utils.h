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
 *
 * 	returns: ?
 */
int save_board(Board* board, const char* path);
