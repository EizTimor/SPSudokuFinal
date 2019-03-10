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

int main() {
	int is_game_live = 1;
	char in[MAX_COMMAND] = { 0 };
	while (is_game_live) {
		if (fgets(in, MAX_COMMAND, stdin) == NULL) {
			if (ferror(stdin)) {
				printf(FGETS_ERROR);
			}
			return 0;
		}
		if (in[MAX_COMMAND - 1] == 0) {
			printf("%s", COMMAND_TOO_LONG_ERROR);
			continue;
		}
		is_game_live = execute_command(parse_command(in));
	}
	return 0;
}
