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

#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "gamestate.h"
#include "move.h"

// Applies a given move to the given game state.
// This will update all relevant state data reflecting the new move.
// Important: This does not check that the move is actually a legal move in the
// game state. Applying an illegal or nonesense move can permanently corrupt the
// game state/
void makeMove(GameState* gameState, Move* move);

// Unmakes a given move from a given game state.
// This assumes that the move being unmade was the move that was just applied
// to achieve the current state.
// Important: This does not check that the move is actually a legal move in the
// game state. Unapplying an illegal or nonesense move can permanently corrupt the
// game state/
void unmakeMove(GameState* gameState, Move* move);

#endif
