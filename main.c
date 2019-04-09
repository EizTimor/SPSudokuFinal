/*
 * main.c
 *
 *Author: Timor Eizenman & Ido Lerer
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "solver.h"
#include "parser.h"

#define MAX_COMMAND 258
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
		if (in[MAX_COMMAND - 2] != 0) {
			printf("%s", COMMAND_TOO_LONG_ERROR);
			in[MAX_COMMAND - 2] = 0;
			continue;
		}
		cmd = parse_command(in);
		if (!cmd)
			continue;
		is_game_live = execute_command(cmd);
		destroy_command(cmd);
	}
	return 0;
}
