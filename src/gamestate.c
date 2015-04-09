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
    gs->bitboards = ALLOC_ZERO(ORD_MAX + 1, uint64_t, gs->bitboards, "Unable to allocate bitboards.");
    gs->dataStack = ALLOC(_GS_STACK_SIZE, StateData, gs->dataStack, "Unable to allocate state data stack.");
    gs->board = ALLOC(144, Piece*, gs->board, "Error allocating board array.");
    gs->pieceCounts = ALLOC_ZERO(ORD_MAX + 1, int, gs->pieceCounts, "Error allocating piece count array.");

    for(int i=0; i<_GS_STACK_SIZE; i++) {
        createStateData(&gs->dataStack[i]);
    }

    gs->current = &(gs->dataStack[0]);
    gs->created = true;

    for(int i=0; i<144; i++) {
        gs->board[i] = &OFF_BOARD;
    }

    for(int file = FILE_A; file <= FILE_H; file++) {
        for(int rank = RANK_1; rank <= RANK_8; rank++) {
            gs->board[B_IDX(file, rank)] = &EMPTY;
        }
    }
}

void reinitBitboards(GameState* gs) {
    for(int i=0; i<=ORD_MAX; i++) {
        gs->bitboards[i] = 0;
    }

    const Piece** board = gs->board;

    for(int i=0; i<64; i++) {
        int sq = BOARD_SQUARES[i];
        int ord = board[sq]->ordinal;
        (gs->bitboards[ord]) |= BITS_SQ[sq];
    }
}

void destroyGamestate(GameState* gs) {
    if(!(gs->created)) {
        fprintf(stderr, "Attempting to destroy gamestate that isn't created (or already destroyed)");
    }

    free(gs->bitboards);
    free(gs->dataStack);
    free(gs->board);
    free(gs->pieceCounts);
    gs->created = false;
}
