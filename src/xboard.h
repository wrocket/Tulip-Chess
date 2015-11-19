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

#ifndef XBOARD_H
#define XBOARD_H

#include <stdbool.h>

#include "gamestate.h"
#include "book.h"
#include "log.h"

#define XBOARD_BUFF_LEN 1024

typedef struct {
    GameState gameState;
    bool forceMode;
    OpenBook currentBook;
    bool bookOpen;
    GameLog log;
    char* outputBuffer;
    int myTime;
    int opponentTime;
    bool postThinking;
} XBoardState;

bool startXBoard(void);
void postXBOutput(void* xBoardState, int ply, int score, long timeCentiseconds, long nodes, MoveBuffer* pv);
#endif
