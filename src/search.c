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

#include <limits.h>
#include <time.h>
#include <stdio.h>

#include "tulip.h"
#include "search.h"
#include "gamestate.h"
#include "move.h"
#include "movegen.h"
#include "makemove.h"
#include "eval.h"
#include "attack.h"
#include "result.h"
#include "util.h"
#include "log.h"

static int compareMoveScore(const void* a, const void* b) {
    return ((MoveScore*) b)->score - ((MoveScore*) a)->score; // Underflow issues? Hopefully our scores are on the order of 1e5...
}

static int compareMvvLva(const void* a, const void* b) {
    Move* aMove = (Move*) a;
    Move* bMove = (Move*) b;

    int result;

    // First order by captured piece, most valuable first.
    result = (bMove->captures->relativeValue) - (aMove->captures->relativeValue);
    if (result == 0) {
        // Next order by moving piece, least valuable first.
        result = (aMove->movingPiece->relativeValue) - (bMove->movingPiece->relativeValue);
    }

    return result;
}

static void sortMoveScores(MoveScore* scores, const int length) {
    qsort(scores, (size_t) length, sizeof(MoveScore), compareMoveScore);
}

void orderByMvvLva(MoveBuffer* buffer) {
    qsort(buffer->moves, ((size_t) buffer->length), sizeof(Move), compareMvvLva);
}

void initSearchArgs(SearchArgs* args) {
    args->log = NULL;
    args->depth = 5;
}

static int alphaBeta(GameState* state, SearchResult* result, const int depth, const int maxDepth, int alpha, int beta, bool allowNullMove) {
    result->nodes++;

    if (depth >= maxDepth) {
        return evaluate(state);
    }

    // Apply the null move heuristic.
    // Do not allow subsequent nodes in the move tree to apply the null move.
    makeNullMove(state);
    const int nullScore = alphaBeta(state, result, depth + 1 + NULL_MOVE_RADIUS, maxDepth, -beta, -beta + 1, false);
    unmakeNullMove(state);

    if (nullScore > beta) {
        return beta;
    }

    MoveBuffer* buffer = &state->moveBuffers[depth];
    const int moveCount = generatePseudoMoves(state, buffer);

    // If we're early in the search, sort the moves a bit nicer.
    if (depth < 3) {
        orderByMvvLva(buffer);
    }

    int legalMoveCount = 0;

    for (int i = 0; i < moveCount; i++) {
        Move m = buffer->moves[i];

        makeMove(state, &m);

        if (isLegalPosition(state)) {
            legalMoveCount++;
            const int moveScore =  -1 * alphaBeta(state, result, depth + 1, maxDepth, -1 * beta, -1 * alpha, allowNullMove);
            unmakeMove(state, &m);

            if (moveScore >= beta) {
                return beta;
            }

            if (moveScore > alpha) {
                alpha = moveScore;
            }
        } else {
            unmakeMove(state, &m);
        }
    }

    if (legalMoveCount == 0) {
        if (isCheck(state)) {
            return -INFINITY + depth; // Add depth to encourage "faster" checkmates.
        } else {
            return 0; // Stalemate
        }
    }

    return alpha;
}

void iterativeDeepen(GameState* state, SearchResult* result, MoveScore* moveScores, MoveBuffer* legalMoves, int maxDepth) {
    for (int i = 0; i < legalMoves->length; i++) {
        Move m = legalMoves->moves[i];

        makeMove(state, &m);
        const int score = -1 * alphaBeta(state, result, 0, maxDepth, -1 * INFINITY, INFINITY, true);
        unmakeMove(state, &m);

        moveScores[i].move = m;
        moveScores[i].score = score;
        moveScores[i].depth = maxDepth;

        if (score > result->score) {
            result->score = score;
            result->move = m;
        }
    }

    sortMoveScores(moveScores, legalMoves->length);
}

static void reorderMovesFromMoveScores(MoveScore* scores, int scoreLength, MoveBuffer* moveBuffer) {
    if (scoreLength != moveBuffer->length) {
        fprintf(stderr, "Move score buffer length and move buffer length disagree.\n");
    }

    int limit = scoreLength > moveBuffer->length ? scoreLength : moveBuffer->length;

    for (int i = 0; i < limit; i++) {
        moveBuffer->moves[i] = scores[i].move;
    }
}

