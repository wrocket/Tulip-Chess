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
#include <stdarg.h>

#include "util.h"
#include "xboard.h"
#include "gamestate.h"
#include "fen.h"
#include "notation.h"
#include "makeMove.h"
#include "book.h"
#include "move.h"
#include "search.h"
#include "log.h"

static void xBoardWrite(XBoardState* xbs, const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    vsprintf(xbs->outputBuffer, format, argptr);
    va_end(argptr);

    writeLog(&xbs->log, "<<\t%s", xbs->outputBuffer); // Write to the log.
    printf("%s\n", xbs->outputBuffer); // Write to stdout.
    fflush(stdout);
}

static void logInput(XBoardState* xbs, char* message) {
    writeLog(&xbs->log, ">>\t%s", message);
}

static void logGameState(XBoardState* xbs) {
    char* outputMessage;
    outputMessage = malloc(1024);
    if (!outputMessage) {
        printf("Error: Unable to allocate log message buffer.\n");
        exit(-1);
    }

    printFen(&xbs->gameState, outputMessage, 1024);
    writeLog(&xbs->log, "Current game state: %s", outputMessage);

    free(outputMessage);
}

static void xBoardNew(XBoardState* xbs) {
    if (!parseFenWithPrint(&xbs->gameState, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", true)) {
        xBoardWrite(xbs, "Error: Unable to set initial board position; invalid FEN. (?!?!)");
    }

    xbs->forceMode = false;
}

static void xBoardForce(XBoardState* xbs) {
    xbs->forceMode = true;
}

static void xBoardApplyMove(XBoardState* xbs, Move* move) {
    makeMove(&xbs->gameState, move);
}

static bool xBoardThinkAndMove(XBoardState* xbs) {
    MoveBuffer mb;
    char moveStr[16];
    bool foundMove = false;
    Move move;

    // First check the book for this position.
    if (xbs->bookOpen) {
        createMoveBuffer(&mb);
        const int bookMoveCount = getMovesFromBook(&xbs->gameState, &mb, &xbs->currentBook);
        if (bookMoveCount > 0) {
            foundMove = true;
            move = mb.moves[rand() % bookMoveCount];
            writeLog(&xbs->log, "Found move from book.");
        }
        destroyMoveBuffer(&mb);
    } else {
        writeLog(&xbs->log, "Book not open, proceeding to search.");
    }

    // If nothing was found in the book, perform a search.
    if (!foundMove) {
        SearchResult searchResult;
        SearchArgs args;
        initSearchArgs(&args);
        args.depth = 6;
        args.log = &xbs->log;
        createSearchResult(&searchResult);

        search(&xbs->gameState, &args, &searchResult);

        if (searchResult.searchStatus != SEARCH_STATUS_NO_LEGAL_MOVES) {
            move = searchResult.move;
            foundMove = true;
        }

        destroySearchResult(&searchResult);
    }

    if (foundMove) {
        printShortAlg(&move, &xbs->gameState, moveStr);
        xBoardWrite(xbs, "move %s", moveStr);
        xBoardApplyMove(xbs, &move);
        logGameState(xbs);
    }

    return foundMove;
}

static void xBoardGo(XBoardState* xbs) {
    xbs->forceMode = false;
    xBoardThinkAndMove(xbs);
}

static void xBoardMove(XBoardState* xbs, char* move) {
    Move m;

    if (matchMove(move, &xbs->gameState, &m)) {
        makeMove(&xbs->gameState, &m);
        logGameState(xbs);

        if (!xbs->forceMode) {
            xBoardThinkAndMove(xbs);
        }
    } else {
        xBoardWrite(xbs, "Illegal move: %s", move);
    }
}

static void xBoardProtover(XBoardState* xbs) {
    xBoardWrite(xbs, "ping=1");
    xBoardWrite(xbs, "san=1");
    xBoardWrite(xbs, "time=1");
    xBoardWrite(xbs, "sigint=0");
    xBoardWrite(xbs, "sigterm=0");
    xBoardWrite(xbs, "done=1");
}

static void xBoardPing(XBoardState* xbs, char** tokens, int tokenCount) {
    if (tokenCount >= 2) {
        xBoardWrite(xbs, "pong %s", tokens[1]);
    }
}

static void xBoardSetboard(XBoardState* xbs, char** tokens, int tokenCount) {
    char* fenStr;

    // Computing the length of the joined string.
    unsigned long bufferSize = 0;

    // Add the lengths of all the token strings.
    for (int i = 0; i < tokenCount; i++) {
        bufferSize += strlen(tokens[i]);
    }

    // Add the number of spaces needed to join them.
    if (bufferSize > 0) {
        bufferSize += ((unsigned long) tokenCount - 1);
    }

    // Add one for the null char.
    bufferSize++;

    fenStr = malloc(bufferSize * sizeof(char));
    if (!fenStr) {
        xBoardWrite(xbs, "Error: Unable to allocate memory for FEN.");
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
        xBoardWrite(xbs, "Error: Illegal position: %s", fenStr);
    }

    free(fenStr);
}

static bool isCommand(const char* command, char* str) {
    return strcmp(command, str) == 0;
}

static void chopNewline(char* buff, int length) {
    for (int i = 0; i < length; i++) {
        char c = buff[i];
        if (c == '\n' || c == '\r') {
            buff[i] = '\0';
            break;
        }
    }
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
        perror("Error: Unable to allocate memory for XBoard input buffer.");
        result = false;
        goto cleanup_inputBuff;
    }

    xbState.outputBuffer = malloc(XBOARD_BUFF_LEN * sizeof(char));
    if (!xbState.outputBuffer) {
        perror("Error: Unable to allocate memory for XBoard output buffer.");
        result = false;
        goto cleanup_outputBuff;
    }

    tb = createTokenBuffer(maxTokens, maxTokenLen);
    if (!tb) {
        goto cleanup_inputBuff;
    }

    initializeGamestate(&xbState.gameState);

    const char* openingBook = "tulip_openings.sqlite";

    xbState.bookOpen = openBook(openingBook, &xbState.currentBook);

    bool done = false;

    if (!openLog(&xbState.log)) {
        xBoardWrite(&xbState, "Error: Unable to open game log.");
        goto cleanup_book;
    }

    while (!done) {
        if (!fgets(inputBuffer, inputBufferSize, stdin)) {
            perror("Error: Unable to read from stdin.");
            result = false;
            goto cleanup_gamestate;
        }

        chopNewline(inputBuffer, inputBufferSize);
        logInput(&xbState, inputBuffer);

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
            } else if (isCommand("ping", cmd)) {
                xBoardPing(&xbState, tb, tokenCount);
            } else {
                xBoardMove(&xbState, cmd);
            }
        }
    }

    closeLog(&xbState.log);
cleanup_book:
    if (xbState.bookOpen) {
        closeBook(&xbState.currentBook);
    }
cleanup_gamestate:
    destroyGamestate(&xbState.gameState);
    freeTokenBuffer(tb, maxTokens);
cleanup_outputBuff:
    free(inputBuffer);
cleanup_inputBuff:
    return result;
}
