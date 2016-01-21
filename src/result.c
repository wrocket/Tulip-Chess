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

#include "tulip.h"
#include "draw.h"
#include "attack.h"
#include "move.h"
#include "movegen.h"
#include "result.h"

int32_t getResult(GameState* g) {
        if (isMaterialDraw(g)) {
                return STATUS_MATERIAL_DRAW;
        }

        if (isThreefoldDraw(g)) {
                return STATUS_THREEFOLD_DRAW;
        }

        const int32_t moves = countLegalMoves(g);

        if (moves == 0) {
                const bool check = isCheck(g);

                if (check) {
                        return g->current->toMove == COLOR_WHITE ?
                               STATUS_WHITE_CHECKMATED : STATUS_BLACK_CHECKMATED;
                } else {
                        return STATUS_STALEMATE;
                }
        }

        return STATUS_NONE;
}
