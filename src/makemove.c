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
#include "gamestate.h"
#include "piece.h"
#include "bitboard.h"
#include "hashconsts.h"
#include "board.h"
#include "makemove.h"
#include "statedata.h"

// Useful to debug what's being hashed in to the position
#define APPLY_MASK(mask) /*printf("applying mask to %016"PRIX64": %016"PRIX64"\n", hash, (uint64_t) (mask));*/ hash ^= (mask);

static void unCastleRook(GameState* gameState, const int homeSq, const int rookCastledSq, const int rookOrdinal, const Piece* rook) {
        const Piece** board = gameState->board;
        uint64_t* emptyBb = &gameState->bitboards[ORD_EMPTY];
        uint64_t* rookBb = &gameState->bitboards[rookOrdinal];
        *rookBb = (*rookBb & ~BITS_SQ[rookCastledSq]) | BITS_SQ[homeSq];
        *emptyBb = (*emptyBb & ~BITS_SQ[homeSq]) | BITS_SQ[rookCastledSq];
        board[homeSq] = rook;
        board[rookCastledSq] = &EMPTY;
}

static void whiteKingCastle(Move* move, GameState* gs, uint64_t* runningHash) {
        uint64_t hash = *runningHash;
        if (move->to == SQ_G1) {
                gs->board[SQ_H1] = &EMPTY;
                gs->board[SQ_F1] = &WROOK;
                APPLY_MASK(HASH_MASK_CASTLE_WK);
                uint64_t* rb = &gs->bitboards[ORD_WROOK];
                uint64_t* eb = &gs->bitboards[ORD_EMPTY];
                *rb = (*rb & ~((uint64_t)BIT_SQ_H1)) | ((uint64_t)BIT_SQ_F1);
                *eb = (*eb & ~BIT_SQ_F1) | BIT_SQ_H1;
        } else if (move->to == SQ_C1) {
                gs->board[SQ_A1] = &EMPTY;
                gs->board[SQ_D1] = &WROOK;
                APPLY_MASK(HASH_MASK_CASTLE_WQ);
                uint64_t* rb = &gs->bitboards[ORD_WROOK];
                uint64_t* eb = &gs->bitboards[ORD_EMPTY];
                *rb = (*rb & ~BIT_SQ_A1) | BIT_SQ_D1;
                *eb = (*eb & ~BIT_SQ_D1) | BIT_SQ_A1;
        }
        *runningHash = hash;
}

static void blackKingCastle(Move* move, GameState* gs, uint64_t* runningHash) {
        uint64_t hash = *runningHash;
        if (move->to == SQ_G8) {
                gs->board[SQ_H8] = &EMPTY;
                gs->board[SQ_F8] = &BROOK;
                APPLY_MASK(HASH_MASK_CASTLE_BK);
                uint64_t* rb = &gs->bitboards[ORD_BROOK];
                uint64_t* eb = &gs->bitboards[ORD_EMPTY];
                *rb = (*rb & ~BIT_SQ_H8) | BIT_SQ_F8;
                *eb = (*eb & ~BIT_SQ_F8) | BIT_SQ_H8;
        } else if (move->to == SQ_C8) {
                gs->board[SQ_A8] = &EMPTY;
                gs->board[SQ_D8] = &BROOK;
                APPLY_MASK(HASH_MASK_CASTLE_BQ);
                uint64_t* rb = &gs->bitboards[ORD_BROOK];
                uint64_t* eb = &gs->bitboards[ORD_EMPTY];
                *rb = (*rb & ~BIT_SQ_A8) | BIT_SQ_D8;
                *eb = (*eb & ~BIT_SQ_D8) | BIT_SQ_A8;
        }
        *runningHash = hash;
}

static void promotePawn(Move* move, GameState* gs, const int color, const int pawnOrdinal, uint64_t* runningHash) {
        const Piece* promotePiece = getPromotePiece(color, move->moveCode);

        uint64_t hash = *runningHash;

        // Delete pawn from board
        uint64_t* pawnBb = &gs->bitboards[pawnOrdinal];
        *pawnBb &= ~BITS_SQ[move->from];
        gs->pieceCounts[pawnOrdinal]--;
        APPLY_MASK(HASH_PIECE_SQ[move->to][pawnOrdinal]);

        // Add the new piece
        uint64_t* promoteBb = &gs->bitboards[promotePiece->ordinal];
        *promoteBb |= BITS_SQ[move->to];
        APPLY_MASK(HASH_PIECE_SQ[move->to][promotePiece->ordinal]);

        // Delete captured piece from dest square
        uint64_t* captureBb = &gs->bitboards[move->captures->ordinal];
        *captureBb &= ~BITS_SQ[move->to];

        // Place new piece
        gs->board[move->to] = promotePiece;
        gs->pieceCounts[promotePiece->ordinal]++;

        *runningHash = hash;
}

