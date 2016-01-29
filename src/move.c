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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <inttypes.h>

#include "tulip.h"
#include "board.h"
#include "move.h"

void createMoveBuffer(MoveBuffer* buff) {
    Move* moves = ALLOC(MOVE_BUFFER_LENGTH, Move, moves, "Moves not allocated.");
    buff->moves = moves;
    buff->length = 0;
    buff->created = true;
}

void destroyMoveBuffer(MoveBuffer* buff) {
    if (!(buff->created)) {
        fprintf(stderr, "Attempting to destroy move buffer that hasn't been created (or was already destroyed)\n");
        return;
    }

    free(buff->moves);
    buff->length = 0;
    buff->created = false;
}


const Piece* getPromotePiece(const int32_t color, const int32_t moveCode) {
    if (color == COLOR_WHITE) {
        switch (moveCode) {
        case PROMOTE_Q: return &WQUEEN;
        case PROMOTE_N: return &WKNIGHT;
        case PROMOTE_B: return &WBISHOP;
        case PROMOTE_R: return &WROOK;
        default:
            fprintf(stderr, "Inside getPromotePiece() (white) with an invalid move code: %i\n", moveCode);
            return &WPAWN;
        }
    } else {
        switch (moveCode) {
        case PROMOTE_Q: return &BQUEEN;
        case PROMOTE_N: return &BKNIGHT;
        case PROMOTE_B: return &BBISHOP;
        case PROMOTE_R: return &BROOK;
        default:
            fprintf(stderr, "Inside getPromotePiece() (black) with an invalid move code: %i\n", moveCode);
            return &BPAWN;
        }
    }
}
