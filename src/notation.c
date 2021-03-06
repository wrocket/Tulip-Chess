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
#include <string.h>
#include <inttypes.h>

#ifdef __linux__
#include <strings.h>
#endif

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

static int32_t notation_printMoveDisambiguation(GameState* g, Move* move, char* buffer) {
        int32_t c = 0;
        MoveBuffer legalMoves;
        int32_t* collidingMoves;
        int32_t collidingMoveCnt = 0;

        createMoveBuffer(&legalMoves);
        generateLegalMoves(g, &legalMoves);

        collidingMoves = ALLOC((uint64_t) legalMoves.length, int32_t, collidingMoves, "Unable to allocate ambiguous move buffer.");

        // Look for moves where both the moving piece and destination square are the same.
        // See http://en.wikipedia.org/wiki/Algebraic_notation_(chess)#Disambiguating_moves for rules.
        for (int32_t i = 0; i < legalMoves.length; i++) {
                Move* candidate = &legalMoves.moves[i];
                if (doMoveCollide(move, candidate)) {
                        collidingMoves[collidingMoveCnt++] = candidate->from;
                }
        }

        if (collidingMoveCnt > 0) {
                bool sameRank = false;
                bool sameFile = false;

                const int32_t fromRank = RANK_IDX(move->from);
                const int32_t fromFile = FILE_IDX(move->from);

                for (int i = 0; i < collidingMoveCnt; i++) {
                        const int32_t ambigSq = collidingMoves[i];
                        sameRank |= fromRank == RANK_IDX(ambigSq);
                        sameFile |= fromFile == FILE_IDX(ambigSq);
                }

                // Disambiguate in the following order:
                // 1. First by file. If that doesn't fix it...
                // 2. Next by rank. If that doesn't fix it...
                // 3. File and rank. That has to fix it.
                if (sameFile && sameRank) {
                        c = printSquareIndex(move->from, buffer);
                } else if (sameRank || !sameFile) {
                        buffer[c++] = fileToChar(fromFile);
                } else if (sameFile) {
                        buffer[c++] = rankToChar(fromRank);
                }
        }

        free(collidingMoves);
        destroyMoveBuffer(&legalMoves);

        return c;
}

int32_t notation_printShortAlg(Move* move, GameState* gameState, char* buffer) {
        int32_t count = 0;
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
                buffer[count++] = (char) toupper(movingPiece->name);
                count += notation_printMoveDisambiguation(gameState, move, &buffer[count]);
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
                buffer[count++] = (char) toupper(getPromotePiece(movingPiece->color, move->moveCode)->name);
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

// The idea here is to strip a move down to an easy form to "fuzzy" match move strings.
// By removing extra decorations (check status, capture notation, whitespace) and such,
// we can do a reasonable job of matching against move strings generated by other programs.
static void normalizeMove(char* original, char* normalized) {
        char* c = original;
        char* start = normalized;
        int32_t cnt = 0;
        while (*c) {
                if (isalnum(*c) && tolower(*c) != 'x') {
                        // Normalize algebraic castling to "oh" instead of "zero"
                        char cv;
                        if (*c == '0') {
                                cv = 'O';
                        } else {
                                cv = *c;
                        }

                        // Normalize casing. We have an exception for the letter 'B' since two moves like
                        // Bxc3 and bxc3 can both be legal (and be very different moves, one a pawn, the other a bishop)
                        *normalized = cv == 'B' ? cv : (char) tolower(cv);
                        normalized++;
                        cnt++;
                }
                c++;
        }

        *normalized = '\0';

        // Lop off trailing 'ep' designating pawn en passant captures.
        if (cnt >= 3) {
                char* ep_file = strstr(start, "ep");
                if (ep_file && ep_file == normalized - 2) {
                        *(normalized - 2) = '\0';
                }
        }
}

static bool checkMoveMatch(char* normalizedGeneratedMove, char* normalizedInputMove, Move currentMove, Move* resultMove) {
        bool result = false;
        if (strcmp(normalizedGeneratedMove, normalizedInputMove) == 0) {
                result = true;
                *resultMove = currentMove;
        }

        return result;
}

bool notation_matchMove(char* str, GameState* gs, Move* m) {
        MoveBuffer buffer;
        char moveStr[16];
        char normalizedMove[16];
        char normalizedInputMove[16];
        bool result = false;

        createMoveBuffer(&buffer);
        generateLegalMoves(gs, &buffer);

        normalizeMove(str, normalizedInputMove);

        for (int32_t i = 0; i < buffer.length; i++) {
                Move currentMove = buffer.moves[i];

                // Fist match with algebraic notation
                notation_printShortAlg(&currentMove, gs, moveStr);
                normalizeMove(moveStr, normalizedMove);
                result = checkMoveMatch(normalizedMove, normalizedInputMove, currentMove, m);
                if (result) {
                        break;
                }

                // Next match with coordinate notation
                notation_printMoveCoordinate(&currentMove, moveStr);
                normalizeMove(moveStr, normalizedMove);
                result = checkMoveMatch(normalizedMove, normalizedInputMove, currentMove, m);
                if (result) {
                        break;
                }
        }

        destroyMoveBuffer(&buffer);
        return result;
}


bool notation_matchPseudoMoveCoord(GameState* gameState, char* moveStr, Move* m) {
        MoveBuffer moveBuff;
        int32_t moveCount;
        bool result = false;
        char moveStrBuff[8];

        createMoveBuffer(&moveBuff);
        moveCount = generatePseudoMoves(gameState, &moveBuff);

        for (int32_t i = 0; i < moveCount; i++) {
                Move current = moveBuff.moves[i];
                notation_printMoveCoordinate(&current, moveStrBuff);

                if (strcasecmp(moveStrBuff, moveStr) == 0) {
                        result = true;
                        *m = current;
                        break;
                }
        }

        destroyMoveBuffer(&moveBuff);

        return result;
}

int32_t notation_printMoveCoordinate(Move* move, char* buffer) {
        int32_t index = 0;

        index += printSquareIndex(move->from, buffer);
        index += printSquareIndex(move->to, buffer + index);

        switch (move->moveCode) {
        case PROMOTE_N:
                buffer[index++] = '=';
                buffer[index++] = 'n';
                break;
        case PROMOTE_R:
                buffer[index++] = '=';
                buffer[index++] = 'r';
                break;
        case PROMOTE_B:
                buffer[index++] = '=';
                buffer[index++] = 'b';
                break;
        case PROMOTE_Q:
                buffer[index++] = '=';
                buffer[index++] = 'q';
                break;
        }

        buffer[index++] = '\0';

        return index;
}
