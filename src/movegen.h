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

#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <inttypes.h>

#include "move.h"

// Fills a given move buffer with pseudolegal moves, returning the number of
// moves generated. A psuedolegal move is a move that can be made if (but
// not only if) you ignore rules around moving into check. The assumption
// is that the caller will check the actual legality of the move before
// playing the move to a game state.
int32_t generatePseudoMoves(GameState* gameState, MoveBuffer* destination);

// Fills a given move buffer with legal moves. This checks the validity
// of every resulting gamestate from each pseudolegal move, so it has
// substantial runtime cost.
int32_t generateLegalMoves(GameState* gameState, MoveBuffer* destination);

// Counts the number of legal moves.
int32_t countLegalMoves(GameState* gameState);

#endif
