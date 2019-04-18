/*
 * parser.h
 *
 *      Authors: Ido Lerer, Timor Eizenman
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#define INV_COMMAND_ERROR "Error: invalid command."
#define MALLOC_ERROR "Error: malloc has failed.\n"
#define WRONG_GAME_MODE_ERROR "Error: '%s' command is not available in the current game mode.\nThe command is available in %s."
#define NOT_ENOUGH_PARAMS_ERROR "Error: not enough parameters.\n'%s' command requires %d parameters."
#define TOO_MANY_PARAMS_ERROR "Error: too many parameters.\n'%s' command requires %d parameters."

Command* create_command(int id, int params[3], float float_param,
		char* string_param, char* error_message) {
	int i;
	Command* cmd = (Command*) malloc(sizeof(Command));
	if (cmd == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	cmd->id = id;
	if (error_message) {
		cmd->error_message = malloc((strlen(error_message) + 1) * sizeof(char));
		strcpy(cmd->error_message, error_message);
	} else {
		cmd->error_message = NULL;
	}
	if (string_param) {
		cmd->string_param = malloc((strlen(string_param) + 1) * sizeof(char));
		strcpy(cmd->string_param, string_param);
	} else {
		cmd->string_param = NULL;
	}
	cmd->float_param = float_param;
	for (i = 0; i < 3; i++) {
		cmd->params[i] = params[i];
	}
	return cmd;
}

void destroy_command(Command* cmd) {
	if (!cmd)
		return;
	if(cmd->error_message)
		free(cmd->error_message);
	if(cmd->string_param)
		free(cmd->string_param);
	free(cmd);
	return;
}

/*
 * Used to get the command name from its id.
 */
const char* get_command_name(int id) {
	static char* names[] = { "invalid_command", "solve", "edit", "mark_errors",
			"print_board", "set", "validate", "guess", "generate", "undo",
			"redo", "save", "hint", "guess_hint", "num_solutions", "autofill",
			"reset", "exit" };
	if (id < INVALID_COMMAND || id > EXIT) {
		return 0;
	} else {
		return names[id];
	}
}

/*
 * Used to get the command id from its name.
 */
int get_command_id(char *type) {
	int id;
	if (!type || type == '\0')
		return -1;
	for (id = INVALID_COMMAND; id <= EXIT; id++) {
		if (!strcmp(type, get_command_name(id)))
			return id;
	}
	return -1;
}

/*
 * Returns the number of parameters the command expects.
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
	return 0;
}

/*
 * Returns a string containing the modes in which the command is available.
 */
const char* get_command_modes(enum command_id id) {
	static char* modes[] =
			{ "all game modes", "Solve", "Edit and Solve", "Edit" };
	switch (id) {
	case INVALID_COMMAND:
		return NULL;
	case SOLVE:
	case EDIT:
	case EXIT:
		return modes[0];
	case MARK_ERORRS:
	case GUESS:
	case HINT:
	case GUESS_HINT:
	case AUTOFILL:
		return modes[1];
	case PRINT_BOARD:
	case SET:
	case VALIDATE:
	case UNDO:
	case REDO:
	case SAVE:
	case NUM_SOLUTIONS:
	case RESET:
		return modes[2];
	case GENERATE:
		return modes[3];
	}
	return 0;
}

/*
 * Returns true iff command is available in current game mode.
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
		return current_game_mode == GAME_MODE_SOLVE;
	case PRINT_BOARD:
	case SET:
	case VALIDATE:
	case UNDO:
	case REDO:
	case SAVE:
	case NUM_SOLUTIONS:
	case RESET:
		return current_game_mode == GAME_MODE_EDIT
				|| current_game_mode == GAME_MODE_SOLVE;
	case GENERATE:
		return current_game_mode == GAME_MODE_EDIT;
	}
	return 0;
}

/*
 * Returns true iff the parameters of the command are optional.
 */
int is_params_optional(enum command_id id) {
	return id == EDIT;
}

/*
 * The function receives an array and fills it with the required parameters for the command_name.
 * The function assumes strtok was already called to fetch the command name and can be
 * called again with Null pointer to continue analyzing the received string input.
 * Returns 1 on success and 0 if an error occurred (in which case the error_message
 * will contain the error description).
 */
int fill_int_params(const char* command_name, int num_params, int params[3],
		int optional, char* error_message) {
	const char delim[] = " \t\r\n";
	int i = 0;
	char *token = NULL;
	while ((token = strtok(NULL, delim)) != NULL) {
		if (i < num_params) {
			params[i] = atoi(token);
		} else {
			sprintf(error_message, TOO_MANY_PARAMS_ERROR, command_name, num_params);
			return 0;
		}
		i++;
	}
	if (i < num_params && !optional) {
		sprintf(error_message, NOT_ENOUGH_PARAMS_ERROR, command_name, num_params);
		return 0;
	}
	return 1;
}

