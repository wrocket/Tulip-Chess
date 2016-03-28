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

#include <inttypes.h>

#include "bitboard.h"
#include "draw.h"
#include "gamestate.h"
#include "hashconsts.h"
#include "piece.h"
#include "tulip.h"

// Deal with king and arbitrary bishops vs king and arbitrary bishops.
// If all the bishops are on the same color, and each side has at least
// one bishop, and there are no other pieces, this is a draw.
static inline bool moreThanFourPieces(GameState* g, const int32_t total) {
	bool result = false;
	const int32_t countWb = g->pieceCounts[ORD_WBISHOP];
	const int32_t countBb = g->pieceCounts[ORD_BBISHOP];

	if (countWb && countBb && (countWb + countBb + 2) == total) {
		const uint64_t wbishop = g->bitboards[ORD_WBISHOP];
		const uint64_t bbishop = g->bitboards[ORD_BBISHOP];

		const bool wbishopsOnLight = (BIT_SQUARES_LIGHT & wbishop) != 0;
		const bool wbishopsOnDark = (BIT_SQUARES_DARK & wbishop) != 0;
		const bool bbishopsOnLight = (BIT_SQUARES_LIGHT & bbishop) != 0;
		const bool bbishopsOnDark = (BIT_SQUARES_DARK & bbishop) != 0;

		const bool allWBishopsOnSameColor = wbishopsOnLight ^ wbishopsOnDark;
		const bool allBBishopsOnSameColor = bbishopsOnLight ^ bbishopsOnDark;
		const bool allBishopsOnSameColor = ((wbishopsOnLight && bbishopsOnLight) || (wbishopsOnDark && bbishopsOnDark));

		result = allWBishopsOnSameColor && allBBishopsOnSameColor && allBishopsOnSameColor;
	}

	return result;
}

// Deal with KB vs KB with bishops on same square color.
static inline bool fourPieces(GameState* g, const int32_t total) {
	bool result = false;

	if (g->pieceCounts[ORD_WBISHOP] == 1 && g->pieceCounts[ORD_BBISHOP] == 1) {
		const uint64_t wbishop = g->bitboards[ORD_WBISHOP];
		const uint64_t bbishop = g->bitboards[ORD_BBISHOP];
		result = ((BIT_SQUARES_LIGHT & wbishop) && (BIT_SQUARES_LIGHT & bbishop))
				 || ((BIT_SQUARES_DARK & wbishop) && (BIT_SQUARES_DARK & bbishop));
	}

	return result;
}

static inline bool threePieces(GameState* g, const int32_t total) {
	// If there are three pieces, two must be kings.
	// That means there's one non-king piece.
	// If that one piece is a knight or bishop of any color, we're in a draw.
	return g->pieceCounts[ORD_WBISHOP] > 0
		   || g->pieceCounts[ORD_BBISHOP] > 0
		   || g->pieceCounts[ORD_WKNIGHT] > 0
		   || g->pieceCounts[ORD_WKNIGHT] > 0;
}

bool draw_isMaterial(GameState* g) {
	const int32_t total = g->current->whitePieceCount + g->current->blackPieceCount;
	bool result;
	switch (total) {
	case 4: // Exactly four pieces looks for KBvKB with same-color bishops.
		result = fourPieces(g, total);
		break;
	case 3:
		result = threePieces(g, total); // Look for KNvK or KBvK
		break;
	case 2:
		result = true; // Two pieces means KvK, a draw.
		break;
	default:
		result = moreThanFourPieces(g, total); // Look for kings and bishops where bishops are same-color.
		break;
	}

	return result;
}

bool draw_isThreefold(GameState* g) {
	StateData* currentData = g->current;
	const uint64_t startingHash = currentData->hash;
	bool result = false;
	int32_t repeatCount = 0;
	const int limit = MIN(currentData->fiftyMoveCount, currentData->halfMoveCount);

	for (int32_t i = 0; i < limit; i++) {
		currentData--;
		const uint64_t currentHash = currentData->hash;
		// Hash changes with toMove, so compare the hash with either white to move or black to move.
		if (currentHash == startingHash || (currentHash ^ HASH_WHITE_TO_MOVE) == startingHash) {
			repeatCount++;
			if (repeatCount == 2) {
				result = true;
				break;
			}
		}
	}

	return result;
}
