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

#ifndef NOTATION_H
#define NOTATION_H

#include <inttypes.h>

#include "tulip.h"
#include "piece.h"
#include "gamestate.h"
#include "move.h"

// Prints the given move in the given game state to the given char buffer.
// Returns the number of characters printed, not including the null char.
int32_t printShortAlg(Move* move, GameState* gameState, char* buffer);

// Matches a human-input string against legal moves.
// This method isn't terribly fast, but it uses "fuzzy" matching logic.
// It will match with both coordinate notation (e.g. e2e4) and short
// algebraic notation (e.g. Ne6, Qd7#), accounting for differences in
// "decoration" (e.g. '++' instead of '#' for checkmate).
// Returns false if no match is made, else returns true, and places the
// relevant move information in the move argument.
bool matchMove(char* str, GameState* gs, Move* move);

// Prints a move in coordinate notation to a given char buffer.
// The char buffer must be at least 7 characters in size.
// Returns the number of characters printed to the buffer.
// The result will be null-terminated.
int32_t printMoveCoordinate(Move*, char*);

// Matches a *pseudo*move in coordinate algebraic form to a Move object for
// the given game state. Returns false if no such move exists.
bool matchPseudoMoveCoord(GameState* gameState, char* moveStr, Move* m);

#endif
