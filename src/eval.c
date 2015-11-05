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
#include "board.h"
#include "gamestate.h"
#include "evalconsts.h"
#include "bitboard.h"
#include "piece.h"
#include "eval.h"

#define EVAL_SHARE_RANK_RQ_WHITE() onSameFileWithPowerPiece(state, sq, ORD_WROOK, ORD_WQUEEN)
#define EVAL_SHARE_RANK_RQ_BLACK() onSameFileWithPowerPiece(state, sq, ORD_BROOK, ORD_BQUEEN)

static inline int countBits(uint64_t n) {
    // Thank you, Brian Kernighan!
    int c;
    for (c = 0; n; c++) {
        n &= n - 1;
    }
    return c;
}

static inline int kingMovesBetweenSquares(int sq1, int sq2) {
    const int r1 = RANK_IDX(sq1);
    const int f1 = FILE_IDX(sq1);
    const int r2 = RANK_IDX(sq2);
    const int f2 = FILE_IDX(sq2);

    const int dr = abs(r1 - r2);
    const int df = abs(f1 - f2);

    const int min = dr < df ? dr : df;

    return min + abs(dr - df);
}

// Determine if the given square shares a rank with the given rook or queen.
static inline int onSameFileWithPowerPiece(GameState* gs, int sq, int ordinalRook, int ordinalQueen) {
    uint64_t* bb = gs->bitboards;
    const int file = FILE_IDX(sq);
    const uint64_t fileMask = BITS_FILES[file] & ~BITS_SQ[sq];
    const bool onRank = ((fileMask & bb[ordinalRook]) != 0) | ((fileMask & bb[ordinalQueen]) != 0);
    return onRank ? BONUS_RQ_SHARE_RANK : 0;
}

int classifyEndgame(GameState* g) {
    StateData* sd = g->current;
    int* counts = g->pieceCounts;

    if (sd->whitePieceCount + sd->blackPieceCount == 3) {
        if (counts[ORD_BROOK] || counts[ORD_BQUEEN]) return ENDGAME_BROOKvKING;
        if (counts[ORD_WROOK] || counts[ORD_WQUEEN]) return ENDGAME_WROOKvKING;
    }

    return ENDGAME_UNCLASSIFIED;
}

int evaluateOpening(GameState* state) {
    const Piece** board = state->board;
    int score = 0;
    uint64_t* bb = state->bitboards;

    uint64_t mobilityWhite = 0;
    uint64_t mobilityBlack = 0;

    for (int sq = SQ_A1; sq <= SQ_H8; sq++) {
        const Piece* p = board[sq];

        switch (p->ordinal) {
        case ORD_WPAWN:
            score += SCORE_PAWN;
            score += SQ_SCORE_PAWN_OPENING_WHITE[sq];
            if (board[sq + OFFSET_N] == &WPAWN) {
                score += PENALTY_DOUBLED_PAWN;
            }
            break;
        case ORD_BPAWN:
            score -= SCORE_PAWN;
            score -= SQ_SCORE_PAWN_OPENING_BLACK[sq];
            if (board[sq + OFFSET_S] == &BPAWN) {
                score -= PENALTY_DOUBLED_PAWN;
            }
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

static int evaluateMidgame(GameState* state) {
    const Piece** board = state->board;
    int score = 0;
    uint64_t* bb = state->bitboards;
    uint64_t mobilityWhite = 0;
    uint64_t mobilityBlack = 0;
    for (int sq = SQ_A1; sq <= SQ_H8; sq++) {
        const Piece* p = board[sq];

        switch (p->ordinal) {
        case ORD_WPAWN:
            score += SCORE_PAWN;
            break;
        case ORD_BPAWN:
            score -= SCORE_PAWN;
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

static int evaluateEndgame(GameState* state) {
    const Piece** board = state->board;
    int score = 0;
    StateData* sd = state->current;

    const int endgame = classifyEndgame(state);

    // If in a rook endgame, penalize having the kings far apart.
    // This is to encourage the player with the advantage to push the king up
    // into a normal checkmate position.
    if (endgame == ENDGAME_WROOKvKING) {
        score += KING_DISTANCE_PENALTY();
    } else if (endgame == ENDGAME_BROOKvKING) {
        score -= KING_DISTANCE_PENALTY();
    }

    for (int sq = SQ_A1; sq <= SQ_H8; sq++) {
        const Piece* p = board[sq];

        switch (p->ordinal) {
        case ORD_WPAWN:
            score += SCORE_PAWN;
            break;
        case ORD_BPAWN:
            score -= SCORE_PAWN;
            break;
        case ORD_WKNIGHT:
            score += SCORE_KNIGHT;
            break;
        case ORD_BKNIGHT:
            score -= SCORE_KNIGHT;
            break;
        case ORD_WBISHOP:
            score += SCORE_BISHOP;
            break;
        case ORD_BBISHOP:
            score -= SCORE_BISHOP;
            break;
        case ORD_WROOK:
            score += EVAL_SHARE_RANK_RQ_WHITE();
            score += SCORE_ROOK;
            break;
        case ORD_BROOK:
            score -= EVAL_SHARE_RANK_RQ_BLACK();
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

    return score;
}

#undef KING_DISTANCE_PENALTY

int evaluate(GameState* state) {
    StateData* current = state->current;

    int totalPieces = current->whitePieceCount + current->blackPieceCount;
    int result;

    // TODO: Ponder discarding pawns from this count.
    if (totalPieces > 20) {
        result = evaluateOpening(state);
    } else if (totalPieces > 10) {
        result = evaluateMidgame(state);
    } else {
        result = evaluateEndgame(state);
    }

    const int mult = state->current->toMove == COLOR_WHITE ? 1 : -1;

    return mult * result;
}

double friendlyScore(GameState* state, int rawScore) {
    const int multiplier = state->current->toMove == COLOR_WHITE ? 1 : -1;
    return (double) (rawScore * multiplier) / 100.0;
}

#undef EVAL_SHARE_RANK_RQ_WHITE
#undef EVAL_SHARE_RANK_RQ_BLACK
