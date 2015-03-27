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

#include "tulip.h"
#include "piece.h"
#include "stateData.h"
#include "board.h"

void createStateData(StateData * data) {
	data->board = ALLOC(144, Piece*, data->board, "Error allocating board array.");
	data->pieceCounts = ALLOC_ZERO(ORD_MAX, int, data->pieceCounts, "Error allocating piece count array.");

	for(int i=0; i<144; i++) {
		data->board[i] = &OFF_BOARD;
	}

	for(int file = FILE_A; file <= FILE_H; file++) {
		for(int rank = RANK_1; rank <= RANK_8; rank++) {
			data->board[B_IDX(file, rank)] = &EMPTY;
		}
	}

	data->toMove = COLOR_WHITE;
	data->whiteKingSquare = 0;
	data->blackKingSquare = 0;
	data->castleFlags = 0;
	data->hash = 0;
	data->epFile = 0;
}

void copyStateData(StateData* from, StateData* to) {
	memcpy(to->board, from->board, 144 * sizeof(Piece*));
	memcpy(to->pieceCounts, from->pieceCounts, ORD_MAX * sizeof(int));
	to->toMove = from->toMove;
	to->whiteKingSquare = from->whiteKingSquare;
	to->castleFlags = from->castleFlags;
	to->blackKingSquare = from->blackKingSquare;
	to->hash = from->hash;
	to->epFile = from->epFile;
}

void destroyStateData(StateData * data) {
	free(data->board);
}
