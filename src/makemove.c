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
#include "piece.h"
#include "bitboard.h"
#include "board.h"
#include "makemove.h"
#include "statedata.h"

static void whiteKingCastle(Move* move, GameState* gs) {
    if (move->to == SQ_G1) {
        gs->board[SQ_H1] = &EMPTY;
        gs->board[SQ_F1] = &WROOK;
        uint64_t* rb = &gs->bitboards[ORD_WROOK];
        uint64_t* eb = &gs->bitboards[ORD_EMPTY];
        *rb = (*rb & ~BIT_SQ_H1) | BIT_SQ_F1;
        *eb = (*eb & ~BIT_SQ_F1) | BIT_SQ_H1;
    } else if (move->to == SQ_C1) {
        gs->board[SQ_A1] = &EMPTY;
        gs->board[SQ_D1] = &WROOK;
        uint64_t* rb = &gs->bitboards[ORD_WROOK];
        uint64_t* eb = &gs->bitboards[ORD_EMPTY];
        *rb = (*rb & ~BIT_SQ_A1) | BIT_SQ_D1;
        *eb = (*eb & ~BIT_SQ_D1) | BIT_SQ_A1;
    }
}

static void blackKingCastle(Move* move, GameState* gs) {
    if (move->to == SQ_G8) {
        gs->board[SQ_H8] = &EMPTY;
        gs->board[SQ_F8] = &BROOK;
        uint64_t* rb = &gs->bitboards[ORD_BROOK];
        uint64_t* eb = &gs->bitboards[ORD_EMPTY];
        *rb = (*rb & ~BIT_SQ_H8) | BIT_SQ_F8;
        *eb = (*eb & ~BIT_SQ_F8) | BIT_SQ_H8;
    } else if (move->to == SQ_C8) {
        gs->board[SQ_A8] = &EMPTY;
        gs->board[SQ_D8] = &BROOK;
        uint64_t* rb = &gs->bitboards[ORD_BROOK];
        uint64_t* eb = &gs->bitboards[ORD_EMPTY];
        *rb = (*rb & ~BIT_SQ_A8) | BIT_SQ_D8;
        *eb = (*eb & ~BIT_SQ_D8) | BIT_SQ_A8;
    }
}

void makeMove(GameState* gameState, Move* move) {
    // Copy the state data to the next item in the stack, move the pointer to the next element.
    StateData* nextData = (gameState->current) + 1;
    copyStateData(gameState->current, nextData);
    gameState->current = nextData;
    const Piece* movingPiece = move->movingPiece;
    const Piece* capturedPiece = move->captures;
    const bool isPawn = movingPiece == &BPAWN || movingPiece == &WPAWN;
    const bool isCapture = capturedPiece != &EMPTY;

    uint64_t* movingPieceBitboard = &gameState->bitboards[movingPiece->ordinal];
    uint64_t* capturedPieceBitboard = &gameState->bitboards[capturedPiece->ordinal];
    uint64_t bitboardSqTo = BITS_SQ[move->to];
    uint64_t bitboardSqFrom = BITS_SQ[move->from];

    const Piece** board = gameState->board;

    // Adjust half-move, flip to-move
    nextData->halfMoveCount++;
    nextData->toMove = INVERT_COLOR(nextData->toMove);

    // Update fifty move count.
    if (isPawn || isCapture) {
        nextData->fiftyMoveCount = 0;
    } else {
        nextData->fiftyMoveCount++;
    }

    // Decrement relevant piece counts
    if (isCapture) {
        gameState->pieceCounts[capturedPiece->ordinal]--;
        gameState->pieceCounts[ORD_EMPTY]++;
    }

    if (move->moveCode == NO_MOVE_CODE) {
        // Delete moving piece from source square, place in destination square.
        *movingPieceBitboard = (~bitboardSqFrom & *movingPieceBitboard) | bitboardSqTo;
        // Delete captured piece from dest square
        *capturedPieceBitboard &= ~bitboardSqTo;
    }

    // Place "empty" at the source square
    gameState->bitboards[ORD_EMPTY] |= bitboardSqFrom;

    board[move->to] = movingPiece;
    board[move->from] = &EMPTY;

    // Update EP file
    int newEpFile = isPawn && abs(move->to - move->from) == (2 * OFFSET_N) 
                        ? FILE_IDX(move->to) : NO_EP_FILE;
    nextData->epFile = newEpFile;

    // Piece-specific special moves: EP captures, promotions, castling.
    switch (movingPiece->ordinal) {
        case ORD_WKING:
            nextData->castleFlags &= ~(CASTLE_WK | CASTLE_WQ);
            nextData->whiteKingSquare = move->to;
            if (move->from == SQ_E1) {
                whiteKingCastle(move, gameState);
            }
            break;
        case ORD_BKING:
            nextData->castleFlags &= ~(CASTLE_BQ | CASTLE_BK);
            nextData->blackKingSquare = move->to;
            if (move->from == SQ_E8) {
                blackKingCastle(move, gameState);
            }
            break;
    }
}

void unmakeMove(GameState* gameState, Move* move) {

}
