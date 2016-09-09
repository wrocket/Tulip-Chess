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
#include <inttypes.h>

#include "tulip.h"
#include "piece.h"
#include "move.h"
#include "statedata.h"
#include "ztable.h"

// This is the maximum number of half-moves supportable in a game.
#define _GS_STACK_SIZE  512

#define MAX_MOVE_BUFFER 64

// This structure defines a GameState object.
// It will contain all the information needed to describe the current
// position and state of a game of chess.
// This is the "thread-safe" boundary. Do not share GameState objects
// between threads, as most methods that operate on a GameState will
// mutate it or rely on the non-mutation for correct functionality.
typedef struct {
    int32_t* pieceCounts;       // The current piece counts, indexed by the piece ordinal.
    const Piece** board;    // A 144 element array that corresponds to a 12x12 board.
    StateData* dataStack;   // A stack structure storing "small" data that changes with each move.
    StateData* current;     // The current state data object.
    bool created;           // Indicates that this structure has been initialized.
    uint64_t* bitboards;    // An array of bitboards, indexable by piece ordinal.
    MoveBuffer* moveBuffers;     // A series of move buffers for efficient search storage
    ZTable zTable;	// A Zobrist hash table for position hashing.
} GameState;

// Allocate memory and otherwise initialize a GameState to a default state.
void initializeGamestate(GameState*, int64_t);

// Free memory and otherwise release resources of a GameState.
void destroyGamestate(GameState*);

// Recalculate the bitboards array on the given state. Not performant.
void reinitBitboards(GameState* gs);

#endif
