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

#ifndef RESULT_H
#define RESULT_H

#include "gamestate.h"

// Game continues.
#define STATUS_NONE 0

// White is checkmated.
#define STATUS_WHITE_CHECKMATED 1

// Black is checkmated.
#define STATUS_BLACK_CHECKMATED 2

// Stalemate.
#define STATUS_STALEMATE 3

// Material draw.
#define STATUS_MATERIAL_DRAW 4

// Threefold draw.
#define STATUS_THREEFOLD_DRAW 5

// Fifty-move draw.
#define STATUS_FIFTY_MOVE_DRAW 6

int getResult(GameState* g);

#endif
