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

static int compareMoveScore(const void* a, const void* b) {
    return ((MoveScore*) b)->score - ((MoveScore*) a)->score; // Underflow issues? Hopefully our scores are on the order of 1e5...
}

static void sortMoveScores(MoveScore* scores, const int length) {
    qsort(scores, (size_t) length, sizeof(MoveScore), compareMoveScore);
}

static int alphaBeta(GameState* state, SearchResult* result, const int depth, const int maxDepth, int alpha, int beta) {
    result->nodes++;

    if (depth >= maxDepth) {
        return evaluate(state);
    }

    MoveBuffer* buffer = &state->moveBuffers[depth];
    const int moveCount = generatePseudoMoves(state, buffer);
    int legalMoveCount = 0;

    for (int i = 0; i < moveCount; i++) {
        Move m = buffer->moves[i];

        makeMove(state, &m);

        if (isLegalPosition(state)) {
            legalMoveCount++;
            const int moveScore =  -1 * alphaBeta(state, result, depth + 1, maxDepth, -1 * beta, -1 * alpha);
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
        const int score = -1 * alphaBeta(state, result, 0, maxDepth, -1 * INFINITY, INFINITY);
        unmakeMove(state, &m);

        moveScores[i].move = m;
        moveScores[i].score = score;

        if (score > result->score) {
            result->score = score;
            result->move = m;
        }
    }

    sortMoveScores(moveScores, legalMoves->length);
}

bool search(GameState* state, SearchArgs* searchArgs, SearchResult* result) {
    MoveBuffer buffer;
    result->searchStatus = SEARCH_STATUS_NONE;
    createMoveBuffer(&buffer);

    const long start = getCurrentTimeMillis();

    int moveCount = generateLegalMoves(state, &buffer);
    result->moveScoreLength = moveCount;
    result->score = INT_MIN;
    result->nodes = 0;
    if (moveCount) {
        const int depth = searchArgs->depth < 0 ? 0 : searchArgs->depth;
        // Well, we're not actually iteratively deepening yet...
        iterativeDeepen(state, result, result->moveScores, &buffer, depth);
    } else {
        result->move = NULL_MOVE;
        result->searchStatus = SEARCH_STATUS_NO_LEGAL_MOVES;
    }

    const long end = getCurrentTimeMillis();

    result->durationMs = end - start;

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

