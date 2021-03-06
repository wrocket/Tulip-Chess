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
#include <inttypes.h>

#include "gamestate.h"
#include "board.h"
#include "notation.h"
#include "hash.h"
#include "move.h"
#include "json.h"
#include "result.h"
#include "search.h"
#include "eval.h"

#define __STDC_FORMAT_MACROS

// TODO: Use a real JSON library for this.
static const char* maskBooleanToStrU(int32_t value, int32_t mask) {
    return (value & mask) ? "true" : "false";
}

static void printSq(char* buff, int32_t sq) {
    int32_t idx = printSquareIndex(sq, buff);
    buff[idx] = '\0';
}

static void printMoveDetail(Move* m) {
    char strBuff[16];
    notation_printMoveCoordinate(m, strBuff);
    printf("{");
    printf("\"move\": \"%s\", ", strBuff);
    printf("\"movingPiece\": \"%c\", ", m->movingPiece->name);
    printf("\"capturedPiece\": \"%c\", ", m->captures->name);

    const char* codeStr;

    switch (m->moveCode) {
    case NO_MOVE_CODE:
        codeStr = "none";
        break;
    case PROMOTE_R:
        codeStr = "promoteRook";
        break;
    case PROMOTE_B:
        codeStr = "promoteBishop";
        break;
    case PROMOTE_N:
        codeStr = "promoteKnight";
        break;
    case PROMOTE_Q:
        codeStr = "promoteQueen";
        break;
    case CAPTURE_EP:
        codeStr = "epCapture";
        break;
    default:
        codeStr = "unknown";
        break;
    }

    printf("\"moveCode\": \"%s\"", codeStr);
    printf("}");
}

static const char* printColor(int32_t color) {
    switch (color) {
    case COLOR_WHITE: return "white";
    case COLOR_BLACK: return "black";
    default: return "unknown";
    }
}

void printMovelistJson(char* position, char* listType, GameState* gameState, MoveBuffer* buffer) {
    char strBuff[16];

    printf("{");

    printf("\"fenString\": \"%s\", ", position);

    printf("\"moveListType\": \"%s\", ", listType);

    printf("\"moveList\": [");
    for (int32_t i = 0; i < buffer->length; i++) {
        if (i > 0) {
            printf(", ");
        }

        notation_printMoveCoordinate(&(buffer->moves[i]), strBuff);
        printf("\"%s\"", strBuff);
    }
    printf("], ");

    printf("\"shortAlgebraicMoves\":[");
    for (int32_t i = 0; i < buffer->length; i++) {
        if (i > 0) {
            printf(", ");
        }

        notation_printShortAlg(&(buffer->moves[i]), gameState, strBuff);
        printf("\"%s\"", strBuff);
    }

    printf("], ");

    printf("\"moveDetails\": [");
    for (int32_t i = 0; i < buffer->length; i++) {
        if (i > 0) {
            printf(", ");
        }

        printMoveDetail(&(buffer->moves[i]));
    }
    printf("]");
    printf("}\n");
}

void printAttackList(char* position, bool* attackGrid, GameState* state) {
    printf("{");
    printf("\"fenString\": \"%s\", ", position);
    printf("\"attackedSquares\": [");

    char strBuff[4];

    bool atLeastOne = false;
    for (int32_t i = 0; i < 64; i++) {
        int32_t idx = BOARD_SQUARES[i];
        if (attackGrid[idx]) {
            if (atLeastOne) {
                printf(", ");
            }

            atLeastOne = true;
            strBuff[printSquareIndex(idx, strBuff)] = '\0';
            printf("\"%s\"", strBuff);
        }
    }

    printf("]}\n");
}

void printMakeMoveResult(char* position, Move* m, GameState* state) {
    char moveStr[8];

    if (!m) {
        sprintf(moveStr, "NullMv");
    } else {
        notation_printMoveCoordinate(m, moveStr);
    }

    printf("{");
    printf("\"move\": \"%s\", ", moveStr);
    printf("\"originalState\": \"%s\", ", position);
    printf("\"resultingState\": ");
    printGameState("", state);
    printf("}\n");
}

