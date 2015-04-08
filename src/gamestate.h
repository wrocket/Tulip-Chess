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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdbool.h>
#include <stdint.h>

#include "tulip.h"
#include "piece.h"
#include "statedata.h"

#define _GS_STACK_SIZE  512

typedef struct {
    int* pieceCounts;       // The current piece counts, indexed by the piece ordinal.
    const Piece** board;    // A 144 element array that corresponds to a 12x12 board.
    StateData* dataStack;
    int stackPtr;
    StateData* current;
    bool created;
    uint64_t* bitboards;
} GameState;

void initializeGamestate(GameState*);
void destroyGamestate(GameState*);
void fillBitboards(GameState*);
void reinitBitboards(GameState* gs);

#endif
