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
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "sqlite/sqlite3.h"

#include "attack.h"
#include "board.h"
#include "draw.h"
#include "fen.h"
#include "gamestate.h"
#include "eval.h"
#include "book.h"
#include "json.h"
#include "makemove.h"
#include "move.h"
#include "movegen.h"
#include "notation.h"
#include "result.h"
#include "piece.h"
#include "statedata.h"
#include "tulip.h"
#include "util.h"
#include "search.h"
#include "xboard.h"
#include "bitboard.h"
#include "interactive.h"

static void printBanner() {
    printf("Tulip Chess Engine 0.001\n");
    printf("Size of uint64: %lu bits\n", CHAR_BIT * sizeof(uint64_t));
    printf("Using SQLite %s\n", sqlite3_libversion());
}

// Simple alternative to getopt(). Consider using getopt anyway...
static const char* findArg(int argc, char** argv, const char* argName) {
    const char* result = NULL;

    for (int i = 0; i < argc; i++) {
        char* arg = argv[i];
        if (strcmp(arg, argName) == 0 && i < argc - 1) {
            result = argv[i + 1];
            break;
        }
    }

    return result;
}

static GameState parseFenOrQuit(char* str) {
    GameState gs;
    initializeGamestate(&gs);

    if (!parseFen(&gs, str)) {
        fprintf(stderr, "Unable to parse FEN \"%s\"\n", str);
        destroyGamestate(&gs);
        exit(EXIT_FAILURE);
    }

    return gs;
}

static void printState(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -printstate \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);

    printGameState(argv[1], &gs);
    destroyGamestate(&gs);
}

static void checkPassedPawn(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -passedpawns \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);

    int whitePassedPawns[8];
    int whiteCount = 0;
    int blackPassedPawns[8];
    int blackCount = 0;

    for (int32_t sqIdx = 0; sqIdx < 64; sqIdx++) {
        const int32_t sq = BOARD_SQUARES[sqIdx];
        const Piece* p = gs.board[sq];
        if (p == &WPAWN) {
            if ((BITS_PASSED_PAWN_W[sq] & gs.bitboards[ORD_BPAWN]) == 0) {
                whitePassedPawns[whiteCount++] = sq;
            }
        } else if (p == &BPAWN) {
            if ((BITS_PASSED_PAWN_B[sq] & gs.bitboards[ORD_WPAWN]) == 0) {
                blackPassedPawns[blackCount++] = sq;
            }
        }
    }

    printPassedPawns(argv[1], whitePassedPawns, whiteCount, blackPassedPawns, blackCount);

    destroyGamestate(&gs);
}

static void listAttacks(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -listattacks \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);

    bool* attackGrid = ALLOC(144, bool, attackGrid, "Unable to allocate attack grid.");
    for (int rank = RANK_1; rank <= RANK_8; rank++) {
        for (int file = FILE_A; file <= FILE_H; file++) {
            int idx = B_IDX(file, rank);
            attackGrid[idx] = canAttack(gs.current->toMove, idx, &gs);
        }
    }

    printAttackList(argv[1], attackGrid, &gs);

    free(attackGrid);
    destroyGamestate(&gs);
}

static void listMoves(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: -listmoves [pseudo/legal] \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[2]);

    MoveBuffer buffer;
    createMoveBuffer(&buffer);

    if (0 == strcmp("pseudo", argv[1])) {
        generatePseudoMoves(&gs, &buffer);
    } else if (0 == strcmp("legal", argv[1])) {
        generateLegalMoves(&gs, &buffer);
    } else {
        fprintf(stderr, "Move list mode \"%s\" unknown.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    printMovelistJson(argv[2], argv[1], &gs, &buffer);

    destroyMoveBuffer(&buffer);
    destroyGamestate(&gs);
}

static void printMoveResult(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: -makemove [moveString] \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[2]);
    Move m;

    if (!notation_matchMove(argv[1], &gs, &m)) {
        fprintf(stderr, "Unknown move \"%s\" for position %s\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    makeMove(&gs, &m);
    printMakeMoveResult(argv[2], &m, &gs);
    destroyGamestate(&gs);
}

static void printNullMoveResult(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -nullmove \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);
    makeNullMove(&gs);
    printMakeMoveResult(argv[2], NULL, &gs);
    destroyGamestate(&gs);
}

static void checkStatus(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -checkstatus \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);

    bool posCheck = isCheck(&gs);

    printCheckStatus(argv[1], posCheck);

    destroyGamestate(&gs);
}

