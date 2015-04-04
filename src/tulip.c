// The MIT License (MIT)

// Copyright (c) 2015 Brian Wray (brian@wrocket.org)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "tulip.h"
#include "attack.h"
#include "gamestate.h"
#include "piece.h"
#include "board.h"
#include "statedata.h"
#include "movegen.h"
#include "move.h"
#include "util.h"
#include "fen.h"
#include "json.h"

void printBanner() {
	printf("Tulip Chess Engine 0.001\n");
	printf("Size of uint64: %lu bits\n", CHAR_BIT * sizeof(uint64_t));
}

void printState(int argc, char** argv) {
	if(argc != 2) {
		fprintf(stderr, "Usage: -printstate \"[FEN string]\"\n");
		exit(EXIT_FAILURE);
	}

	GameState gs;
	initializeGamestate(&gs);

	if(!parseFen(&gs, argv[1])) {
		fprintf(stderr, "Unable to parse FEN \"%s\"\n", argv[2]);
		destroyGamestate(&gs);
		exit(EXIT_FAILURE);
	}

	printGameState(argv[1], &gs);
	destroyGamestate(&gs);
}

void listAttacks(int argc, char** argv) {
	if(argc != 2) {
		fprintf(stderr, "Usage: -listattacks \"[FEN string]\"\n");
		exit(EXIT_FAILURE);
	}

	GameState gs;
	initializeGamestate(&gs);

	if(!parseFen(&gs, argv[1])) {
		fprintf(stderr, "Unable to parse FEN \"%s\"\n", argv[2]);
		destroyGamestate(&gs);
		exit(EXIT_FAILURE);
	}

	bool* attackGrid = ALLOC(144, bool, attackGrid, "Unable to allocate attack grid.");
	for(int rank=RANK_1; rank <= RANK_8; rank++) {
		for(int file=FILE_A; file <= FILE_H; file++) {
			int idx = B_IDX(file, rank);
			attackGrid[idx] = canAttack(gs.current->toMove, idx, &gs);
		}
	}

	printAttackList(argv[1], attackGrid, &gs);

	free(attackGrid);
	destroyGamestate(&gs);
}

void listMoves(int argc, char** argv) {
	if(argc != 3) {
		fprintf(stderr, "Usage: -listmoves [pseudo/legal] \"[FEN string]\"\n");
		exit(EXIT_FAILURE);
	}

	GameState gs;
	initializeGamestate(&gs);

	if(!parseFen(&gs, argv[2])) {
		fprintf(stderr, "Unable to parse FEN \"%s\"\n", argv[2]);
		destroyGamestate(&gs);
		exit(EXIT_FAILURE);
	}

	MoveBuffer buffer;
	createMoveBuffer(&buffer);
	
	if(0 == strcmp("pseudo", argv[1])) {
		generatePseudoMoves(&gs, &buffer);
	} else {
		fprintf(stderr, "Move list mode \"%s\" not supported (yet).\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	printMovelistJson(argv[2], argv[1], &buffer);

	destroyMoveBuffer(&buffer);
	destroyGamestate(&gs);
}

int main(int argc, char** argv) {

	argc--;
	argv++;

	if(argc >= 1) {
		if(0 == strcmp("-listmoves", argv[0])) {
			listMoves(argc, argv);
		} else if(0 == strcmp("-printstate", argv[0])) {
			printState(argc, argv);
		} else if(0 == strcmp("-listattacks", argv[0])) {
			listAttacks(argc, argv);
		} else {
			printBanner();
			printf("Unknown command \"%s\"\n", argv[0]);
		}
	} else {
		printBanner();
		printf("No operation mode specified, quitting.\n");
	}

	return EXIT_SUCCESS;
}
