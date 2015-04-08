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
#include <stdlib.h>

#include "tulip.h"
#include "move.h"
#include "gamestate.h"
#include "board.h"
#include "piece.h"
#include "attack.h"

#define PUSH_MOVE(move, fromSq, toSq, moving, capturesP, code)  \
    (m)->from=(fromSq); (m)->to=(toSq); (m)->movingPiece=(moving); (m)->captures=(capturesP); (m)->moveCode=(code);

static void nonSlider(const int sq, const int offset, const Piece** board, Move* moveBuff, int* count, const int capturable) {
    const int target = sq + offset;
    const Piece* targetPiece = board[target];
    if (targetPiece == &EMPTY || targetPiece->color == capturable) {
        Move* m = &moveBuff[*count];
        PUSH_MOVE(m, sq, target, board[sq], board[target], NO_MOVE_CODE);
        (*count)++;
    }
}

static void slider(const int sq, const int offset, const Piece** board, Move* moveBuff, int* count, const int capturable) {
    int target = sq + offset;
    const Piece* targetPiece = board[target];
    const Piece* movingPiece = board[sq];
    Move* m = &moveBuff[*count];
    while (targetPiece == &EMPTY) {
        PUSH_MOVE(m, sq, target, movingPiece, &EMPTY, NO_MOVE_CODE);
        (*count)++;
        m++;
        target += offset;
        targetPiece = board[target];
    }

    if (targetPiece->color == capturable) {
        PUSH_MOVE(m, sq, target, movingPiece, targetPiece, NO_MOVE_CODE);
        (*count)++;
    }
}

static void bishop(const int sq, const Piece** board, Move* moveArr, int* count, const int capturable) {
    slider(sq, OFFSET_NE, board, moveArr, count, capturable);
    slider(sq, OFFSET_NW, board, moveArr, count, capturable);
    slider(sq, OFFSET_SE, board, moveArr, count, capturable);
    slider(sq, OFFSET_SW, board, moveArr, count, capturable);
}

static void rook(const int sq, const Piece** board, Move* moveArr, int* count, const int capturable) {
    slider(sq, OFFSET_N, board, moveArr, count, capturable);
    slider(sq, OFFSET_W, board, moveArr, count, capturable);
    slider(sq, OFFSET_E, board, moveArr, count, capturable);
    slider(sq, OFFSET_S, board, moveArr, count, capturable);
}

static void knight(const int sq, const Piece** board, Move* moveArr, int* count, const int capturable) {
    nonSlider(sq, OFFSET_KNIGHT_1, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_KNIGHT_2, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_KNIGHT_3, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_KNIGHT_4, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_KNIGHT_5, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_KNIGHT_6, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_KNIGHT_7, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_KNIGHT_8, board, moveArr, count, capturable);
}

static void king(const int sq, const Piece** board, Move* moveArr, int* count, const int capturable) {
    nonSlider(sq, OFFSET_N, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_S, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_E, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_W, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_NE, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_NW, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_SE, board, moveArr, count, capturable);
    nonSlider(sq, OFFSET_SW, board, moveArr, count, capturable);
}

static void whitePawnEp(const int sq, const Piece** board, Move* moveBuffer, int* count, int epFile) {
    const int dF = epFile - FILE_IDX(sq);
    if ((dF == 1 || dF == -1) && RANK_IDX(sq) == RANK_5) {
        Move* m = &moveBuffer[*count];
        PUSH_MOVE(m, sq, sq + (dF * OFFSET_E) + OFFSET_N, &WPAWN, &BPAWN, CAPTURE_EP);
        (*count)++;
    }
}

// TODO: Condense whitePawnEp and blackPawnEp in to a single method.
static void blackPawnEp(const int sq, const Piece** board, Move* moveBuffer, int* count, int epFile) {
    const int dF = epFile - FILE_IDX(sq);
    if ((dF == 1 || dF == -1) && RANK_IDX(sq) == RANK_4) {
        Move* m = &moveBuffer[*count];
        PUSH_MOVE(m, sq, sq + (dF * OFFSET_E) + OFFSET_S, &BPAWN, &WPAWN, CAPTURE_EP);
        (*count)++;
    }
}

static void pawnPromote(Move* moveBuff, const int from, const int to, const Piece* captures, int* count, const Piece* pawn) {
        Move* m = &moveBuff[*count];

        PUSH_MOVE(m, from, to, pawn, captures, PROMOTE_Q);
        m++;

        PUSH_MOVE(m, from, to, pawn, captures, PROMOTE_N);
        m++;

        PUSH_MOVE(m, from, to, pawn, captures, PROMOTE_R);
        m++;

        PUSH_MOVE(m, from, to, pawn, captures, PROMOTE_B);

        (*count) += 4;
}

