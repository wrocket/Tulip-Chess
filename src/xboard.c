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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"
#include "xboard.h"
#include "gamestate.h"
#include "fen.h"
#include "notation.h"
#include "makeMove.h"

static void xBoardNew(XBoardState* xbs) {
    if (!parseFenWithPrint(&xbs->gameState, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false)) {
        printf("Error: Unable to set initial board position; invalid FEN. (?!?!)\n");
    }

    xbs->forceMode = false;
}

static void xBoardForce(XBoardState* xbs) {
    xbs->forceMode = true;
}

static void xBoardGo(XBoardState* xbs) {
    xbs->forceMode = false;
}

static void xBoardMove(XBoardState* xbs, char* move) {
    Move m;

    if (matchMove(move, &xbs->gameState, &m)) {
        makeMove(&xbs->gameState, &m);
    } else {
        printf("Illegal move: %s\n", move);
    }
}

static void xBoardProtover(XBoardState* xbs) {
    printf("ping=1\n");
    printf("san=1\n");
    printf("time=1\n");
    printf("sigint=0\n");
    printf("sigterm=0\n");
    printf("done=1\n");
}

static void xBoardSetboard(XBoardState* xbs, char** tokens, int tokenCount) {
    const int fenStrSize = 1024;
    char* fenStr;

    fenStr = malloc(fenStrSize * sizeof(char));
    if (!fenStr) {
        printf("Error: Unable to allocate memory for FEN.\n");
        return;
    }

    // Join the FEN tokens with single spaces.
    int idx = 0;
    for (int i = 1; i < tokenCount; i++) {
        if (i > 1) {
            fenStr[idx++] = ' ';
        }

        unsigned long len = strlen(tokens[i]);
        memcpy(fenStr + idx, tokens[i], len);
        idx += len;
    }

    fenStr[idx] = '\0';

    if (!parseFenWithPrint(&xbs->gameState, fenStr, false)) {
        printf("Error: Illegal position: %s\n", fenStr);
    }

    free(fenStr);
}

static bool isCommand(const char* command, char* str) {
    return strcmp(command, str) == 0;
}

bool startXBoard() {
    const int inputBufferSize = 1024;
    const int maxTokens = 32;
    const int maxTokenLen = 32;
    char* inputBuffer;
    bool result = true;
    char** tb;
    XBoardState xbState;

    inputBuffer = malloc(inputBufferSize * sizeof(char));
    if (!inputBuffer) {
        perror("Unable to allocate memory for XBoard input buffer.\n");
        result = false;
        goto cleanup_inputBuff;
    }

    tb = createTokenBuffer(maxTokens, maxTokenLen);
    if (!tb) {
        goto cleanup_tokenBuff;
    }

    initializeGamestate(&xbState.gameState);

    bool done = false;

    while (!done) {
        if (!fgets(inputBuffer, inputBufferSize, stdin)) {
            perror("Unable to read from stdin.");
            result = false;
            goto cleanup_gamestate;
        }

        int tokenCount = tokenize(inputBuffer, tb, maxTokens);
        if (tokenCount > 0) {
            char* cmd = tb[0];
            if (isCommand("xboard", cmd)) {

            } else if (isCommand("new", cmd)) {
                xBoardNew(&xbState);
            } else if (isCommand("protover", cmd)) {
                xBoardProtover(&xbState);
            } else if (isCommand("quit", cmd)) {
                done = true;
            } else if (isCommand("force", cmd)) {
                xBoardForce(&xbState);
            } else if (isCommand("go", cmd)) {
                xBoardGo(&xbState);
            } else if (isCommand("setboard", cmd)) {
                xBoardSetboard(&xbState, tb, tokenCount);
            } else {
                xBoardMove(&xbState, cmd);
            }
        }
    }

cleanup_gamestate:
    destroyGamestate(&xbState.gameState);
cleanup_tokenBuff:
    freeTokenBuffer(tb, maxTokens);
cleanup_inputBuff:
    return result;
}