static bool isEarlyCheckmate(int score) {
    return score >= (INFINITY - 1000);
}

// This is called once per search. It orders the moves at the root level thusly:
// 1. Checks and captures first. A move that is both should be first.
// 2. More valuable captured pieces before less valuable pieces.
static void orderRootNode(GameState* state, MoveBuffer* buffer) {
    const int moveCount = buffer->length;
    MoveScore* scores = ALLOC((unsigned int) moveCount, MoveScore, scores, "Error allocating move scores in root node.");

    for (int i = 0; i < moveCount; i++) {
        Move* m = &buffer->moves[i];
        int score = 0;

        // Loosely sort by captured piece value
        if (m->captures != &EMPTY) {
            score += m->captures->relativeValue;
        }

        // Add special bonus to checks.
        makeMove(state, m);
        if (isCheck(state)) {
            score += 10; // Arbitrary value; high enough to put checks before captures.
        }
        unmakeMove(state, m);

        scores[i].move = *m;
        scores[i].score = score;
    }

    sortMoveScores(scores, moveCount);

    for (int i = 0; i < moveCount; i++) {
        buffer->moves[i] = scores[i].move;
    }

    free(scores);
}

static void logSearchStart(SearchArgs* args) {
    const int logBuffSize = 1024;
    char* logBuff = malloc(logBuffSize * sizeof(char));
    if (!logBuff) {
        perror("Unable to allocate log buffer.");
        exit(-1);
    }

    snprintf(logBuff, 1024, "Root search starting; depth=%i", args->depth);
    writeEntry(args->log, logBuff);
    free(logBuff);
}

static void logSearchResult(SearchArgs* args, SearchResult* result, GameState* state) {
    const int logBuffSize = 1024;
    char* logBuff = malloc(logBuffSize * sizeof(char));
    if (!logBuff) {
        perror("Unable to allocate log buffer.");
        exit(-1);
    }

    const long nodes = result->nodes;
    const long duration = result->durationMs;
    const double knodes = ((double) nodes) / 1000.0;
    const double seconds = ((double) duration) / 1000.0;
    const int scoreMult = state->current->toMove == COLOR_BLACK ? -1 : 1;
    const int score = result->score * scoreMult;

    snprintf(logBuff, logBuffSize, "Search complete. Score %+.2f; %ld nodes in %ldms (%.2f KNps)", (double) score / 100.0, nodes, duration, knodes / seconds);
    writeEntry(args->log, logBuff);
    free(logBuff);
}

bool search(GameState* state, SearchArgs* searchArgs, SearchResult* result) {
    MoveBuffer buffer;
    result->searchStatus = SEARCH_STATUS_NONE;
    createMoveBuffer(&buffer);

    logSearchStart(searchArgs);

    const long start = getCurrentTimeMillis();
    const int moveCount = generateLegalMoves(state, &buffer);

    // Put captures and checks at the top.
    orderRootNode(state, &buffer);

    result->moveScoreLength = moveCount;
    result->score = INT_MIN;
    result->nodes = 0;
    MoveScore* scores = result->moveScores;

    if (moveCount) {
        for (int depth = 1; depth <= searchArgs->depth; depth++) {
            iterativeDeepen(state, result, scores, &buffer, depth);

            // After each iteration, reorder the move search order "best moves first."
            // This speeds up successive searches by creating beta cutoffs faster.
            reorderMovesFromMoveScores(scores, result->moveScoreLength, &buffer);

            // If we found a checkmate, just play that immediately. No need to deepen further!
            if (isEarlyCheckmate(scores[0].score)) {
                break;
            }
        }
    } else {
        result->searchStatus = SEARCH_STATUS_NO_LEGAL_MOVES;
    }

    const long end = getCurrentTimeMillis();

    result->durationMs = end - start;

    if (searchArgs->log != NULL) {
        logSearchResult(searchArgs, result, state);
    }

    destroyMoveBuffer(&buffer);
    return true;
}



void createSearchResult(SearchResult* result) {
    result->searchStatus = SEARCH_STATUS_NONE;
    result->score = INT_MIN;
    result->nodes = 0;
    result->moveScores = ALLOC(MOVE_BUFFER_LENGTH, MoveScore, result->moveScores, "Error allocating move score array.");
    result->moveScoreLength = 0;
}

void destroySearchResult(SearchResult* result) {
    free(result->moveScores);
}