static void whitePawnPromote(const int sq, const Piece** board, Move* moveBuff, int* count) {
    int target = sq + OFFSET_N;
    if (board[target] == &EMPTY) {
        pawnPromote(moveBuff, sq, target, &EMPTY, count, &WPAWN);
    }

    target = sq + OFFSET_NE;
    if (board[target]->color == COLOR_BLACK) {
        pawnPromote(moveBuff, sq, target, board[target], count, &WPAWN);
    }

    target = sq + OFFSET_NW;
    if (board[target]->color == COLOR_BLACK) {
        pawnPromote(moveBuff, sq, target, board[target], count, &WPAWN);
    }
}

static void blackPawnPromote(const int sq, const Piece** board, Move* moveBuff, int* count) {
    int target = sq + OFFSET_S;
    if (board[target] == &EMPTY) {
        pawnPromote(moveBuff, sq, target, &EMPTY, count, &BPAWN);
    }

    target = sq + OFFSET_SE;
    if (board[target]->color == COLOR_WHITE) {
        pawnPromote(moveBuff, sq, target, board[target], count, &BPAWN);
    }

    target = sq + OFFSET_SW;
    if (board[target]->color == COLOR_WHITE) {
        pawnPromote(moveBuff, sq, target, board[target], count, &BPAWN);
    }
}

