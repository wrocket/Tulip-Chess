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

#ifndef SEARCH_H
#define SEARCH_H

#include "move.h"
#include "gamestate.h"
#include "notation.h"

#define INFINITY 10000

#define SEARCH_STATUS_NONE              0
#define SEARCH_STATUS_NO_LEGAL_MOVES    1

// Used for sorting moves by their score.
typedef struct {
    Move move;
    int score;
    int depth;
} MoveScore;

// Structure to define the output of the search method.
typedef struct {
    int score;              // The current game score, from the perspective of the side to move.
    Move move;              // The best move.
    long durationMs;        // The search duration.
    long nodes;             // The number of positions processed.
    int searchStatus;       // SEARCH_STATUS_NO_LEGAL_MOVES if no moves could be considered.
    MoveScore* moveScores;  // A list of the moves considered and their scores.
    int moveScoreLength;    // The length of moveScores.
} SearchResult;

typedef struct {
    int depth;      // The maximum search depth.
} SearchArgs;

// Allocate resources for a search result.
void createSearchResult(SearchResult* result);

// Release resources for a search result.
void destroySearchResult(SearchResult* result);

// Think. Figure out the best move. This is a blocking operation at the moment.
bool search(GameState* state, SearchArgs* searchArgs, SearchResult* result);

// Do a zero-depth move ordering, first by most valuable victim (Mvv) and then by least valuable attacker (lva).
void orderByMvvLva(MoveBuffer* buffer);
#endif
