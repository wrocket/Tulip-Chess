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

#include "tulip.h"
#include "board.h"
#include "gamestate.h"
#include "evalconsts.h"
#include "bitboard.h"
#include "piece.h"

#define EVAL_SHARE_RANK_RQ_WHITE() onSameFileWithPowerPiece(state, sq, ORD_WROOK, ORD_WQUEEN)
#define EVAL_SHARE_RANK_RQ_BLACK() onSameFileWithPowerPiece(state, sq, ORD_BROOK, ORD_BQUEEN)

// Determine if the given square shares a rank with the given rook or queen.
static inline int onSameFileWithPowerPiece(GameState* gs, int sq, int ordinalRook, int ordinalQueen) {
    uint64_t* bb = gs->bitboards;
    const int file = FILE_IDX(sq);
    const uint64_t fileMask = BITS_FILES[file] & ~BITS_SQ[sq];
    const bool onRank = (fileMask & bb[ordinalRook]) != 0 | (fileMask & bb[ordinalQueen]) != 0;
    return onRank ? BONUS_RQ_SHARE_RANK : 0;
}

static int evaluateOpening(GameState* state) {
    const Piece** board = state->board;
    int score = 0;
    for (int sq = SQ_A1; sq <= SQ_H8; sq++) {
        const Piece* p = board[sq];

        switch (p->ordinal) {
        case ORD_WPAWN:
            score += SCORE_PAWN;
            score += SQ_SCORE_PAWN_OPENING_WHITE[sq];
            break;
        case ORD_BPAWN:
            score -= SCORE_PAWN;
            score -= SQ_SCORE_PAWN_OPENING_BLACK[sq];
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

    return score;
}

static int evaluateMidgame(GameState* state) {
    const Piece** board = state->board;
    int score = 0;
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

    return score;
}

static int evaluateEndgame(GameState* state) {
    const Piece** board = state->board;
    int score = 0;
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
        default:
            break;
        }
    }

    return score;
}

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

    return result;
}

#undef EVAL_SHARE_RANK_RQ_WHITE
#undef EVAL_SHARE_RANK_RQ_BLACK
