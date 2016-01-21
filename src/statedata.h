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

#ifndef STATE_DATA_H
#define STATE_DATA_H

#include <stdint.h>
#include <inttypes.h>

#include "tulip.h"
#include "board.h"

// Castle flags. The unsigned "castle flags" in the StateData struct is a bit
// field of these bits.
#define CASTLE_WK   0x1
#define CASTLE_WQ   0x2
#define CASTLE_BK   0x4
#define CASTLE_BQ   0x8

// Marker value to indicate an en passant file is not specified in the given
// position.
#define NO_EP_FILE  INVALID_FILE

// Structure to define the notion of StateData.
// This is a structure of "small" things (integers, booleans, etc) that change with each move on the chessboard.
// The idea is that we can keep a stack of these, pushing a new item on each move, providing a handy way to
// peek back in history (e.g. threefold detection) or undo a move.
// There should be no heap-allocated members in this structure.
typedef struct {
        int32_t toMove;             // The side to move, either COLOR_WHITE or COLOR_BLACK
        int32_t castleFlags;        // The castle flags as a bitmap; see CASTLE_WK and others.
        int32_t whiteKingSquare;    // The current square index of the white king.
        int32_t blackKingSquare;    // The current square index of the black king.
        int32_t epFile;             // The current en passant file, if any.
        int32_t fiftyMoveCount;     // The fifty move count. This is the number of half-moves since a capture or pawn move.
        int32_t halfMoveCount;      // The half move count. This increments by one after every move.
        uint64_t hash;          // The current state hash.
        int32_t whitePieceCount;    // The current white piece count.
        int32_t blackPieceCount;    // The current black piece count.
} StateData;

// Allocate memory for an new state data object.
void createStateData(StateData *);

// Copy data from one state data object "from" to another "to".
void copyStateData(StateData* from, StateData* to);

#endif