void printGameState(char* position, GameState* state) {
    char squareStr[8];
    StateData* stateData;

    stateData = state->current;

    printf("{");
    printf("\"fenString\": \"%s\", ", position);

    printf("\"toMove\": \"%s\", ", printColor(stateData->toMove));

    printSq(squareStr, stateData->whiteKingSquare);
    printf("\"whiteKingSquare\": \"%s\", ", squareStr);

    printSq(squareStr, stateData->blackKingSquare);
    printf("\"blackKingSquare\": \"%s\", ", squareStr);

    printf("\"fiftyMoveCount\": %i, ", stateData->fiftyMoveCount);
    printf("\"halfMoveCount\": %i, ", stateData->halfMoveCount);

    const int32_t flags = stateData->castleFlags;
    printf("\"castleWhiteKingside\": %s, ", maskBooleanToStrU(flags, CASTLE_WK));
    printf("\"castleWhiteQueenside\": %s, ", maskBooleanToStrU(flags, CASTLE_WQ));
    printf("\"castleBlackKingside\": %s, ", maskBooleanToStrU(flags, CASTLE_BK));
    printf("\"castleBlackQueenside\": %s, ", maskBooleanToStrU(flags, CASTLE_BQ));

    printf("\"epFile\": ");

    if (stateData->epFile == NO_EP_FILE) {
        printf("\"none\", ");
    } else {
        printf("\"%c\", ", fileToChar(stateData->epFile));
    }

    printf("\"hash\": \"%016"PRIX64"\", ", state->current->hash);
    printf("\"recalculatedHash\": \"%016"PRIX64"\", ", computeHash(state));

    printf("\"board\": {");
    bool printedSq = false;
    for (int32_t i = 0; i < 64; i++) {
        const int32_t sq = BOARD_SQUARES[i];
        const Piece* p = state->board[sq];
        if (p != &EMPTY) {
            if (printedSq) {
                printf(", ");
            }

            printSq(squareStr, sq);
            printf("\"%s\": \"%c\"", squareStr, p->name);
            printedSq = true;
        }
    }

    printf("}, ");

    printf("\"whitePieceCount\": %i, \"blackPieceCount\": %i, ", state->current->whitePieceCount, state->current->blackPieceCount);

    printf("\"pieceCounts\": {");
    const Piece* pieces[] = {&WPAWN, &BPAWN, &WKNIGHT, &BKNIGHT, &WBISHOP,
                             &BBISHOP, &WROOK, &BROOK, &WQUEEN, &BQUEEN,
                             &WKING, &BKING, &EMPTY, &OFF_BOARD
                            };

    for (int32_t i = 0; i < 14; i++) {
        if (i > 0) {
            printf(", ");
        }

        printf("\"%c\": %i", (pieces[i])->name, state->pieceCounts[pieces[i]->ordinal]);

    }
    printf("},");
    printf(" \"bitboards\" : {");
    for (int32_t i = 0; i < ALL_PIECES_LEN; i++) {
        if (i > 0) {
            printf(", ");
        }

        const Piece* p = ALL_PIECES[i];
        printf("\"%c\": \"%016"PRIX64"\"", p->name, state->bitboards[p->ordinal]);
    }
    printf("}");
    printf("}\n");
}

void printMatchMoveResult(Move* move) {
    printf("{\"matchedMove\": ");
    if (move) {
        printMoveDetail(move);
    } else {
        printf("null");
    }
    printf("}\n");
}

void printCheckStatus(char* position, bool isCheck) {
    printf("{");
    printf("\"fenString\": \"%s\", ", position);
    printf("\"isCheck\": %s", isCheck ? "true" : "false");
    printf("}\n");
}

static const char* gameStatusToStr(int32_t status) {
    switch (status) {
    case STATUS_NONE:
        return "none";
        break;
    case STATUS_WHITE_CHECKMATED:
        return "whiteCheckmated";
        break;
    case STATUS_BLACK_CHECKMATED:
        return "blackCheckmated";
        break;
    case STATUS_STALEMATE:
        return "stalemate";
        break;
    case STATUS_MATERIAL_DRAW:
        return "materialDraw";
        break;
    case STATUS_THREEFOLD_DRAW:
        return "threefoldDraw";
        break;
    case STATUS_FIFTY_MOVE_DRAW:
        return "fiftyMoveDraw";
        break;
    default:
        return "unknown";
        break;
    }
}

