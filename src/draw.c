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

    // TODO: Bug. An improbable position consisting of kings with an arbitrary number
    // of same colored bishops on the same color is a material draw. However, this is
    // a great speedup to simply discard this sort of nonsense...
    if (total <= 4) {
        if (total == 4) {
            if (g->pieceCounts[ORD_WBISHOP] == 1 && g->pieceCounts[ORD_BBISHOP] == 1) {
                const uint64_t wbishop = g->bitboards[ORD_WBISHOP];
                const uint64_t bbishop = g->bitboards[ORD_BBISHOP];

                // Either both bishops are on light squares, or both on dark squares.
                result = ((BIT_SQUARES_LIGHT & wbishop) && (BIT_SQUARES_LIGHT & bbishop))
                 || ((BIT_SQUARES_DARK & wbishop) && (BIT_SQUARES_DARK & bbishop));
            }
        } else if (total == 3) {
            result = g->pieceCounts[ORD_WBISHOP] > 0
                        || g->pieceCounts[ORD_BBISHOP] > 0
                        || g->pieceCounts[ORD_WKNIGHT] > 0
                        || g->pieceCounts[ORD_WKNIGHT] > 0;
        } else { // Must be a piece count of 2, KvK, this is a draw.
            result = true;
        }
    }

    return result;
}
