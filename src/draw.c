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

#include "tulip.h"
#include "piece.h"
#include "bitboard.h"
#include "gamestate.h"
#include "draw.h"

bool isMaterialDraw(GameState* g) {
    const int whiteCount = g->current->whitePieceCount;
    const int blackCount = g->current->blackPieceCount;
    const int total = whiteCount + blackCount;

    bool result = false;

    if (total <= 4) {
        if (total == 4) {
            // Deal with KB vs KB with bishops on same squre.
            if (g->pieceCounts[ORD_WBISHOP] == 1 && g->pieceCounts[ORD_BBISHOP] == 1) {
                const uint64_t wbishop = g->bitboards[ORD_WBISHOP];
                const uint64_t bbishop = g->bitboards[ORD_BBISHOP];
                result = ((BIT_SQUARES_LIGHT & wbishop) && (BIT_SQUARES_LIGHT & bbishop))
                    || ((BIT_SQUARES_DARK & wbishop) && (BIT_SQUARES_DARK & bbishop));
            }
        } else if (total == 3) {
            // Deal with KBvsK and KNvsK
            result = g->pieceCounts[ORD_WBISHOP] > 0
                        || g->pieceCounts[ORD_BBISHOP] > 0
                        || g->pieceCounts[ORD_WKNIGHT] > 0
                        || g->pieceCounts[ORD_WKNIGHT] > 0;
        } else { // Must be a piece count of 2, KvK, this is a draw.
            result = true;
        }
    } else {
        // Deal with king and arbitrary bishops vs king and arbitrary bishops.
        // If all the bishops are on the same color, and each side has at least
        // one bishop, and there are no other pieces, this is a draw.
        const int wbishops = g->pieceCounts[ORD_WBISHOP];
        const int bbishops = g->pieceCounts[ORD_BBISHOP];

        if (wbishops && bbishops && (wbishops + bbishops + 2) == total) {
            // TODO: optimize?
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
    }

    return result;
}
