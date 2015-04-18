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

#include <ctype.h>
#include <stdio.h>

#include "attack.h"
#include "board.h"
#include "gamestate.h"
#include "makemove.h"
#include "move.h"
#include "movegen.h"
#include "notation.h"
#include "piece.h"
#include "tulip.h"

static bool doMoveCollide(Move* legalMove, Move* candidate) {
    return legalMove->from != candidate->from
        && legalMove->movingPiece == candidate->movingPiece
        && legalMove->to == candidate->to;
}

static int printMoveDisambiguation(GameState* g, Move* move, char* buffer) {
	int c = 0;
	MoveBuffer legalMoves;
	int* collidingMoves;
	int collidingMoveCnt = 0;

	createMoveBuffer(&legalMoves);
	generateLegalMoves(g, &legalMoves);

	collidingMoves = ALLOC(legalMoves.length, int, collidingMoves, "Unable to allocate ambiguous move buffer.");

	for (int i=0; i < legalMoves.length; i++) {
		Move* candidate = &legalMoves.moves[i];
		if (doMoveCollide(move, candidate)) {
			collidingMoves[collidingMoveCnt++] = candidate->from;
		}
	}

	if (collidingMoveCnt > 0) {
		bool sameRank = false;
		bool sameFile = false;

		int fromRank = RANK_IDX(move->from);
		int fromFile = FILE_IDX(move->from);

		for (int i = 0; i < collidingMoveCnt; i++) {
			int ambigSq = collidingMoves[i];
			sameRank |= fromRank == RANK_IDX(ambigSq);
			sameFile |= fromFile == FILE_IDX(ambigSq);
		}

		if (sameFile && sameRank) {
			c = printSquareIndex(move->from, buffer);
		} else if (sameRank || !sameFile) {
			buffer[c++] = fileToChar(fromFile);
		} else if(sameFile) {
			buffer[c++] = rankToChar(fromRank);
		}
	}

	free(collidingMoves);
	destroyMoveBuffer(&legalMoves);

	return c;
}

int printShortAlg(Move* move, GameState* gameState, char* buffer) {
	int count = 0;
	const Piece* movingPiece = move->movingPiece;
	bool isPawn = movingPiece == &WPAWN || movingPiece == &BPAWN;

	if (movingPiece == &WKING || movingPiece == &BKING) {
		int moveOffset = move->from - move->to;
		if (moveOffset == 2) {
			count = sprintf(buffer, "O-O-O");
			goto add_check;
		} else if (moveOffset == -2) {
			count = sprintf(buffer, "O-O");
			goto add_check;
		}
	}

	if (!isPawn) {
		buffer[count++] = toupper(movingPiece->name);
		count += printMoveDisambiguation(gameState, move, &buffer[count]);
	}

	if (move->captures != &EMPTY) {
		if (isPawn) {
			buffer[count++] = indexToFileChar(move->from);
		}

		buffer[count++] = 'x';
	}

	count += printSquareIndex(move->to, buffer + count);

	if (IS_PROMOTE(move->moveCode)) {
		buffer[count++] = '=';
		buffer[count++] = toupper(getPromotePiece(movingPiece->color, move->moveCode)->name);
	}

add_check:

	makeMove(gameState, move);
	if (isCheck(gameState)) {
		buffer[count++] = countLegalMoves(gameState) == 0 ? '#' : '+';
	}

	unmakeMove(gameState, move);

	buffer[count] = '\0';
	return count;
}
