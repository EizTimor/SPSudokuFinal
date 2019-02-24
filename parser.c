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

enum commandID {
	SET, HINT, VALIDATE, RESTART, EXIT
};

Command* createCommand(int id, int params[3]) {
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

void destroyCommand(Command* cmd) {
	if (!cmd)
		return;
	free(cmd);
	return;
}

void printCommand(Command* cmd) {
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
int getCommandType(char *type) {
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

Command* parseCommand(char *str) {
	const char delim[] = " \t\r\n";
	int params[3] = { 0 }, i = 0, type;
	char *token = strtok(str, delim);
	type = getCommandType(token);
	switch (type) {
	case SET:
		for (; i < 3; i++) {
			token = strtok(NULL, delim);
			if (token == NULL)
				return NULL;
			params[i] = atoi(token);
		}
		return createCommand(SET, params);
	case HINT:
		for (; i < 2; i++) {
			token = strtok(NULL, delim);
			if (token == NULL)
				return NULL;
			params[i] = atoi(token);
		}
		return createCommand(HINT, params);
	case VALIDATE:
		return createCommand(VALIDATE, params);
	case RESTART:
		return createCommand(RESTART, params);
	case EXIT:
		return createCommand(EXIT, params);
	}
	return NULL;
}
