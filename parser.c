/*
* parser.h
*
*  Created on: 20 Dec 2018
*      Authors: Ido Lerer, Timor Eizenman
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "game.h"

#define MALLOC_ERROR "Error: malloc has failed\n"
#define WRONG_GAME_MODE_ERROR
 "Error: '%s' command is not available in the current game mode.\n
 the command is available in %s."

Command* create_command(int id, int params[3], char* error_message) {
	int i;
	Command* cmd = (Command*) malloc(sizeof(Command));
	if (cmd == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	cmd->id = id;
	cmd->error_message = error_message;
	for (i = 0; i < 3; i++) {
		cmd->params[i] = params[i];
	}
	return cmd;
}

void destroy_command(Command* cmd) {
	if (!cmd)
	return;
	free(cmd);
	return;
}

void print_command(Command* cmd) {
	if (!cmd)
	return;
	switch (cmd->id) {
		case SET:
		printf("%s %d %d %d", "set", cmd->params[0], cmd->params[1],
		cmd->params[2]);
		break;

		case HINT:
		printf("%s %d %d", "hint", cmd->params[0], cmd->params[1]);
		break;

		case VALIDATE:
		printf("validate");
		break;

		case RESTART:
		printf("restart");
		break;

		case EXIT:
		printf("exit");

	}
}
/*
* a function used to get the command id from its name.
*/
int get_command_type(char *type) {
	if (!type || type == '\0')
		return -1;
	if (!strcmp(type, "set"))
		return SET;
	if (!strcmp(type, "hint"))
		return HINT;
	if (!strcmp(type, "validate"))
		return VALIDATE;
	if (!strcmp(type, "restart"))
		return RESTART;
	if (!strcmp(type, "exit"))
		return EXIT;
	return -1;
}
/*
* a function used to get the command name from its id.
*/
char* get_command_name(enum command_id id) {
	switch (id) {
		case INVALID_COMMAND:
			return "invalid_command";
		case SOLVE:
			return "solve";
		case EDIT:
			return "edit";
		case MARK_ERORRS:
			return "mark_errors";
		case PRINT_BOARD:
			return "print_board";
		case SET:
			return "set";
		case VALIDATE:
			return "validate";
		case GUESS:
			return "guess";
		case GENERATE:
			return "generate";
		case UNDO:
			return "undo";
		case REDO:
			return "redo";
		case SAVE:
			return "save";
		case HINT:
			return "hint"
		case GUESS_HINT:
			return "guess_hint";
		case NUM_SOLUTIONS:
			return "num_solutions";
		case AUTOFILL:
			return "autofill";
		case RESET:
			return "reset";
		case RESTART:
			return "restart";
		case EXIT:
			return "exit";
	}
}

char* fill_params(char* command_name, int num_params, int params[3], int optional, char* str){
	const char delim[] = " \t\r\n";
	int i = 0;
	char *token = NULL;
	while ((token = strtok(NULL, delim)) != NULL ){
		if (i < num_params){
			params[i] = atoi(token);
		}
		else {
			return sprintf(
				"Error: too many parameters.\n'%s' command requires %d parameters.",
				 command_name, num_params);
		}
	}
	if (i < num_params && !optional){
		return sprintf(
			"Error: not enough parameters.\n'%s' command requires %d parameters.",
			 command_name, num_params);
	}
	return NULL;
}

Command* parse_command(char *str) {
	const char delim[] = " \t\r\n";
	int params[3] = { 0 }, i = 0, type;
	char *token = strtok(str, delim), *message;
	type = get_command_type(token);

	switch (type) {
		case SOLVE:
		message = fill_params("solve", 1, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case EDIT:
		message = fill_params("edit", 1, params, 1);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case MARK_ERORRS:
		if (current_game_mode != SOLVE)
			return create_command(INVALID_COMMAND, params,
				 sprintf(WRONG_GAME_MODE_ERROR, "mark_errors", "Solve").
		message = fill_params("mark_errors", 1, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case PRINT_BOARD:
		if (current_game_mode == INIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "print_board", "Solve and Edit").
		message = fill_params("print_board", 0, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case SET:
		if (current_game_mode == INIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "set", "Solve and Edit").
		message = fill_params("set", 3, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case VALIDATE:
		if (current_game_mode == INIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "validate", "Solve and Edit").
		message = fill_params("validate", 0, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case GUESS:
		if (current_game_mode != SOLVE)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "guess", "Solve").
		message = fill_params("guess", 1, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case GENERATE:
		if (current_game_mode != EDIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "generate", "Edit").
		message = fill_params("generate", 2, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case UNDO:
		if (current_game_mode == INIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "undo", "Solve and Edit").
		message = fill_params("undo", 0, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case REDO:
		if (current_game_mode == INIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "redo", "Solve and Edit").
		message = fill_params("redo", 0, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case SAVE:
		if (current_game_mode == INIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "save", "Solve and Edit").
		message = fill_params("save", 1, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);

		case SAVE:
		if (current_game_mode == INIT)
			return create_command(INVALID_COMMAND, params,
				sprintf(WRONG_GAME_MODE_ERROR, "save", "Solve and Edit").
		message = fill_params("save", 1, params, 0);
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);
	}
	return NULL;
}
