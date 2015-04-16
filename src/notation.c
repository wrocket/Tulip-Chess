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

#include "tulip.h"
#include "piece.h"
#include "notation.h"
#include "gamestate.h"
#include "attack.h"
#include "board.h"
#include "makemove.h"
#include "move.h"

int printShortAlg(Move* move, GameState* gameState, char* buffer) {
	int count = 0;
	bool isPawn = move->movingPiece == &WPAWN || move->movingPiece == &BPAWN;

	if (move->movingPiece == &WKING && move->from == SQ_E1) {
		if(move->to == SQ_G1) {
			count = sprintf(buffer, "O-O");
			goto add_check;
		} else if(move->to == SQ_C1) {
			count = sprintf(buffer, "O-O-O");
			goto add_check;
		}
	} else if (move->movingPiece == &BKING && move->from == SQ_E8) {
		if(move->to == SQ_G8) {
			count = sprintf(buffer, "O-O");
			goto add_check;
		} else if(move->to == SQ_C8) {
			count = sprintf(buffer, "O-O-O");
			goto add_check;
		}
	}

	const Piece* movingPiece = move->movingPiece;

	// TODO: Origin square disambiguation
	if (!isPawn) {
		buffer[count++] = toupper(movingPiece->name);
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
		// TODO: Checkmate
		buffer[count++] = '+';
	}
	unmakeMove(gameState, move);

	buffer[count++] = '\0';

	return count - 1;
}
