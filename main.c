/*
 * main.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Timor Eizenman & Ido Lerer
 */

#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "mainAux.h"
#include <time.h>

#define DEFAULT 0
#define SIMPLE 3

int main() {
	Board* board;
	int game_active = 1, fixed_amount;

	while (game_active) {
		fixed_amount = get_fixed_amount();

		if (fixed_amount == -1)
			exit(0);

		board = create_board(SIMPLE, SIMPLE, fixed_amount);
		game_active = start_game(board);
	}
	return 0;
}
