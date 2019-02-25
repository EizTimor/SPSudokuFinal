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
  int id, i = 0, num_params;
	if (!cmd)
	 return;
  id = cmd->id;
  num_params = num_of_params(id);
  printf("%s", get_command_name(id));
  while (i < num_of_params) {
    printf(" %d", cmd->params[i]);
    i++;
  }
}

/*
* a function used to get the command id from its name.
*/
int get_command_id(char *type) {
  int id;
  if (!type || type == '\0')
		return -1;
  for(id = command_id.INVALID_COMMAND; i <= command_id.EXIT; id++){
    if (!strcmp(type, get_command_name(i)))
  		return id;
  }
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

/*
* returns a string containing the modes in which the command is available.
*/
char* get_command_modes(enum command_id id) {
	switch (id) {
		case INVALID_COMMAND:
			return NULL;
		case SOLVE:
    case EDIT:
    case EXIT:
			return "all game modes";
		case MARK_ERORRS:
    case GUESS:
    case HINT:
    case GUESS_HINT:
    case AUTOFILL:
			return "Solve";
		case PRINT_BOARD:
    case SET:
    case VALIDATE:
    case UNDO:
    case REDO:
    case SAVE:
    case NUM_SOLUTIONS:
    case RESET:
			return "Edit and Solve";
		case GENERATE:
    return "Edit";
	}
}

/*
* returns true iff command is available in current game mode.
*/
int is_command_available(enum command_id id) {
	switch (id) {
		case INVALID_COMMAND:
			return 0;
		case SOLVE:
    case EDIT:
    case EXIT:
			return 1;
		case MARK_ERORRS:
    case GUESS:
    case HINT:
    case GUESS_HINT:
    case AUTOFILL:
			return current_game_mode == game_mode.SOLVE;
		case PRINT_BOARD:
    case SET:
    case VALIDATE:
    case UNDO:
    case REDO:
    case SAVE:
    case NUM_SOLUTIONS:
    case RESET:
			return current_game_mode == game_mode.EDIT ||
      current_game_mode == game_mode.SOLVE;
		case GENERATE:
    return current_game_mode == game_mode.EDIT;
	}
}

/*
* returns the number of parameters the command expectects.
*/
int num_of_params(enum command_id id) {
	switch (id) {
		case INVALID_COMMAND:
    case PRINT_BOARD:
    case VALIDATE:
    case UNDO:
    case REDO:
    case AUTOFILL:
    case NUM_SOLUTIONS:
    case RESET:
    case EXIT:
			return 0;
    case SOLVE:
    case EDIT:
    case MARK_ERORRS:
    case GUESS:
    case SAVE:
      return 1;
    case GENERATE:
    case HINT:
    case GUESS_HINT:
      return 2;
    case SET:
      return 3;
	}
}

/*
* returns true iff the parameters of the command are optional.
*/
int is_params_optional(enum command_id id) {
	return id == EDIT;
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
	int params[3] = { 0 }, i = 0, id;
	char *token = strtok(str, delim), *message;
	id = get_command_id(token);
  if (id == -1){
    return NULL;
  } else {
		if (!is_command_available(id)){
      return create_command(INVALID_COMMAND, params,
        sprintf(WRONG_GAME_MODE_ERROR, get_command_name(id), get_command_modes(id)).
    }
		message = fill_params(get_command_name(id), num_of_params(id), params, is_params_optional(id));
		if (message)
			return create_command(INVALID_COMMAND, params, message);
		else
			return create_command(type, params, NULL);
  }
	return NULL;
}