/*
 * The function receives a float pointer and fills it with the required parameter
 * for the command_name.
 * The function assumes "strtok" was already called to fetch the command name and can be
 * called again with Null pointer to continue analyzing the received string input.
 * Returns 1 on success and 0 if an error occurred (in which case the error_message
 * will contain the error description).
 */
int fill_float_params(const char* command_name, int num_params,
		float* float_param, int optional, char* error_message) {
	const char delim[] = " \t\r\n";
	int i = 0;
	char *token = NULL;
	while ((token = strtok(NULL, delim)) != NULL) {
		if (i < num_params) {
			*float_param = atof(token);
		} else {
			sprintf(error_message, TOO_MANY_PARAMS_ERROR, command_name, num_params);
			return 0;
		}
		i++;
	}
	if (i < num_params && !optional) {
		sprintf(error_message, NOT_ENOUGH_PARAMS_ERROR, command_name, num_params);
		return 0;
	}
	return 1;
}

/*
 * The function receives a string pointer and fills it with the required parameter
 * for the command_name.
 * The function assumes "strtok" was already called to fetch the command name and can be
 * called again with Null pointer to continue analyzing the received string input.
 * Returns 1 on success and 0 if an error occurred (in which case the error_message
 * will contain the error description).
 */
int fill_string_params(const char* command_name, int num_params, char** param,
		int optional, char *error_message) {
	const char delim[] = " \t\r\n";
	int i = 0;
	char *token = NULL;
	while ((token = strtok(NULL, delim)) != NULL) {
		if (i < num_params) {
			*param = token;
		} else {
			sprintf(error_message, TOO_MANY_PARAMS_ERROR, command_name, num_params);
			return 0;
		}
		i++;
	}
	if (i < num_params && !optional) {
		sprintf(error_message, NOT_ENOUGH_PARAMS_ERROR, command_name, num_params);
		return 0;
	}
	return 1;
}

/*
 * The function containing the parsing logic, used to convert a string with the user's input
 * to a Command structure. If an error has occurred during the parsing, a command
 * of type INVALID will be returned with the error description. Else, the command object
 * representing the user's command with all parameters filled will be returned.
 *
 */
Command* parse_command(char *str) {
	const char delim[] = " \t\r\n";
	int params[3] = { 0 }, id;
	char *token = strtok(str, delim), error_message[128], *string_param = NULL;
	float float_param = 0;
	const char* command_name;
	const char* command_modes;
	if (!token)
		return NULL;
	id = get_command_id(token);
	if (id == -1) {
		return create_command(INVALID_COMMAND, params, float_param,
				string_param, INV_COMMAND_ERROR);
	}
	command_name = get_command_name(id);
	command_modes = get_command_modes(id);
	if (!is_command_available(id)) {
		sprintf(error_message, WRONG_GAME_MODE_ERROR, command_name,
				command_modes);
		return create_command(INVALID_COMMAND, params, float_param,
				string_param, error_message);
	}
	switch (id) {
	case SOLVE:
	case EDIT:
	case SAVE:
		if (!fill_string_params(get_command_name(id), num_of_params(id),
				&string_param, is_params_optional(id), error_message)) {
			return create_command(INVALID_COMMAND, params, float_param,
					string_param, error_message);
		} else
			return create_command(id, params, float_param, string_param, NULL);
	case GUESS:
		if (!fill_float_params(get_command_name(id), num_of_params(id),
				&float_param, is_params_optional(id), error_message)) {
			return create_command(INVALID_COMMAND, params, float_param,
					string_param, error_message);
		} else
			return create_command(id, params, float_param, string_param, NULL);
	default:
		if (!fill_int_params(get_command_name(id), num_of_params(id), params,
				is_params_optional(id), error_message)) {
			return create_command(INVALID_COMMAND, params, float_param,
					string_param, error_message);
		} else
			return create_command(id, params, float_param, NULL, NULL);
	}
}

/*
 *A function used to print a command structure, useful for debugging.
 */
void print_command(Command* cmd) {
	int id, i = 0, num_params;
	if (!cmd)
		return;
	id = cmd->id;
	num_params = num_of_params(id);
	printf(
			"name: %s\nid: %d\nstring_param: %s\nfloat_param: %f\nerror_message: %s\nint params:",
			get_command_name(id), cmd->id, cmd->string_param, cmd->float_param,
			cmd->error_message);
	while (i < num_params) {
		printf(" %d", cmd->params[i]);
		i++;
	}
	printf("\n");
}