static void whitePawn(const int sq, const Piece** board, Move* moveBuff, int* count) {
    Move* m = &moveBuff[*count];
    int target;

    target = sq + OFFSET_N;
    if (board[target] == &EMPTY) {
        PUSH_MOVE(m, sq, target, &WPAWN, &EMPTY, NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    if (sq <= SQ_H2
        && board[sq + (2 * OFFSET_N)] == &EMPTY) {
        PUSH_MOVE(m, sq, sq + (OFFSET_N * 2), &WPAWN, &EMPTY, NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    target = sq + OFFSET_NE;
    if (board[target]->color == COLOR_BLACK) {
        PUSH_MOVE(m, sq, target, &WPAWN, board[target], NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    target = sq + OFFSET_NW;
    if (board[target]->color == COLOR_BLACK) {
        PUSH_MOVE(m, sq, target, &WPAWN, board[target], NO_MOVE_CODE);
        (*count)++;
        m++;
    }
}

// TODO: Condense whitePawn() and backPawn() in to a single method...
static void blackPawn(const int sq, const Piece** board, Move* moveBuff, int* count) {
    Move* m = &moveBuff[*count];
    int target;

    target = sq + OFFSET_S;
    if (board[target] == &EMPTY) {
        PUSH_MOVE(m, sq, target, &BPAWN, &EMPTY, NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    if (sq >= SQ_A7
        && board[sq + (2 * OFFSET_S)] == &EMPTY) {
        PUSH_MOVE(m, sq, sq + (2 * OFFSET_S), &BPAWN, &EMPTY, NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    target = sq + OFFSET_SE;
    if (board[sq + OFFSET_SE]->color == COLOR_WHITE) {
        PUSH_MOVE(m, sq, target, &BPAWN, board[target], NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    target = sq + OFFSET_SW;
    if (board[target]->color == COLOR_WHITE) {
        PUSH_MOVE(m, sq, target, &BPAWN, board[target], NO_MOVE_CODE);
        (*count)++;
        m++;
    }
}

static void blackKingCastle(GameState* gs, const Piece** board, Move* moveArr, int* count) {
    const int castleFlags = gs->current->castleFlags;
    Move* m = &moveArr[*count];

    if ((castleFlags & CASTLE_BK)
        && board[SQ_F8] == &EMPTY
        && board[SQ_G8] == &EMPTY
        && !canAttack(COLOR_WHITE, SQ_F8, gs)) {
        PUSH_MOVE(m, SQ_E8, SQ_G8, &BKING, &EMPTY, NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    if ((castleFlags & CASTLE_BQ)
        && board[SQ_D8] == &EMPTY
        && board[SQ_C8] == &EMPTY
        && board[SQ_B8] == &EMPTY
        && !canAttack(COLOR_WHITE, SQ_D8, gs)) {
        PUSH_MOVE(m, SQ_E8, SQ_C8, &BKING, &EMPTY, NO_MOVE_CODE);
        (*count)++;
    }   
}

static void whiteKingCastle(GameState* gs, const Piece** board, Move* moveArr, int* count) {
    const int castleFlags = gs->current->castleFlags;
    Move* m = &moveArr[*count];

    if ((castleFlags & CASTLE_WK)
        && board[SQ_F1] == &EMPTY
        && board[SQ_G1] == &EMPTY
        && !canAttack(COLOR_BLACK, SQ_F1, gs)) {
        PUSH_MOVE(m, SQ_E1, SQ_G1, &WKING, &EMPTY, NO_MOVE_CODE);
        (*count)++;
        m++;
    }

    if ((castleFlags & CASTLE_WQ)
        && board[SQ_D1] == &EMPTY
        && board[SQ_C1] == &EMPTY
        && board[SQ_B1] == &EMPTY
        && !canAttack(COLOR_BLACK, SQ_D1, gs)) {
        PUSH_MOVE(m, SQ_E1, SQ_C1, &WKING, &EMPTY, NO_MOVE_CODE);
        (*count)++;
    }
}

int generatePseudoMovesBlack(GameState* gs, MoveBuffer* moveBuff) {
    int count = 0;
    const Piece** board = gs->board;
    const int epFile = gs->current->epFile;
    Move* moveArr = moveBuff->moves;

    for(int i=0; i<64; i++) {
        const int sq = BOARD_SQUARES[i];
        const Piece* p = board[sq];

        switch(p->ordinal) {
            case ORD_BPAWN:
                if (sq <= SQ_H2) {
                    blackPawnPromote(sq, board, moveArr, &count);
                } else {
                    blackPawn(sq, board, moveArr, &count);
                    if (epFile != NO_EP_FILE) {
                        blackPawnEp(sq, board, moveArr, &count, epFile);
                    }
                }
                break;
            case ORD_BKNIGHT:
                knight(sq, board, moveArr, &count, COLOR_WHITE);
                break;
            case ORD_BBISHOP:
                bishop(sq, board, moveArr, &count, COLOR_WHITE);
                break;
            case ORD_BROOK:
                rook(sq, board, moveArr, &count, COLOR_WHITE);
                break;
            case ORD_BQUEEN:
                bishop(sq, board, moveArr, &count, COLOR_WHITE);
                rook(sq, board, moveArr, &count, COLOR_WHITE);
                break;
            case ORD_BKING:
                king(sq, board, moveArr, &count, COLOR_WHITE);
                if ((gs->current->castleFlags & (CASTLE_BK | CASTLE_BQ)) 
                        && !canAttack(COLOR_WHITE, SQ_E8, gs)) {
                    blackKingCastle(gs, board, moveArr, &count);
                }
                break;
        }
    }

    moveBuff->length = count;
    return count;
}

int generatePseudoMovesWhite(GameState* gs, MoveBuffer* moveBuff) {
    int count = 0;
    const Piece** board = gs->board;
    const int epFile = gs->current->epFile;
    Move* moveArr = moveBuff->moves;

    for(int i=0; i<64; i++) {
        const int sq = BOARD_SQUARES[i];
        const Piece* p = board[sq];

        switch(p->ordinal) {
            case ORD_WPAWN:
                if (sq >= SQ_A7) {
                    whitePawnPromote(sq, board, moveArr, &count);
                } else {
                    whitePawn(sq, board, moveArr, &count);
                    if (epFile != NO_EP_FILE) {
                        whitePawnEp(sq, board, moveArr, &count, epFile);
                    }
                }
                break;
            case ORD_WKNIGHT:
                knight(sq, board, moveArr, &count, COLOR_BLACK);
                break;
            case ORD_WBISHOP:
                bishop(sq, board, moveArr, &count, COLOR_BLACK);
                break;
            case ORD_WROOK:
                rook(sq, board, moveArr, &count, COLOR_BLACK);
                break;
            case ORD_WQUEEN:
                bishop(sq, board, moveArr, &count, COLOR_BLACK);
                rook(sq, board, moveArr, &count, COLOR_BLACK);
                break;
            case ORD_WKING:
                king(sq, board, moveArr, &count, COLOR_BLACK);
                if ((gs->current->castleFlags & (CASTLE_WK | CASTLE_WQ))
                        && !canAttack(COLOR_BLACK, SQ_E1, gs)) {
                    whiteKingCastle(gs, board, moveArr, &count);
                }
                break;
        }
    }

    moveBuff->length = count;
    return count;
}

int generatePseudoMoves(GameState* gs, MoveBuffer* moveBuff) {
    const int toMove = gs->current->toMove;
    if (toMove == COLOR_WHITE) {
        return generatePseudoMovesWhite(gs, moveBuff);
    } else if (toMove == COLOR_BLACK) {
        return generatePseudoMovesBlack(gs, moveBuff);
    } else {
        fprintf(stderr, "Invalid to-move: %d\n", toMove);
        return 0;
    }
}

#undef PUSH_MOVE