void printGameStatus(char* position, int32_t status) {
    const char* resultStr = gameStatusToStr(status);

    printf("{");
    printf("\"fenString\": \"%s\", ", position);
    printf("\"status\": \"%s\"", resultStr);
    printf("}\n");
}

void printHashSequence(HashSeqItem* items, int32_t length, uint64_t initialHash) {
    printf("{");
    printf("\"initialHash\": \"%016"PRIX64"\", ", initialHash);
    printf("\"hashSequence\": [");
    for (int32_t i = 0; i < length; i++) {
        HashSeqItem item = items[i];
        if (i > 0) {
            printf(", ");
        }

        printf("{\"move\":\"%s\", \"resultingHash\": \"%016"PRIX64"\"}", item.move, item.hash);
    }
    printf("]");
    printf("}\n");
}

static void printSquareArray(int32_t*  squares, int32_t count) {
    char sqStr[4];
    printf("[");
    for (int32_t i = 0; i < count; i++) {
        if (i > 0) {
            printf(", ");
        }

        printSquareIndex(squares[i], sqStr);
        printf("\"%s\"", sqStr);
    }
    printf("]");
}

void printPassedPawns(char* position, int32_t*  wPawns, int32_t wCount, int32_t*  bPawns, int32_t bCount) {
    printf("{");
    printf("\"fenString\": \"%s\", ", position);
    printf("\"whitePassedPawns\": ");
    printSquareArray(wPawns, wCount);
    printf(", \"blackPassedPawns\": ");
    printSquareArray(bPawns, bCount);
    printf("}\n");
}

void printEvaluation(char* position, int32_t score) {
    printf("{");
    printf("\"fenString\": \"%s\", ", position);
    printf("\"score\": %i", score);
    printf("}\n");
}

void printKingRectSize(char* sq, int32_t size) {
    printf("{\"square\": \"%s\", \"rectangleSize\": %d}\n", sq, size);
}

void printEndgameClassification(int32_t type) {
    const char* typeStr;
    if (type == ENDGAME_UNCLASSIFIED) {
        typeStr = "unclassified";
    } else if (type == ENDGAME_BROOKvKING) {
        typeStr = "krvk_black";
    } else if (type == ENDGAME_WROOKvKING) {
        typeStr = "krvk_white";
    } else {
        typeStr = "unknown";
    }

    printf("{\"endgameType\":\"%s\"}\n", typeStr);
}

void printSearchResult(SearchResult* result, GameState* state) {
    char moveStr[16];
    const bool printMove = result->searchStatus == SEARCH_STATUS_NONE;

    if (printMove) {
        notation_printShortAlg(&result->move, state, moveStr);
    }

    const char* statusStr = result->searchStatus == SEARCH_STATUS_NONE ? "none" : "noLegalMoves";

    printf("{\"searchResult\": {");
    printf("\"move\": ");
    if (printMove) {
        printf("\"%s\",", moveStr);
    } else {
        printf("null,");
    }

    // Mini-hack: Treat NaN nodes per second as zero.
    double nps;
    if (result->durationMs > 0) {
        nps = (double) result->nodes / (double) result->durationMs * 1000.0;
    } else {
        nps = 0.0;
    }

    printf("\"score\": %i,", result->score);
    printf("\"status\": \"%s\",", statusStr);
    printf("\"nodes\": %ld,", result->nodes);
    printf("\"elapsedMs\": %ld,", result->durationMs);
    printf("\"nodesPerSecond\": %0.2f,", nps);
    printf("\"rootNodeScores\": [");
    for (int32_t i = 0; i < result->moveScoreLength; i++) {
        if (i != 0) {
            printf(", ");
        }

        MoveScore score = result->moveScores[i];
        notation_printShortAlg(&score.move, state, moveStr);
        printf("{\"move\":\"%s\", \"score\":%i, \"depth\":%i}", moveStr, score.score, score.depth);
    }
    printf("]}}");
}
