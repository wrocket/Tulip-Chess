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

#ifndef EVAL_H
#define EVAL_H

#include "gamestate.h"

#define ENDGAME_UNCLASSIFIED    0
#define ENDGAME_BROOKvKING      1
#define ENDGAME_WROOKvKING      2

// Returns the score of the position from the point of view of the side to move.
// So, if white is to move, a positive number is good for white.
// If black is to move, a positive number is good for black.
int32_t evaluate(GameState* state);

// Take an internal represntation of a score (integer, according to the side to
// move) and return a friendly version (double, with white advantage > 0 and
// black advantage < 0).
double friendlyScore(GameState* state, int32_t rawScore);

// Classify an endgame into one of several general types.
int32_t classifyEndgame(GameState* state);
#endif
