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
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>

#include "tulip.h"
#include "string.h"
#include "board.h"
#include "piece.h"
#include "hash.h"
#include "gamestate.h"
#include "statedata.h"
#include "piece.h"
#include "util.h"

#define _FEN_MAX_TOKENS     16
#define _FEN_MAX_TOKEN_LEN  64

static bool parseToMove(char * t, int* result, bool printErr) {
    if (strlen(t) != 1) {
        if (printErr) fprintf(stderr, "Invalid to-move string: %s\n", t);
        return false;
    }

    if (!strstr("bw", t)) {
        if (printErr) fprintf(stderr, "Invalid to-move string: %s\n", t);
        return false;
    }

    *result = (t[0] == 'b' ? COLOR_BLACK : COLOR_WHITE);
    return true;
}

static bool parseFiftyMove(char* t, int* result, bool printErr) {
    char* endToken;
    long parsed = strtol(t, &endToken, 10);
    int intVal;
    if (parsed >= 0 && parsed <= INT_MAX) {
        intVal = (int) parsed;
    }
    else {
        if (printErr) fprintf(stderr, "Fifty-move token \"%s\" out of range.", t);
        return false;
    }

    if (endToken == t) {
        if (printErr) fprintf(stderr, "Missing fifty-move token.\n");
        return false;
    }

    if (parsed < 0 || errno != 0) {
        if (printErr) fprintf(stderr, "Invalid fifty-move token \"%s\"", t);
        if (errno != 0) {
            if (printErr) fprintf(stderr, ": %s\n", strerror(errno));
        } else {
            if (printErr) fprintf(stderr, ".\n");
        }
        return false;
    }

    *result = intVal;
    return true;
}

static bool parseEpFile(char* t, int* result) {
    unsigned long len = strlen(t);
    bool valid = true;

    if (len == 1 && strcmp("-", t) == 0) {
        *result = NO_EP_FILE;
    } else if (len == 2 && (t[1] == '3' || t[1] == '6')) {
        const int file = parseFileChar(t[0]);
        if (file != INVALID_FILE) {
            *result = file;
        }
        else {
            valid = false;
        }
    } else {
        valid = false;
    }

    return valid;
}

static bool parseCastleFlags(char* t, int* result) {
    int flags = 0;

    if (strstr(t, "K")) {
        flags |= CASTLE_WK;
    }

    if (strstr(t, "Q")) {
        flags |= CASTLE_WQ;
    }

    if (strstr(t, "k")) {
        flags |= CASTLE_BK;
    }

    if (strstr(t, "q")) {
        flags |= CASTLE_BQ;
    }

    *result = flags;
    return true;
}

static bool parseBoard(char* t, GameState* gs, bool printErr) {
    const Piece** board = gs->board;
    int* pCounts = gs->pieceCounts;
    bool result = true;
    int currentFile = FILE_A;
    int currentRank = RANK_8;
    int squares = 0;
    int sqwk = -1;
    int sqbk = -1;

    char* c = t;
    while (*c && currentRank >= RANK_1) {
        if (*c == '/') {
            currentRank--;
            currentFile = FILE_A;
        } else if (isdigit(*c)) {
            int spaceDigits = *c - '0'; // Hack: Char to int. Might be funny in strange encodings...
            for (int i = 0; i < spaceDigits; i++) {
                board[B_IDX(currentFile++, currentRank)] = &EMPTY;
                pCounts[(&EMPTY)->ordinal]++;
                squares++;
            }
        } else {
            const Piece* p = parsePiece(*c);
            if (!p) {
                if (printErr) fprintf(stderr, "Invalid FEN. Unknown piece found: %c\n", *c);
                result = false;
                goto parse_board_err;
            }

            const int idx = B_IDX(currentFile++, currentRank);

            if (p == &WKING) {
                if (sqwk >= 0) {
                    if (printErr) fprintf(stderr, "Invalid FEN. Multiple white kings on the board.\n");
                    result = false;
                    goto parse_board_err;
                }

                sqwk = idx;
            } else if (p == &BKING) {
                if (sqbk >= 0) {
                    if (printErr) fprintf(stderr, "Invalid FEN. Multiple black kings on the board.\n");
                    result = false;
                    goto parse_board_err;
                }

                sqbk = idx;
            }

            pCounts[p->ordinal]++;

            board[idx] = p;
            squares++;
        }

        ++c;
    }

    if (sqbk < 0) {
        if (printErr) fprintf(stderr, "Invalid FEN. No black king on the board.\n");
        result = false;
    }

    if (sqwk < 0) {
        if (printErr) fprintf(stderr, "Invalid FEN. No white king on the board.\n");
        result = false;
    }

    if (squares != 64) {
        if (printErr) fprintf(stderr, "Invalid FEN. Expected to parse 64 squares, instead parsed %d.\n", squares);
        result = false;
    }

    gs->current->whiteKingSquare = sqwk;
    gs->current->blackKingSquare = sqbk;

    gs->current->whitePieceCount = pCounts[ORD_WPAWN] + pCounts[ORD_WKNIGHT] + pCounts[ORD_WBISHOP]
                                   + pCounts[ORD_WROOK] + pCounts[ORD_WQUEEN] + pCounts[ORD_WKING];

    gs->current->blackPieceCount = pCounts[ORD_BPAWN] + pCounts[ORD_BKNIGHT] + pCounts[ORD_BBISHOP]
                                   + pCounts[ORD_BROOK] + pCounts[ORD_BQUEEN] + pCounts[ORD_BKING];

parse_board_err:
    return result;
}

