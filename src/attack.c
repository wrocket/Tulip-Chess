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

// Script for generating the bitmask header file.

#include <stdint.h>
#include <stdbool.h>

#include "tulip.h"
#include "bitboard.h"
#include "attack.h"
#include "piece.h"

#define BITS(p) state->bitboards[(p)->ordinal]

static bool slide(const int startSq, const int offset, const Piece** board, const Piece* slider, const Piece* queen) {
	const Piece* p;
	int q = startSq;

	do {
		q += offset;
		p = board[q];
	}
	while (p == &EMPTY);

	return p == slider || p == queen;
}

bool canAttack(const int color, const int sq, GameState* state) {
	uint64_t mask;
	const Piece* knight;
	const Piece* bishop;
	const Piece* rook;
	const Piece* queen;
	const Piece* king;
	const Piece* pawn;
	uint64_t pawnMask;

	const Piece** b = state->current->board;

	bool result;

	if (color == COLOR_BLACK) {
		rook = &BROOK;
		bishop = &BBISHOP;
		queen = &BQUEEN;
		king = &BKING;
		knight = &BKNIGHT;
		pawn = &BPAWN;
		pawnMask = BITS_BPAWN[sq];
	} else {
		rook = &WROOK;
		bishop = &WBISHOP;
		king = &WKING;
		queen = &WQUEEN;
		knight = &WKNIGHT;
		pawn = &WPAWN;
		pawnMask = BITS_WPAWN[sq];
	}

	// Check for pawn attacks.
	if(BITS(pawn) & pawnMask) {
		return true;
	}

	// Look for knight attacks.
	if (BITS(knight) & BITS_KNIGHT[sq]) {
		return true;
	}

	// If an opposing rook or queen is on the file or rank...
	mask = BITS_ROOK[sq];
	if ((BITS(rook) & mask) || BITS(queen) & mask) {
		result = slide(sq, OFFSET_N, b, rook, queen)
			|| slide(sq, OFFSET_S, b, rook, queen)
			|| slide(sq, OFFSET_E, b, rook, queen)
			|| slide(sq, OFFSET_W, b, rook, queen);

		if (result) {
			return true;
		}
	}

	mask = BITS_BISHOP[sq];
	if ((BITS(bishop) & mask) || BITS(queen) & mask) {
		result = slide(sq, OFFSET_NE, b, bishop, queen)
			|| slide(sq, OFFSET_NW, b, bishop, queen)
			|| slide(sq, OFFSET_SE, b, bishop, queen)
			|| slide(sq, OFFSET_SW, b, bishop, queen);

		if (result) {
			return true;
		}
	}

	// Look for king attacks.
	if (BITS(king) & BITS_KING[sq]) {
		return true;
	}

	// No pawn attacks, no rook attacks, no bishop attacks, or knight attacks, or queen attacks...
	// Guess they can't attack.
	return false;
}
