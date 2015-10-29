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
#include <inttypes.h>
#include <sys/stat.h>

#include "sqlite/sqlite3.h"

#include "book.h"
#include "tulip.h"
#include "gamestate.h"
#include "move.h"
#include "notation.h"

#define BOOK_MAX_MOVES 256
#define BOOK_MAX_STR_LEN 8

static void initializeStrArray(char*** array) {
    *array = ALLOC(BOOK_MAX_MOVES, sizeof(char*), *array, "Unable to allocate memory for book move string array.");
    for (int i = 0; i < BOOK_MAX_MOVES; i++) {
        (*array)[i] = ALLOC(BOOK_MAX_STR_LEN, sizeof(char), (*array)[i], "Unable to allocate memory for book move string.");
    }
}

static void destroyStrArray(char*** array) {
    for (int i = 0; i < BOOK_MAX_STR_LEN; i++) {
        free((*array)[i]);
    }
    free(*array);
}

static int readMoves(GameState* state, char*** strArray, OpenBook* book) {
    int moveCount = 0;
    sqlite3* db = book->database;
    sqlite3_stmt *res;
    char hashStr[17]; // 16 chars plus null terminator

    // Prepare the statement
    char *sql = "select MOVE from OPENING_BOOK where POSITION_HASH = ? COLLATE NOCASE";
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK) {
        snprintf(hashStr, 17, "%016"PRIX64"", state->current->hash);
        sqlite3_bind_text(res, 1, hashStr, (int) strlen(hashStr), SQLITE_STATIC);
    } else {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // Read results, copying the raw strings to the str buffer.
    int step;
    do {
        step = sqlite3_step(res);
        if (step == SQLITE_ROW) {
            const char * moveStr =  (const char *) sqlite3_column_text(res, 0);
            strncpy((*strArray)[moveCount++], moveStr, BOOK_MAX_STR_LEN);
        }
    } while (step == SQLITE_ROW);

    sqlite3_finalize(res);

    return moveCount;
}

bool openBook(const char* fileName, OpenBook* book) {
    sqlite3* db;
    int rc;
    bool result = true;
    struct stat st;

    if (stat(fileName, &st) != 0 ) {
        result = false;
    } else {
        rc = sqlite3_open(fileName, &db);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            result = false;
        } else {
            book->database = db;
        }
    }

    return result;
}

bool closeBook(OpenBook* book) {
    return sqlite3_close(book->database) == SQLITE_OK;
}

int getMovesFromBook(GameState* gameState, MoveBuffer* buffer, OpenBook* book) {
    int strCount;
    int moveCount = 0;
    char** strArray;
    Move m;
    initializeStrArray(&strArray);

    // Get the move strings from the database
    strCount = readMoves(gameState, &strArray, book);

    // Parse the move strings and add the legal moves to the move buffer.
    for (int i = 0; i < strCount; i++) {
        if (matchMove(strArray[i], gameState, &m)) {
            buffer->moves[moveCount++] = m;
        }
    }

    buffer->length = moveCount;
    destroyStrArray(&strArray);
    return moveCount;
}

#undef BOOK_MAX_MOVES
#undef BOOK_MAX_STR_LEN