static void enPassant(GameState* gs, Move* move, const int attackSq, uint64_t* runningHash) {
        uint64_t hash = *runningHash;
        // Delete the captured piece
        uint64_t* captureBb = &gs->bitboards[move->captures->ordinal];
        *captureBb &= ~BITS_SQ[attackSq];
        uint64_t* emptyBb = &gs->bitboards[ORD_EMPTY];
        *emptyBb = (~BITS_SQ[move->to] & *emptyBb) | BITS_SQ[attackSq];
        gs->board[attackSq] = &EMPTY;
        APPLY_MASK(HASH_PIECE_SQ[attackSq][move->captures->ordinal]);
        APPLY_MASK(HASH_PIECE_SQ[attackSq][ORD_EMPTY]);

        // Put attacking pawn in new place
        uint64_t* movingBb = &gs->bitboards[move->movingPiece->ordinal];
        *movingBb = (~(BITS_SQ[move->from]) & *movingBb) | BITS_SQ[move->to];
        *runningHash = hash;
}

void makeNullMove(GameState* gameState) {
        // Copy the state data to the next item in the stack, move the pointer to the next element.
        StateData* nextData = (gameState->current) + 1;
        copyStateData(gameState->current, nextData);
        gameState->current = nextData;

        uint64_t hash = nextData->hash;

        // Adjust half-move, flip to-move
        nextData->halfMoveCount++;
        nextData->toMove = INVERT_COLOR(nextData->toMove);
        APPLY_MASK(HASH_WHITE_TO_MOVE);

        // Clear out the EP file to prevent really strange null positions.
        // Basically, the "wrong" side is to move after a null move, so the EP file is quite wrong.
        nextData->epFile = NO_EP_FILE;

        nextData->hash = hash;
}

void unmakeNullMove(GameState* gameState) {
        gameState->current--;
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
        const int32_t sqTo = move->to;
        const int32_t sqFrom = move->from;
        uint64_t* movingPieceBitboard = &gameState->bitboards[movingPiece->ordinal];
        uint64_t* capturedPieceBitboard = &gameState->bitboards[capturedPiece->ordinal];
        uint64_t bitboardSqTo = BITS_SQ[sqTo];
        uint64_t bitboardSqFrom = BITS_SQ[sqFrom];

        const Piece** board = gameState->board;

        uint64_t hash = nextData->hash;

        // Adjust half-move, flip to-move
        nextData->halfMoveCount++;
        nextData->toMove = INVERT_COLOR(nextData->toMove);
        APPLY_MASK(HASH_WHITE_TO_MOVE);

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
                if (capturedPiece->color == COLOR_WHITE) {
                        nextData->whitePieceCount--;
                } else {
                        nextData->blackPieceCount--;
                }
        }

        if (move->moveCode == NO_MOVE_CODE) {
                // Delete moving piece from source square, place in destination square.
                *movingPieceBitboard = (~bitboardSqFrom & *movingPieceBitboard) | bitboardSqTo;
                // Delete captured piece from dest square
                *capturedPieceBitboard &= ~bitboardSqTo;
        }

        // Place "empty" at the source square
        gameState->bitboards[ORD_EMPTY] |= bitboardSqFrom;
        APPLY_MASK(HASH_PIECE_SQ[sqFrom][movingPiece->ordinal]);
        APPLY_MASK(HASH_PIECE_SQ[sqFrom][ORD_EMPTY]);
        board[sqFrom] = &EMPTY;

        // Remove the target square and replace with what's moving.
        // In the case of most moves, the target square will contain whatever piece is being captured (movingPiece->capturedPiece)
        // In EP moves, this is always empty, and the captuedPiece is a pawn (which isn't actually what's in the destination square)
        if (move->moveCode == CAPTURE_EP) {
                APPLY_MASK(HASH_PIECE_SQ[sqTo][ORD_EMPTY]);
        } else {
                APPLY_MASK(HASH_PIECE_SQ[sqTo][capturedPiece->ordinal]);
        }

        APPLY_MASK(HASH_PIECE_SQ[sqTo][movingPiece->ordinal]);
        board[sqTo] = movingPiece;

        // Update EP file
        int32_t oldEpFile = nextData->epFile;
        int32_t newEpFile = isPawn && abs(sqTo - sqFrom) == (2 * OFFSET_N)
                            ? FILE_IDX(sqTo) : NO_EP_FILE;
        if (oldEpFile != newEpFile) {
                APPLY_MASK(HASH_EP_FILE[oldEpFile]);
                APPLY_MASK(HASH_EP_FILE[newEpFile]);
                nextData->epFile = newEpFile;
        }

        // Piece-specific special moves: EP captures, promotions, castling.
        switch (movingPiece->ordinal) {
        case ORD_WKING:
                if (nextData->castleFlags & (CASTLE_WK | CASTLE_WQ)) {
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                        nextData->castleFlags &= ~(CASTLE_WK | CASTLE_WQ);
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                }
                nextData->whiteKingSquare = sqTo;
                if (sqFrom == SQ_E1) {
                        whiteKingCastle(move, gameState, &hash);
                }
                break;
        case ORD_BKING:
                if (nextData->castleFlags & (CASTLE_BK | CASTLE_BQ)) {
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                        nextData->castleFlags &= ~(CASTLE_BK | CASTLE_BQ);
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                }
                nextData->blackKingSquare = sqTo;
                if (sqFrom == SQ_E8) {
                        blackKingCastle(move, gameState, &hash);
                }
                break;
        case ORD_WROOK:
                if (sqFrom == SQ_H1 && nextData->castleFlags & CASTLE_WK) {
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                        nextData->castleFlags &= ~(CASTLE_WK);
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                } else if (sqFrom == SQ_A1 && nextData->castleFlags & CASTLE_WQ) {
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                        nextData->castleFlags &= ~(CASTLE_WQ);
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                }
                break;
        case ORD_BROOK:
                if (sqFrom == SQ_H8 && nextData->castleFlags & CASTLE_BK) {
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                        nextData->castleFlags &= ~(CASTLE_BK);
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                } else if (sqFrom == SQ_A8 && nextData->castleFlags & CASTLE_BQ) {
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                        nextData->castleFlags &= ~(CASTLE_BQ);
                        APPLY_MASK(HASH_PIECE_CASTLE[nextData->castleFlags]);
                }
                break;
        case ORD_WPAWN:
                if (IS_PROMOTE(move->moveCode)) {
                        promotePawn(move, gameState, COLOR_WHITE, ORD_WPAWN, &hash);
                } else if (move->moveCode == CAPTURE_EP) {
                        enPassant(gameState, move, sqTo + OFFSET_S, &hash);
                }
                break;
        case ORD_BPAWN:
                if (IS_PROMOTE(move->moveCode)) {
                        promotePawn(move, gameState, COLOR_BLACK, ORD_BPAWN, &hash);
                } else if (move->moveCode == CAPTURE_EP) {
                        enPassant(gameState, move, sqTo + OFFSET_N, &hash);
                }
                break;
        }

        nextData->hash = hash;
}

