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
#include <string.h>
#include <stdbool.h>

#include "tulip.h"
#include "gamestate.h"
#include "piece.h"
#include "statedata.h"
#include "bitboard.h"

void initializeGamestate(GameState* gs) {
	gs->dataStack = ALLOC(_GS_STACK_SIZE, StateData, gs->dataStack, "Unable to allocate state data stack.");

	for(int i=0; i<_GS_STACK_SIZE; i++) {
		createStateData(&gs->dataStack[i]);
	}

	gs->bitboards = ALLOC_ZERO(ORD_MAX, uint64_t, gs->bitboards, "Unable to allocate bitboards.");

	gs->stackPtr = 0;
	gs->current = &(gs->dataStack[0]);
	gs->created = true;
}

void reinitBitboards(GameState* gs) {
	for(int i=0; i<ORD_MAX; i++) {
		gs->bitboards[i] = 0;
	}

	const Piece** board = gs->current->board;

	for(int i=0; i<64; i++) {
		int sq = BOARD_SQUARES[i];
		int ord = board[sq]->ordinal;
		(gs->bitboards[ord]) |= BITS_SQ[sq];
	}
}

void destroyGamestate(GameState* gs) {
	if(!gs->created) {
		fprintf(stderr, "Attempting to destroy gamestate that isn't created (or already destroyed)");
	}

	for(int i=0; i<_GS_STACK_SIZE; i++) {
		destroyStateData(&gs->dataStack[i]);
	}

	free(gs->bitboards);
	free(gs->dataStack);
	gs->created = false;
}
