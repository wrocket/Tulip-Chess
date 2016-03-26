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
#include <inttypes.h>

#include "tulip.h"
#include "board.h"
#include "gamestate.h"
#include "evalconsts.h"
#include "bitboard.h"
#include "piece.h"
#include "eval.h"

static inline int32_t countBits(uint64_t n) {
	// Thank you, Brian Kernighan!
	int32_t c;
	for (c = 0; n; c++) {
		n &= n - 1;
	}
	return c;
}

static inline int32_t kingMovesBetweenSquares(int32_t sq1, int32_t sq2) {
	const int32_t r1 = RANK_IDX(sq1);
	const int32_t f1 = FILE_IDX(sq1);
	const int32_t r2 = RANK_IDX(sq2);
	const int32_t f2 = FILE_IDX(sq2);

	const int32_t dr = abs(r1 - r2);
	const int32_t df = abs(f1 - f2);

	const int32_t min = dr < df ? dr : df;

	return min + abs(dr - df);
}

// Determine if the given square shares a file with the given rook or queen.
static inline int32_t onSameFileWithPowerPiece(GameState* gs, int32_t sq, int32_t ordinalRook, int32_t ordinalQueen) {
	uint64_t* bb = gs->bitboards;
	const int32_t file = FILE_IDX(sq);
	const uint64_t fileMask = BITS_FILES[file] & ~BITS_SQ[sq];
	const bool onRank = ((fileMask & bb[ordinalRook]) != 0) | ((fileMask & bb[ordinalQueen]) != 0);
	return onRank ? BONUS_RQ_SHARE_RANK : 0;
}

// The idea here is to penalize open long diagonals or files leading to the king.
// We'll count the open squares of diagonals and files leading "towards" the enemy side of the board.
#define COUNT_DIRECTION(offset) sp = sq + (offset); while(b[sp]->ordinal == ORD_EMPTY) {sp += (offset); count++;}
static inline int exposureWhite(const Piece** b, int32_t sq) {
	int32_t count = 0;
	int32_t sp;

	COUNT_DIRECTION(OFFSET_N);
	COUNT_DIRECTION(OFFSET_NE);
	COUNT_DIRECTION(OFFSET_NW);

	return count;
}

static inline int32_t exposureBlack(const Piece** b, int sq) {
	int32_t count = 0;
	int32_t sp;

	COUNT_DIRECTION(OFFSET_S);
	COUNT_DIRECTION(OFFSET_SE);
	COUNT_DIRECTION(OFFSET_SW);

	return count;
}

static inline int32_t bishopMobility(const Piece** b, int sq) {
	// TODO: Figure out some leet way to do this with bitboards.
	int32_t count = 0;
	int32_t sp;

	COUNT_DIRECTION(OFFSET_NE);
	COUNT_DIRECTION(OFFSET_SE);
	COUNT_DIRECTION(OFFSET_NW);
	COUNT_DIRECTION(OFFSET_SW);

	return count;
}
#undef COUNT_DIRECTION

int32_t countKingRectangleSize(int sq) {
	// TODO: Consider if calculating is faster than lookup.
	return KING_RECT_SIZES[sq];
}

int32_t classifyEndgame(GameState* g) {
	StateData* sd = g->current;
	int32_t* counts = g->pieceCounts;

	// Remove pawn counts from this calculation
	const int32_t wPieces = sd->whitePieceCount - counts[ORD_WPAWN];
	const int32_t bPieces = sd->blackPieceCount - counts[ORD_BPAWN];

	if (wPieces + bPieces == 3) {
		// This is the one situation where I don't put braces on my ifs.
		if (counts[ORD_BROOK] || counts[ORD_BQUEEN]) return ENDGAME_BROOKvKING;
		if (counts[ORD_WROOK] || counts[ORD_WQUEEN]) return ENDGAME_WROOKvKING;
	}

	return ENDGAME_UNCLASSIFIED;
}

static inline int32_t basicWPawnBonus(const int32_t sq, const Piece** board, uint64_t* bb) {
	int32_t score = SCORE_PAWN;
	score += SQ_SCORE_PAWN_OPENING_WHITE[sq];
	if (board[sq + OFFSET_N] == &WPAWN) {
		score += PENALTY_DOUBLED_PAWN;
	}

	// If the squares to the SE or SW are also white pawns, add a bonus.
	const uint64_t defendingPawns = bb[ORD_WPAWN] & BITS_WPAWN[sq];
	if (defendingPawns) {
		score += PAWN_CHAIN_BONUS;
	}
	return score;
}