void unmakeMove(GameState* gameState, Move* move) {
        const Piece** board = gameState->board;
        const Piece* moving = move->movingPiece;
        const Piece* captured = move->captures;
        uint64_t fromBb = BITS_SQ[move->from];
        uint64_t toBb = BITS_SQ[move->to];

        // Source square no longer empty
        uint64_t* emptyBb = &gameState->bitboards[ORD_EMPTY];
        *emptyBb &= ~fromBb;

        // Move the piece back to where it started
        uint64_t* movingBb = &gameState->bitboards[moving->ordinal];
        *movingBb = (*movingBb & ~toBb) | fromBb;
        board[move->from] = moving;

        if (move->moveCode != CAPTURE_EP) {
                // Put the captured piece back on the board
                uint64_t* capturedBb = &gameState->bitboards[captured->ordinal];
                *capturedBb |= toBb;
                board[move->to] = captured;
        } else {
                const int epSquare = move->to + (move->movingPiece == &WPAWN ? OFFSET_S : OFFSET_N);
                const uint64_t epSqBits = BITS_SQ[epSquare];
                uint64_t* capturedBb = &gameState->bitboards[captured->ordinal];
                board[epSquare] = move->captures;
                board[move->to] = &EMPTY;
                *capturedBb |= epSqBits;
                *emptyBb &= ~epSqBits;
                *emptyBb |= BITS_SQ[move->to];
        }

        if (IS_PROMOTE(move->moveCode)) {
                const Piece* pawn = move->movingPiece;
                const Piece* promotePiece = getPromotePiece(pawn->color, move->moveCode);

                gameState->pieceCounts[pawn->ordinal]++;
                gameState->pieceCounts[promotePiece->ordinal]--;

                uint64_t* promoteBb = &gameState->bitboards[promotePiece->ordinal];
                *promoteBb &= ~BITS_SQ[move->to];
        }

        if (captured != &EMPTY) {
                int32_t* pc = gameState->pieceCounts;
                pc[captured->ordinal]++;
                pc[ORD_EMPTY]--;
        }

        if (moving == &WKING && move->from == SQ_E1) {
                if (move->to == SQ_G1) {
                        unCastleRook(gameState, SQ_H1, SQ_F1, ORD_WROOK, &WROOK);
                } else if (move->to == SQ_C1) {
                        unCastleRook(gameState, SQ_A1, SQ_D1, ORD_WROOK, &WROOK);
                }
        } else if (moving == &BKING && move->from == SQ_E8) {
                if (move->to == SQ_G8) {
                        unCastleRook(gameState, SQ_H8, SQ_F8, ORD_BROOK, &BROOK);
                } else if (move->to == SQ_C8) {
                        unCastleRook(gameState, SQ_A8, SQ_D8, ORD_BROOK, &BROOK);
                }
        }

        gameState->current--;
}

#undef APPLY_MASK
