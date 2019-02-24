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

#define MALLOC_ERROR "Error: malloc has failed\n"

enum command_id {
	SET, HINT, VALIDATE, RESTART, EXIT
};

Command* create_command(int id, int params[3]) {
	int i;
	Command* cmd = (Command*) malloc(sizeof(Command));
	if (cmd == NULL) {
		printf(MALLOC_ERROR);
		exit(0);
	}
	cmd->id = id;
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

Command* parse_command(char *str) {
	const char delim[] = " \t\r\n";
	int params[3] = { 0 }, i = 0, type;
	char *token = strtok(str, delim);
	type = get_command_type(token);
	switch (type) {
	case SET:
		for (; i < 3; i++) {
			token = strtok(NULL, delim);
			if (token == NULL)
				return NULL;
			params[i] = atoi(token);
		}
		return create_command(SET, params);
	case HINT:
		for (; i < 2; i++) {
			token = strtok(NULL, delim);
			if (token == NULL)
				return NULL;
			params[i] = atoi(token);
		}
		return create_command(HINT, params);
	case VALIDATE:
		return create_command(VALIDATE, params);
	case RESTART:
		return create_command(RESTART, params);
	case EXIT:
		return create_command(EXIT, params);
	}
	return NULL;
}