static void findBookMoves(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: -bookmoves [book file] \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    char* bookFile = argv[1];
    char* fen = argv[2];
    OpenBook book;
    GameState gs = parseFenOrQuit(fen);
    MoveBuffer buff;
    createMoveBuffer(&buff);

    if (!book_open(bookFile, &book)) {
        fprintf(stderr, "Unable to open book [%s]\n", bookFile);
        exit(EXIT_FAILURE);
    }

    book_getMoves(&gs, &buff, &book);
    book_close(&book);

    printMovelistJson(fen, "legal", &gs, &buff);

    destroyMoveBuffer(&buff);
    destroyGamestate(&gs);
}

static void evalPosition(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -evalposition \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    char* fen = argv[1];
    GameState gs = parseFenOrQuit(fen);
    int mult = gs.current->toMove == COLOR_WHITE ? 1 : -1;

    int result = mult * evaluate(&gs);

    printEvaluation(fen, result);
    destroyGamestate(&gs);
}

static void printMakeUnmakeMoveResult(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: -makeunmake [moveString] \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[2]);
    Move m;

    if (!notation_matchMove(argv[1], &gs, &m)) {
        fprintf(stderr, "Unknown move \"%s\" for position %s\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    makeMove(&gs, &m);
    unmakeMove(&gs, &m);
    printGameState(argv[2], &gs);

    destroyGamestate(&gs);
}

static void printMatchMove(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: -matchmove [moveString] \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[2]);

    Move m;
    if (!notation_matchMove(argv[1], &gs, &m)) {
        printMatchMoveResult(NULL);
    } else {
        printMatchMoveResult(&m);
    }

    destroyGamestate(&gs);
}

