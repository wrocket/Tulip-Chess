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

#include "eval.h"

// Generate these with the board_scores.py script in /utils.

const int32_t SQ_SCORE_PAWN_OPENING_WHITE[144] = {
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,   -5,  -10,  -10,   -5,    0,    0,    0,    0,
        0,    0,    0,    0,   10,   10,   10,   10,    0,    0,    0,    0,
        0,    0,    0,    0,    8,   15,   15,    8,    0,    0,    0,    0,
        0,    0,    0,    0,    0,   15,   15,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

const int32_t SQ_SCORE_PAWN_OPENING_BLACK[144] = {
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,   15,   15,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    8,   15,   15,    8,    0,    0,    0,    0,
        0,    0,    0,    0,   10,   10,   10,   10,    0,    0,    0,    0,
        0,    0,    0,    0,   -5,  -10,  -10,   -5,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

const int32_t SQ_SCORE_KNIGHT_OPENING_WHITE[144] = {
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,  -20,   -5,    0,    0,    0,    0,   -5,  -20,    0,    0,
        0,    0,  -10,    0,    0,    5,    0,    5,    0,  -10,    0,    0,
        0,    0,  -10,    0,   10,    0,    0,   10,    0,  -10,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

const int32_t SQ_SCORE_KNIGHT_OPENING_BLACK[144] = {
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,  -10,    0,   10,    0,    0,   10,    0,  -10,    0,    0,
        0,    0,  -10,    0,    0,    5,    0,    5,    0,  -10,    0,    0,
        0,    0,  -20,   -5,    0,    0,    0,    0,   -5,  -20,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};

const int32_t SQ_SCORE_ENDGAME_KING[144] = {
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0, -100,  -75,  -65,  -50,  -50,  -65,  -75, -100,    0,    0,
        0,    0,  -75,  -65,  -45,  -45,  -45,  -45,  -65,  -75,    0,    0,
        0,    0,  -65,    0,    0,    0,    0,    0,    0,  -65,    0,    0,
        0,    0,  -50,    0,    0,    0,    0,    0,    0,  -50,    0,    0,
        0,    0,  -50,    0,    0,    0,    0,    0,    0,  -50,    0,    0,
        0,    0,  -65,  -50,    0,    0,    0,    0,  -50,  -65,    0,    0,
        0,    0,  -75,  -65,  -50,  -45,  -45,  -50,  -65,  -75,    0,    0,
        0,    0, -100,  -75,  -65,  -50,  -50,  -65,  -75, -100,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