static inline int32_t basicBPawnBonus(const int32_t sq, const Piece** board, uint64_t* bb) {
	int32_t score = SCORE_PAWN;
	score += SQ_SCORE_PAWN_OPENING_BLACK[sq];
	if (board[sq + OFFSET_S] == &BPAWN) {
		score += PENALTY_DOUBLED_PAWN;
	}

	// If the squares to the NE or NW are also black pawns, add a bonus.
	const uint64_t defendingPawns = bb[ORD_BPAWN] & BITS_BPAWN[sq];
	if (defendingPawns) {
		score += PAWN_CHAIN_BONUS;
	}

	return score;
}

int32_t evaluateOpening(GameState* state) {
	const Piece** board = state->board;
	int32_t score = 0;
	uint64_t* bb = state->bitboards;

	uint64_t mobilityWhite = 0;
	uint64_t mobilityBlack = 0;

	for (int32_t sq = SQ_A1; sq <= SQ_H8; sq++) {
		const Piece* p = board[sq];

		switch (p->ordinal) {
		case ORD_WPAWN:
			score += basicWPawnBonus(sq, board, bb);
			break;
		case ORD_BPAWN:
			score -= basicBPawnBonus(sq, board, bb);
			break;
		case ORD_WKNIGHT:
			score += SCORE_KNIGHT;
			score += SQ_SCORE_KNIGHT_OPENING_WHITE[sq];
			mobilityWhite |= BITS_KNIGHT[sq];
			break;
		case ORD_BKNIGHT:
			score -= SCORE_KNIGHT;
			score -= SQ_SCORE_KNIGHT_OPENING_BLACK[sq];
			mobilityBlack |= BITS_KNIGHT[sq];
			break;
		case ORD_WBISHOP:
			score += SCORE_BISHOP;
			score += bishopMobility(board, sq) * MINOR_PIECE_MOBILITY_BONUS;
			break;
		case ORD_BBISHOP:
			score -= SCORE_BISHOP;
			score -= bishopMobility(board, sq) * MINOR_PIECE_MOBILITY_BONUS;
			break;
		case ORD_WROOK:
			score += SCORE_ROOK;
			break;
		case ORD_BROOK:
			score -= SCORE_ROOK;
			break;
		case ORD_WQUEEN:
			score += SCORE_QUEEN;
			score += SQ_SCORE_QUEEN_OPENING_WHITE[sq];
			break;
		case ORD_BQUEEN:
			score -= SCORE_QUEEN;
			score -= SQ_SCORE_QUEEN_OPENING_BLACK[sq];
			break;
		case ORD_WKING:
			score += KING_EXPOSURE * exposureWhite(board, sq);
			break;
		case ORD_BKING:
			score -= KING_EXPOSURE * exposureBlack(board, sq);
			break;
		default:
			break;
		}
	}

	// Remove any mobility score that refers to a square covered by a friendly pawn.
	mobilityWhite &= ~bb[ORD_WPAWN];
	mobilityBlack &= ~bb[ORD_BPAWN];

	score += MINOR_PIECE_MOBILITY_BONUS * (countBits(mobilityWhite) - countBits(mobilityBlack));

	return score;
}

static int32_t evaluateMidgame(GameState* state) {
	const Piece** board = state->board;
	int32_t score = 0;
	uint64_t* bb = state->bitboards;
	uint64_t mobilityWhite = 0;
	uint64_t mobilityBlack = 0;
	for (int32_t sq = SQ_A1; sq <= SQ_H8; sq++) {
		const Piece* p = board[sq];

		switch (p->ordinal) {
		case ORD_WPAWN:
			score += basicWPawnBonus(sq, board, bb);
			break;
		case ORD_BPAWN:
			score -= basicBPawnBonus(sq, board, bb);
			break;
		case ORD_WKNIGHT:
			score += SCORE_KNIGHT;
			mobilityWhite |= BITS_KNIGHT[sq];
			break;
		case ORD_BKNIGHT:
			score -= SCORE_KNIGHT;
			mobilityBlack |= BITS_KNIGHT[sq];
			break;
		case ORD_WBISHOP:
			score += SCORE_BISHOP;
			break;
		case ORD_BBISHOP:
			score -= SCORE_BISHOP;
			break;
		case ORD_WROOK:
			score += SCORE_ROOK;
			break;
		case ORD_BROOK:
			score -= SCORE_ROOK;
			break;
		case ORD_WQUEEN:
			score += SCORE_QUEEN;
			break;
		case ORD_BQUEEN:
			score -= SCORE_QUEEN;
			break;
		default:
			break;
		}
	}

	// Remove any mobility score that refers to a square covered by a friendly pawn.
	mobilityWhite &= ~bb[ORD_WPAWN];
	mobilityBlack &= ~bb[ORD_BPAWN];

	score += MINOR_PIECE_MOBILITY_BONUS * (countBits(mobilityWhite) - countBits(mobilityBlack));

	return score;
}

