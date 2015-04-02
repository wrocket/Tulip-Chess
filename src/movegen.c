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

#include <stdio.h>

#include "tulip.h"
#include "move.h"
#include "gamestate.h"
#include "board.h"
#include "piece.h"

static void nonSlider(const int sq, const int offset, const Piece** board, Move* moveBuff, int* count, const int capturable) {
	const int target = sq + offset;
	const Piece* targetPiece = board[target];
	if(targetPiece == &EMPTY || targetPiece->color == capturable) {
		Move* m = &moveBuff[*count];
		m->to = target;
		m->from = sq;
		m->movingPiece = board[sq];
		m->captures = board[target];
		m->moveCode = NO_MOVE_CODE;
		(*count)++;
	}
}

static void slider(const int sq, const int offset, const Piece** board, Move* moveBuff, int* count, const int capturable) {
	int target = sq + offset;
	const Piece* targetPiece = board[target];
	const Piece* movingPiece = board[sq];
	while(targetPiece == &EMPTY) {
		Move* m = &moveBuff[*count];
		m->to = target;
		m->from = sq;
		m->movingPiece = movingPiece;
		m->captures = &EMPTY;
		m->moveCode = NO_MOVE_CODE;
		(*count)++;
		target += offset;
		targetPiece = board[target];
	}

	if(targetPiece->color == capturable) {
		Move* m = &moveBuff[*count];
		m->to = target;
		m->from = sq;
		m->movingPiece = movingPiece;
		m->captures = targetPiece;
		m->moveCode = NO_MOVE_CODE;
		(*count)++;
	}
}

static void whitePawn(const int sq, const Piece** board, Move* moveBuff, int* count) {
	if(board[sq + OFFSET_N] == &EMPTY) {
		Move* m = &moveBuff[*count];
		m->to = sq + OFFSET_N;
		m->from = sq;
		m->movingPiece = &WPAWN;
		m->captures = &EMPTY;
		m->moveCode = NO_MOVE_CODE;
		(*count)++;
	}

	if(RANK_IDX(sq) == RANK_2
		&& board[sq + (2 * OFFSET_N)] == &EMPTY) {
		Move* m = &moveBuff[*count];
		m->to = sq + (OFFSET_N * 2);
		m->from = sq;
		m->movingPiece = &WPAWN;
		m->captures = &EMPTY;
		m->moveCode = NO_MOVE_CODE;
		(*count)++;
	}

	if(board[sq + OFFSET_NE]->color == COLOR_BLACK) {
		Move* m = &moveBuff[*count];
		m->to = sq + OFFSET_NE;
		m->from = sq;
		m->movingPiece = &WPAWN;
		m->captures = board[sq + OFFSET_NE];
		m->moveCode = NO_MOVE_CODE;
		(*count)++;
	}

	if(board[sq + OFFSET_NW]->color == COLOR_BLACK) {
		Move* m = &moveBuff[*count];
		m->to = sq + OFFSET_NW;
		m->from = sq;
		m->movingPiece = &WPAWN;
		m->captures = board[sq + OFFSET_NE];
		m->moveCode = NO_MOVE_CODE;
		(*count)++;
	}
}

int generatePsuedoMoves(GameState* gs, MoveBuffer* moveBuff) {
	int count = 0;
	const Piece** board = gs->board;
	Move* moveArr = moveBuff->moves;

	if(gs->current->toMove == COLOR_BLACK) {
		fprintf(stderr, "Black move generation not yet implemented.");
		return 0;
	}

	for(int i=0; i<64; i++) {
		const int sq = BOARD_SQUARES[i];
		const Piece* p = board[sq];

		switch(p->ordinal) {
			case ORD_WPAWN:
				whitePawn(sq, board, moveArr, &count);
				break;
			case ORD_WKNIGHT:
				nonSlider(sq, OFFSET_KNIGHT_1, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_KNIGHT_2, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_KNIGHT_3, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_KNIGHT_4, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_KNIGHT_5, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_KNIGHT_6, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_KNIGHT_7, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_KNIGHT_8, board, moveArr, &count, COLOR_BLACK);
				break;
			case ORD_WBISHOP:
				slider(sq, OFFSET_NE, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_NW, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_SE, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_SW, board, moveArr, &count, COLOR_BLACK);
				break;
			case ORD_WROOK:
				slider(sq, OFFSET_N, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_S, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_E, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_W, board, moveArr, &count, COLOR_BLACK);
				break;
			case ORD_WQUEEN:
				slider(sq, OFFSET_N, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_S, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_E, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_W, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_NE, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_NW, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_SE, board, moveArr, &count, COLOR_BLACK);
				slider(sq, OFFSET_SW, board, moveArr, &count, COLOR_BLACK);
			case ORD_WKING:
				nonSlider(sq, OFFSET_N, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_S, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_E, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_W, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_NE, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_NW, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_SE, board, moveArr, &count, COLOR_BLACK);
				nonSlider(sq, OFFSET_SW, board, moveArr, &count, COLOR_BLACK);
				break;
		}

	}

	moveBuff->length = count;
	return count;
}