bool parseFenWithPrint(GameState* state, char* fenStr, bool printErrors) {
    bool result = true;
    char** tokenBuffer;
    int toMove = COLOR_WHITE;
    int castleFlags = 0;
    int epFile = NO_EP_FILE;
    int fiftyMove = 0;

    errno = 0;

    tokenBuffer = createTokenBuffer(_FEN_MAX_TOKENS, _FEN_MAX_TOKEN_LEN);

    int tokenCount = tokenize(fenStr, tokenBuffer, _FEN_MAX_TOKENS);

    if (tokenCount != 6) {
        if (printErrors) {
            fprintf(stderr, "Invalid FEN string; invalid token count (should be 6): %i.", tokenCount);
        }

        result = false;
        goto clean_tokens;
    }

    if (!parseBoard(tokenBuffer[0], state, printErrors)) {
        result = false;
        goto clean_tokens;
    }

    if (!parseToMove(tokenBuffer[1], &toMove, printErrors)) {
        result = false;
        goto clean_tokens;
    }

    if (!parseCastleFlags(tokenBuffer[2], &castleFlags)) {
        result = false;
        goto clean_tokens;
    }

    if (!parseEpFile(tokenBuffer[3], &epFile)) {
        result = false;
        goto clean_tokens;
    }

    if (!parseFiftyMove(tokenBuffer[4], &fiftyMove, printErrors)) {
        result = false;
        goto clean_tokens;
    }

    // Do additional validation on castle flags.
    // Some FEN producers send castle flags that don't match positions.
    if (state->current->whiteKingSquare != SQ_E1) {
        castleFlags &= ~(CASTLE_WK | CASTLE_WQ);
    }

    if (state->current->blackKingSquare != SQ_E8) {
        castleFlags &= ~(CASTLE_BK | CASTLE_BQ);
    }

    if (state->board[SQ_H1] != &WROOK) {
        castleFlags &= ~CASTLE_WK;
    }

    if (state->board[SQ_A1] != &WROOK) {
        castleFlags &= ~CASTLE_WQ;
    }

    if (state->board[SQ_H8] != &BROOK) {
        castleFlags &= ~CASTLE_BK;
    }

    if (state->board[SQ_A8] != &BROOK) {
        castleFlags &= ~CASTLE_BQ;
    }

    state->current->castleFlags = castleFlags;
    state->current->toMove = toMove;
    state->current->epFile = epFile;
    state->current->fiftyMoveCount = fiftyMove;

    reinitBitboards(state);
    state->current->hash = computeHash(state);

clean_tokens:
    freeTokenBuffer(tokenBuffer, _FEN_MAX_TOKENS);

    return result;
}

bool parseFen(GameState* state, char* fenStr) {
    return parseFenWithPrint(state, fenStr, true);
}

