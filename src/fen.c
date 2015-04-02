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

#include "tulip.h"
#include "string.h"
#include "board.h"
#include "piece.h"
#include "gamestate.h"
#include "statedata.h"
#include "piece.h"
#include "util.h"

#define _FEN_MAX_TOKENS		16
#define _FEN_MAX_TOKEN_LEN	64

static int parseToMove(char * t, int* result) {
	if(strlen(t) != 1) {
		fprintf(stderr, "Invalid to-move string: %s\n", t);
		return false;
	}

	if(!strstr("bw", t)) {
		fprintf(stderr, "Invalid to-move string: %s\n", t);
		return false;
	}

	*result = (t[0] == 'b' ? COLOR_BLACK : COLOR_WHITE);
	return true;
}

static int parseEpFile(char* t, int* result) {
	int len = strlen(t);
	bool valid = true;

	if(len == 1 && strcmp("-", t) == 0) {
		*result = NO_EP_FILE;
	} else if(len == 2 && (t[1] == '3' || t[1] == '6')) {
		const int file = parseFileChar(t[0]);
		if(file != INVALID_FILE) {
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

static int parseCastleFlags(char* t, int* result) {
	int flags = 0;

	if(strstr(t, "K")) {
		flags |= CASTLE_WK;
	}

	if(strstr(t, "Q")) {
		flags |= CASTLE_WQ;
	}

	if(strstr(t, "k")) {
		flags |= CASTLE_BK;
	}

	if(strstr(t, "q")) {
		flags |= CASTLE_BQ;
	}

	*result = flags;
	return true;
}

static int parseBoard(char* t, GameState* gs) {
	const Piece** board = gs->board;

	bool result = true;
	int currentFile = FILE_A;
	int currentRank = RANK_8;
	int squares = 0;
	int sqwk = -1;
	int sqbk = -1;


	char* c = t;
	while(*c && currentRank >= RANK_1) {
		if(*c == '/') {
			currentRank--;
			currentFile = FILE_A;
		} else if(isdigit(*c)) {
			int spaceDigits = *c - '0'; // Hack: Char to int. Might be funny in strange encodings...
			for(int i=0; i<spaceDigits; i++) {
				board[B_IDX(currentFile++, currentRank)] = &EMPTY;
				gs->pieceCounts[(&EMPTY)->ordinal]++;
				squares++;
			}
		} else {
			const Piece* p = parsePiece(*c);
			if(!p) {
				fprintf(stderr, "Invalid FEN. Unknown piece found: %c\n", *c);
				result = false;
				goto parse_board_err;
			}

			const int idx = B_IDX(currentFile++, currentRank);

			if(p == &WKING) {
				if(sqwk >= 0) {
					fprintf(stderr, "Invalid FEN. Multiple white kings on the board.\n");
					result = false;
					goto parse_board_err;
				}

				sqwk = idx;
			} else if(p == &BKING) {
				if(sqbk >= 0) {
					fprintf(stderr, "Invalid FEN. Multiple black kings on the board.\n");
					result = false;
					goto parse_board_err;
				}

				sqbk = idx;
			}

			gs->pieceCounts[p->ordinal]++;

			board[idx] = p;
			squares++;
		}

		++c;
	}

	if(sqbk < 0) {
		fprintf(stderr, "Invalid FEN. No black king on the board.\n");
		result = false;
	}

	if(sqwk < 0) {
		fprintf(stderr, "Invalid FEN. No white king on the board.\n");
		result = false;
	}

	if(squares != 64) {
		fprintf(stderr, "Invalid FEN. Expected to parse 64 squares, instead parsed %d.\n", squares);
		result = false;
	}

	gs->current->whiteKingSquare = sqwk;
	gs->current->blackKingSquare = sqbk;

parse_board_err:
	return result;
}

int parseFen(GameState* state, char* fenStr) {
	bool result = true;
	char** tokenBuffer;
	int toMove = COLOR_WHITE;
	int castleFlags = 0;
	int epFile = NO_EP_FILE;

	tokenBuffer = createTokenBuffer(_FEN_MAX_TOKENS, _FEN_MAX_TOKEN_LEN);

	int tokenCount = tokenize(fenStr, tokenBuffer, _FEN_MAX_TOKENS);

	if(tokenCount != 6) {
		fprintf(stderr, "Invalid FEN string; invalid token count (should be 6): %i.", tokenCount);
		result = false;
		goto clean_tokens;
	}

	if(!parseBoard(tokenBuffer[0], state)) {
		result = false;
		goto clean_tokens;
	}

	if(!parseToMove(tokenBuffer[1], &toMove)) {
		result = false;
		goto clean_tokens;
	}

	if(!parseCastleFlags(tokenBuffer[2], &castleFlags)) {
		result = false;
		goto clean_tokens;
	}

	if(!parseEpFile(tokenBuffer[3], &epFile)) {
		result = false;
		goto clean_tokens;
	}

	state->current->castleFlags = castleFlags;
	state->current->toMove = toMove;
	state->current->epFile = epFile;

	reinitBitboards(state);

clean_tokens:
	freeTokenBuffer(tokenBuffer, _FEN_MAX_TOKENS);

	return result;
}