static void makeMovesAndPrintGameResultStatus(int argc, char** argv) {
    Move m;

    if (argc < 2) {
        fprintf(stderr, "Usage: -gamestatus \"[FEN string]\" move1 move2 moveN\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);

    for (int i = 2; i < argc; i++) {
        if (!notation_matchMove(argv[i], &gs, &m)) {
            fprintf(stderr, "Unplayable move: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }

        makeMove(&gs, &m);
    }

    int result = getResult(&gs);
    printGameStatus(argv[1], result);
    destroyGamestate(&gs);
}

static void hashSequence(int argc, char** argv) {
    Move m;
    HashSeqItem* seqItems = ALLOC(512, HashSeqItem, seqItems, "Unable to allocate hash seq items array");
    int count = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: -hashseq \"[FEN string]\" move1 move2 moveN\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);

    const uint64_t initialHash = gs.current->hash;

    for (int i = 2; i < argc; i++) {
        if (!notation_matchMove(argv[i], &gs, &m)) {
            fprintf(stderr, "Unplayable move: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        makeMove(&gs, &m);
        notation_printMoveCoordinate(&m, seqItems[count].move);
        seqItems[count].data = *gs.current;
        count++;
    }

    printHashSequence(seqItems, count, initialHash);
    destroyGamestate(&gs);
    free(seqItems);
}


static void printEndgame(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -classifyendgame \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    char* fen = argv[argc - 1];
    GameState gs = parseFenOrQuit(fen);
    int result = classifyEndgame(&gs);
    destroyGamestate(&gs);

    printEndgameClassification(result);
}

static void simpleSearch(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: -simplesearch \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    int depth = 7; // Configure default?
    const char* depthStr = findArg(argc, argv, "-depth");
    if (depthStr != NULL && !parseInteger(depthStr, &depth)) {
        exit(EXIT_FAILURE);
    }

    if (depth < 0) {
        fprintf(stderr, "Depth must be non-negative.\n");
        exit(EXIT_FAILURE);
    }

    char* fen = argv[argc - 1];
    GameState gs = parseFenOrQuit(fen);

    SearchArgs args;
    initSearchArgs(&args);
    args.depth = depth;

    SearchResult result;
    createSearchResult(&result);

    search(&gs, &args, &result);

    printSearchResult(&result, &gs);

    destroyGamestate(&gs);
    destroySearchResult(&result);
}

static void printMoveOrder(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -ordermoves \"[FEN string]\"\n");
        exit(EXIT_FAILURE);
    }

    GameState gs = parseFenOrQuit(argv[1]);

    MoveBuffer buffer;
    createMoveBuffer(&buffer);
    generateLegalMoves(&gs, &buffer);

    orderByMvvLva(&buffer);

    printMovelistJson(argv[1], "moveOrder", &gs, &buffer);

    destroyMoveBuffer(&buffer);
    destroyGamestate(&gs);
}

static void kingRect(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: -kingrect [square]\n");
        exit(EXIT_FAILURE);
    }

    if(strlen(argv[1]) != 2) {
        fprintf(stderr, "Invalid square: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    const int32_t file = parseFileChar(argv[1][0]);
    const int32_t rank = parseRankChar(argv[1][1]);

    if(file == INVALID_FILE || rank == INVALID_RANK) {
        fprintf(stderr, "Invalid square: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    const int32_t sqIndex = B_IDX(file, rank);

    const int32_t rectSize = countKingRectangleSize(sqIndex);

    printKingRectSize(argv[1], rectSize);
}

int main(int argc, char** argv) {
    argc--;
    argv++;

    // Randomness needn't be cryptographic strength for our purposes.
    srand((unsigned int) time(NULL));

    if (argc >= 1) {
        if (0 == strcmp("-listmoves", argv[0])) {
            listMoves(argc, argv);
        } else if (0 == strcmp("-printstate", argv[0])) {
            printState(argc, argv);
        } else if (0 == strcmp("-listattacks", argv[0])) {
            listAttacks(argc, argv);
        } else if (0 == strcmp("-makemove", argv[0])) {
            printMoveResult(argc, argv);
        } else if (0 == strcmp("-nullmove", argv[0])) {
            printNullMoveResult(argc, argv);
        } else if (0 == strcmp("-checkstatus", argv[0])) {
            checkStatus(argc, argv);
        } else if (0 == strcmp("-makeunmake", argv[0])) {
            printMakeUnmakeMoveResult(argc, argv);
        } else if (0 == strcmp("-matchmove", argv[0])) {
            printMatchMove(argc, argv);
        } else if (0 == strcmp("-gamestatus", argv[0])) {
            makeMovesAndPrintGameResultStatus(argc, argv);
        } else if (0 == strcmp("-hashseq", argv[0])) {
            hashSequence(argc, argv);
        } else if (0 == strcmp("-bookmoves", argv[0])) {
            findBookMoves(argc, argv);
        } else if (0 == strcmp("-evalposition", argv[0])) {
            evalPosition(argc, argv);
        } else if (0 == strcmp("-simplesearch", argv[0])) {
            simpleSearch(argc, argv);
        } else if (0 == strcmp("-ordermoves", argv[0])) {
            printMoveOrder(argc, argv);
        } else if (0 == strcmp("-classifyendgame", argv[0])) {
            printEndgame(argc, argv);
        } else if (0 == strcmp("-passedpawns", argv[0])) {
            checkPassedPawn(argc, argv);
        } else if (0 == strcmp("-interactive", argv[0])) {
          startInteractive();
        } else if (0 == strcmp("-kingrect", argv[0])) {
            kingRect(argc, argv);
        } else {
            printBanner();
            printf("Unknown command \"%s\"\n", argv[0]);
        }
    } else {
        printBanner();
        printf(">> No operation mode specified, entering XBoard mode.\n");
        startXBoard();
    }

    return EXIT_SUCCESS;
}
