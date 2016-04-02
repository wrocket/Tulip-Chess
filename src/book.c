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
#include <ctype.h>
#include <unistd.h>

#include "book.h"
#include "tulip.h"
#include "gamestate.h"
#include "move.h"
#include "notation.h"
#include "posix.h"
#include "hashconsts.h"

#define BOOK_MAX_MOVES 256
#define BOOK_MAX_STR_LEN 8

static void initializeStrArray(char*** array) {
	*array = ALLOC(BOOK_MAX_MOVES, sizeof(char*), *array, "Unable to allocate memory for book move string array.");
	for (int32_t i = 0; i < BOOK_MAX_MOVES; i++) {
		(*array)[i] = ALLOC(BOOK_MAX_STR_LEN, sizeof(char), (*array)[i], "Unable to allocate memory for book move string.");
	}
}

static void destroyStrArray(char*** array) {
	for (int32_t i = 0; i < BOOK_MAX_STR_LEN; i++) {
		free((*array)[i]);
	}
	free(*array);
}

static int readMoves(GameState* state, char*** strArray, OpenBook* book) {
	// Check if book exists.
	if (access(book->fileName, F_OK) != 0) {
		return 0;
	}

	const size_t sqlLen = 1024;
	char* sql = calloc(sqlLen, sizeof(char));
	if (!sql) {
		perror("Error allocating memory for SQL statement.");
		return 0;
	}

	const uint64_t positionHash = book_bookHash(state);

	snprintf(sql, sqlLen, "sqlite3 %s \"select MOVE from OPENING_BOOK where POSITION_HASH = '%016" PRIX64 "' COLLATE NOCASE\"", book->fileName, positionHash);

	const size_t lineLen = 16;
	char* line = calloc(lineLen, sizeof(char));
	if (!line) {
		perror("Error allocating memory for line buffer.");
		return 0;
	}

	int32_t count = 0;
	FILE *fp;
	fp = popen(sql, "r");
	if (fp) {
		while (fgets(line, (int) lineLen, fp) && count < BOOK_MAX_MOVES) {
			char* idx = line + strlen(line) - 1;
			while (idx > line && iscntrl(*idx)) {
				*idx-- = '\0';
			}

			strncpy((*strArray)[count++], line, lineLen);
		}

		pclose(fp);
	}

	return count;
}

bool book_open(const char* fileName, OpenBook* book) {
	book->fileName = fileName;
	return access(book->fileName, F_OK) == 0;
}

bool book_close(OpenBook* book) {
	return true;
}

int book_getMoves(GameState* gameState, MoveBuffer* buffer, OpenBook* book) {
	int32_t strCount;
	int32_t moveCount = 0;
	char** strArray;
	Move m;
	initializeStrArray(&strArray);

	// Get the move strings from the database
	strCount = readMoves(gameState, &strArray, book);

	// Parse the move strings and add the legal moves to the move buffer.
	for (int32_t i = 0; i < strCount; i++) {
		if (notation_matchMove(strArray[i], gameState, &m)) {
			buffer->moves[moveCount++] = m;
		}
	}

	buffer->length = moveCount;
	destroyStrArray(&strArray);
	return moveCount;
}

uint64_t book_bookHash(GameState* gameState) {
   uint64_t h = 0;

    if (gameState->current->toMove == COLOR_WHITE) {
        h ^= HASH_WHITE_TO_MOVE;
    }

    for (int32_t i = 0; i < 64; i++) {
        const int32_t sq = BOARD_SQUARES[i];
        const Piece* piece = gameState->board[sq];

        h ^= HASH_PIECE_SQ[sq][piece->ordinal];
    }

    return h;
}

#undef BOOK_MAX_MOVES
#undef BOOK_MAX_STR_LEN
