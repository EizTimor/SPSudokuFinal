/*
 * main.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "solver.h"
#include "parser.h"

#define MAX_COMMAND 257
#define FGETS_ERROR "Error: fgets has failed\n"
#define COMMAND_TOO_LONG_ERROR "Error: invalid command, too many characters\n"
#define WELCOME_MSG "Sudoku program started, please enter a command\n"

int main() {
	int is_game_live = 1;
	char in[MAX_COMMAND] = { 0 };
	Command* cmd;
	printf(WELCOME_MSG);
	while (is_game_live) {
		if (fgets(in, MAX_COMMAND, stdin) == NULL) {
			if (ferror(stdin)) {
				printf(FGETS_ERROR);
			}
			return 0;
		}
		if (in[MAX_COMMAND - 1] != 0) {
			printf("%s", COMMAND_TOO_LONG_ERROR);
			continue;
		}
		printf("Input recieved: %s\n", in);
		cmd = parse_command(in);
		if (!cmd)
			continue;
		printf("Command parsed:\n");
		print_command(cmd);
		printf("\n");
		is_game_live = execute_command(cmd);
		printf("Command executed: is_game_live = %d\n", is_game_live);
		destroy_command(cmd);
	}
	return 0;
}
