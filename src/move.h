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

#ifndef MOVE_H
#define MOVE_H

#include <stdbool.h>

#include "piece.h"

#define NO_MOVE_CODE	0	// Indicates nothing special about this move.
#define	CAPTURE_EP		1	// Indicates that a move is an en passant capture.
#define	PROMOTE_R		2	// Indicates that a move is a rook promotion.
#define PROMOTE_B		3	// Indicates that a move is a bishop promotion.
#define PROMOTE_N		4	// Indicates that a move is a knight promotion.
#define PROMOTE_Q		5	// Indicates that a move is a queen promotion.

#define IS_PROMOTE(code) ((code) >= PROMOTE_R)

// This describes the maximum number of moves that could be in a move buffer.
// This should be greater than the maximum number of psuedo-moves possible
// in any given position. TODO: See if this is actually correct...
#define MOVE_BUFFER_LENGTH	256

// Data structure describing a move.
typedef struct {
	int from;					// The "from" or "source" square index.
	int to;						// The "to" or "destination" square index.
	int moveCode;		// An integer that describes a special move condition
	// (e.g. PROMOTE_Q for a promotion to queen)
	const Piece* captures;		// A reference to any piece captured by this move.
	// If no capture, this will be &EMPTY.
	const Piece* movingPiece;	// A reference to the piece moving.
} Move;

// A simple list structure that knows its current length.
typedef struct {
    Move* moves;    // The list of moves in this buffer.
    int length;     // The current length of this buffer.
    bool created;   // Indicating proper allocation/destruction.
} MoveBuffer;

// Initialize a new move buffer to a given address.
void createMoveBuffer(MoveBuffer*);

// Deallocate a move buffer at a given address.
void destroyMoveBuffer(MoveBuffer*);

const Piece* getPromotePiece(const int color, const int moveCode);
#endif
