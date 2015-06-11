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

#ifndef JSON_H
#define JSON_H

#include "move.h"
#include "gamestate.h"
#include "statedata.h"

void printMovelistJson(char*, char*, GameState*, MoveBuffer*);
void printGameState(char*, GameState*);
void printCheckStatus(char*, bool isCheck);
void printMakeMoveResult(char* position, Move* m, GameState* state);
void printAttackList(char* position, bool* attackGrid, GameState* state);
void printMatchMoveResult(Move* move);
void printGameStatus(char* position, int status);

typedef struct {
    char move[8];
    StateData data;
} HashSeqItem;

void printHashSequence(HashSeqItem* items, int count, uint64_t initialHash);

#endif