#define KING_DISTANCE_PENALTY() (kingMovesBetweenSquares(sd->whiteKingSquare, sd->blackKingSquare)) * KING_ENDGAME_DISTANCE_PENALTY

static int32_t evaluateEndgame(GameState* state) {
	const Piece** board = state->board;
	int32_t score = 0;
	uint64_t* bb = state->bitboards;
	StateData* sd = state->current;
	uint64_t mobilityWhite = 0;
	uint64_t mobilityBlack = 0;

	const int endgame = classifyEndgame(state);

	// If in a rook endgame, penalize having the kings far apart.
	// This is to encourage the player with the advantage to push the king up
	// into a normal checkmate position.
	// Also penalize the player with the advantage for leaving the opposing king near the center of the board.
	if (endgame == ENDGAME_WROOKvKING) {
		score += KING_DISTANCE_PENALTY();
		score += KING_ENDGAME_RECTANGLE_PENALTY * countKingRectangleSize(sd->blackKingSquare);
	} else if (endgame == ENDGAME_BROOKvKING) {
		score -= KING_DISTANCE_PENALTY();
		score -= KING_ENDGAME_RECTANGLE_PENALTY * countKingRectangleSize(sd->whiteKingSquare);
	}

	for (int32_t sq = SQ_A1; sq <= SQ_H8; sq++) {
		const Piece* p = board[sq];

		switch (p->ordinal) {
		case ORD_WPAWN:
			score += SCORE_PAWN;
			if ((BITS_PASSED_PAWN_W[sq] & state->bitboards[ORD_BPAWN]) == 0) {
				score += SCORE_PASSED_PAWN;
			}
			break;
		case ORD_BPAWN:
			score -= SCORE_PAWN;
			if ((BITS_PASSED_PAWN_B[sq] & state->bitboards[ORD_WPAWN]) == 0) {
				score -= SCORE_PASSED_PAWN;
			}
			break;
		case ORD_WKNIGHT:
			score += SCORE_KNIGHT;
			mobilityWhite |= BITS_KNIGHT[sq];
			break;
		case ORD_BKNIGHT:
			score -= SCORE_KNIGHT;
			mobilityBlack |= BITS_KNIGHT[sq];
			break;
		case ORD_WBISHOP:
			score += SCORE_BISHOP;
			break;
		case ORD_BBISHOP:
			score -= SCORE_BISHOP;
			break;
		case ORD_WROOK:
			score += onSameFileWithPowerPiece(state, sq, ORD_WROOK, ORD_WQUEEN);
			score += SCORE_ROOK;
			break;
		case ORD_BROOK:
			score -= onSameFileWithPowerPiece(state, sq, ORD_BROOK, ORD_BQUEEN);
			score -= SCORE_ROOK;
			break;
		case ORD_WQUEEN:
			score += SCORE_QUEEN;
			break;
		case ORD_BQUEEN:
			score -= SCORE_QUEEN;
			break;
		case ORD_WKING:
			score += SQ_SCORE_ENDGAME_KING[sq];
			break;
		case ORD_BKING:
			score -= SQ_SCORE_ENDGAME_KING[sq];
			break;
		default:
			break;
		}
	}

	mobilityWhite &= ~bb[ORD_WPAWN];
	mobilityBlack &= ~bb[ORD_BPAWN];

	score += MINOR_PIECE_MOBILITY_BONUS * (countBits(mobilityWhite) - countBits(mobilityBlack));

	return score;
}

#undef KING_DISTANCE_PENALTY

int32_t evaluate(GameState* state) {
	StateData* current = state->current;

	const int32_t totalPieces = current->whitePieceCount
	                            + current->blackPieceCount
	                            - state->pieceCounts[ORD_WPAWN]
	                            - state->pieceCounts[ORD_BPAWN];

	int32_t result;

	if (totalPieces >= 12) {
		result = evaluateOpening(state);
	} else if (totalPieces >= 6) {
		result = evaluateMidgame(state);
	} else {
		result = evaluateEndgame(state);
	}

	const int32_t mult = state->current->toMove == COLOR_WHITE ? 1 : -1;

	return mult * result;
}

double friendlyScore(GameState* state, int32_t rawScore) {
	const int32_t multiplier = state->current->toMove == COLOR_WHITE ? 1 : -1;
	return (double) (rawScore * multiplier) / 100.0;
}
