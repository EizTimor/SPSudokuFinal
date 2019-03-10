/*
 * parser.h
 *
 *  Created on: 20 Dec 2018
 *      Authors: Ido Lerer, Timor Eizenman
 */
#ifndef PARSER_H_
#define PARSER_H_

#include "game.h"

/*
 * a unique ID for each command, the parser returns the ID corresponding to the parsed command.
 */
enum command_id {
	INVALID_COMMAND, SOLVE, EDIT, MARK_ERORRS, PRINT_BOARD, SET, VALIDATE,
	GUESS, GENERATE, UNDO, REDO, SAVE, HINT, GUESS_HINT, NUM_SOLUTIONS, AUTOFILL,
	RESET, EXIT
};

/*
 * Structure: Command
 * ------------------
 * 	A structure used to represent a command
 *
 * 	id : an integer representing the command type. List of all command types:
 * 			SET: 0, HINT: 1, VALIDATE: 2, RESTART: 3, EXIT: 4
 * 	params[]: an integer array storing command parameters.
 * 			For commands with less than 3 parameters excess integers are 0.
 * 	float_parm: a float variable for commands containing a float parameter.
 * 	string_parm: a char* variable for commands containing a string parameter.
 * 	error_message: if an error occurred during the parsing of the command, this variable
 * 					will contain the error description.
 */
typedef struct command_t{
	int id;
	int params[3];
	float float_param;
	char *string_param;
	char *error_message;
} Command;

/*
 * Function: parse_command
 * ----------------------
 * 	Receives a string and returns a command variable with the interpreted command.
 *
 * 	str[] : a character array with the string to be interpreted.
 *
 * 	returns: if the string represents a valid command, returns a command variable with
 * 			 the interpreted command. Otherwise returns a new command of type INVALID_COMMAND
 * 			 with an error message in the error_message command variable.
 */
Command* parse_command(char str[]);

/*
 * Function: destroy_command
 * ------------------------
 * 	Used to destroy a command structure and free the memory allocated to it.
 *
 * 	cmd : a pointer to the command structure.
 *
 */
void destroy_command(Command* cmd);

/*
 * Function: print_command
 * ------------------------
 * 	prints the string corresponding to the command.
 *
 * 	cmd : a pointer to the command structure.
 *
 */
void print_command(Command* cmd);

#endif /* PARSER_H_ */
