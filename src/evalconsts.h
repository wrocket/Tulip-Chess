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

#ifndef EVAL_CONSTS_H
#define EVAL_CONSTS_H

#define SCORE_PAWN      100
#define SCORE_KNIGHT    300
#define SCORE_BISHOP    325
#define SCORE_ROOK      500
#define SCORE_QUEEN     900

#define BONUS_RQ_SHARE_RANK 25

extern const int SQ_SCORE_PAWN_OPENING_WHITE[144];
extern const int SQ_SCORE_PAWN_OPENING_BLACK[144];
extern const int SQ_SCORE_KNIGHT_OPENING_BLACK[144];
extern const int SQ_SCORE_KNIGHT_OPENING_WHITE[144];

#endif
