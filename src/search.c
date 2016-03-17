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
#include "xboard.h"
#include "hash.h"

static int32_t compareMoveScore(const void* a, const void* b) {
	return ((MoveScore*) b)->score - ((MoveScore*) a)->score; // Underflow issues? Hopefully our scores are on the order of 1e5...
}

static int32_t compareMvvLva(const void* a, const void* b) {
	Move* aMove = (Move*) a;
	Move* bMove = (Move*) b;

	int32_t result;

	// First order by captured piece, most valuable first.
	result = (bMove->captures->relativeValue) - (aMove->captures->relativeValue);
	if (result == 0) {
		// Next order by moving piece, least valuable first.
		result = (aMove->movingPiece->relativeValue) - (bMove->movingPiece->relativeValue);
	}

	return result;
}

static void sortMoveScores(MoveScore* scores, const int32_t length) {
	qsort(scores, (size_t) length, sizeof(MoveScore), compareMoveScore);
}

void orderByMvvLva(MoveBuffer* buffer) {
	qsort(buffer->moves, ((size_t) buffer->length), sizeof(Move), compareMvvLva);
}

void initSearchArgs(SearchArgs* args) {
	args->log = NULL;
	args->depth = 5;
	args->chessInterfaceState = NULL;
}

static void storeHash(GameState* state, int32_t score, int32_t depth) {
	// Make sure we store the hash in an absolute score (>0 good for white)
	const int32_t multiplier = state->current->toMove == COLOR_WHITE ? 1 : -1;
	hash_put(state, score * multiplier, depth);
}

static int32_t alphaBeta(GameState* state, SearchResult* result, const int32_t depth, const int32_t maxDepth,
                         int32_t alpha, int32_t beta, bool allowNullMove) {
	result->nodes++;

	if (depth >= maxDepth) {
		const int32_t evalScore = evaluate(state);
		// Experimental - Don't store the leaf node score here, since those aren't terribly valuable (very low depth).
		// storeHash(state, evalScore, depth);
		return evalScore;
	}

	const int32_t storedScore = hash_probe(state, depth);
	if (storedScore != HASH_NOT_FOUND) {
		// Hash scores are stored in absolute values (>0 good for white)
		const int32_t multiplier = state->current->toMove == COLOR_WHITE ? 1 : -1;
		return multiplier * storedScore;
	}

	const bool check = isCheck(state);

	// Apply the null-move heuristic if the situation warrants.
	//
	// Doing null-move in check is *not* a good idea.
	// Null move relies on letting the opponent move twice being the worst possible thing.
	// Forcing them to move out of a checking position, however, is good.
	if (allowNullMove && !check) {
		makeNullMove(state);
		const int32_t nullScore = alphaBeta(state, result, depth + 1 + NULL_MOVE_RADIUS, maxDepth, -beta, -beta + 1, false);
		unmakeNullMove(state);

		if (nullScore > beta) {
			return beta;
		}
	}

	MoveBuffer* buffer = &state->moveBuffers[depth];
	const int32_t moveCount = generatePseudoMoves(state, buffer);

	// If we're early in the search, sort the moves a bit nicer.
	if (depth < 3) {
		orderByMvvLva(buffer);
	}

	bool noLegalMoves = true;

	for (int32_t i = 0; i < moveCount; i++) {
		Move m = buffer->moves[i];

		makeMove(state, &m);

		if (isLegalPosition(state)) {
			noLegalMoves = false;
			const int32_t moveScore =  -1 * alphaBeta(state, result, depth + 1, maxDepth, -1 * beta, -1 * alpha, allowNullMove);
			unmakeMove(state, &m);

			if (moveScore >= beta) {
				result->betaCutoffs++;
				return beta;
			}

			if (moveScore > alpha) {
				alpha = moveScore;
			}
		} else {
			unmakeMove(state, &m);
		}
	}

	if (noLegalMoves) {
		// No legal moves and check? Checkmate. Else, stalemate.
		// Add the search depth to encourage "faster" checkmates; so longer checkmates are worth slightly less.
		return check ? -INFINITY + depth : 0;
	}

	storeHash(state, alpha, depth);

	return alpha;
}

static void logMoveScoreList(GameLog* log, GameState* state, MoveScore* scores, int32_t size) {
	const size_t buffSize = 2048;
	char* buff = calloc(buffSize, sizeof(char));
	uint32_t pos = 0;
	char moveStr[8];
	char scoreStr[16];
	buff[pos++] = '[';
	for (int32_t i = 0; i < size; i++) {
		printShortAlg(&scores[i].move, state, moveStr);
		snprintf(scoreStr, 16, "%s: %+.2f", moveStr, (double) scores[i].score / 100.0);
		for (int32_t j = 0; scoreStr[j] && j < 16; j++) {
			buff[pos++] = scoreStr[j];
		}
		if (i != size - 1) {
			buff[pos++] = ',';
			buff[pos++] = ' ';
		}
	}
	buff[pos++] = ']';
	buff[pos++] = '\0';

	log_write(log, buff);
	free(buff);
}

static void iterativeDeepen(GameState* state, SearchResult* result, MoveScore* moveScores, MoveBuffer* legalMoves, int32_t maxDepth, GameLog* log) {
	for (int32_t i = 0; i < legalMoves->length; i++) {
		Move m = legalMoves->moves[i];

		makeMove(state, &m);
		const int32_t score = -1 * alphaBeta(state, result, 0, maxDepth, -1 * INFINITY, INFINITY, true);
		unmakeMove(state, &m);

		moveScores[i].move = m;
		moveScores[i].score = score;
		moveScores[i].depth = maxDepth;
	}

	sortMoveScores(moveScores, legalMoves->length);

	result->score = moveScores[0].score;
	result->move = moveScores[0].move;

	log_write(log, "Completed iterative deepening to depth=%i", maxDepth);
	logMoveScoreList(log, state, moveScores, legalMoves->length);
}

static void reorderMovesFromMoveScores(MoveScore* scores, int32_t scoreLength, MoveBuffer* moveBuffer) {
	if (scoreLength != moveBuffer->length) {
		fprintf(stderr, "Move score buffer length and move buffer length disagree.\n");
	}

	int32_t limit = scoreLength > moveBuffer->length ? scoreLength : moveBuffer->length;

	for (int32_t i = 0; i < limit; i++) {
		moveBuffer->moves[i] = scores[i].move;
	}
}

static bool isEarlyCheckmate(int32_t score) {
	return score >= (INFINITY - 1000);
}

// This is called once per search. It orders the moves at the root level thusly:
// 1. Checks and captures first. A move that is both should be first.
// 2. More valuable captured pieces before less valuable pieces.
static void orderRootNode(GameState* state, MoveBuffer* buffer) {
	const int32_t moveCount = buffer->length;
	MoveScore* scores = ALLOC((uint32_t) moveCount, MoveScore, scores, "Error allocating move scores in root node.");

	for (int32_t i = 0; i < moveCount; i++) {
		Move* m = &buffer->moves[i];
		int32_t score = 0;

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

	for (int32_t i = 0; i < moveCount; i++) {
		buffer->moves[i] = scores[i].move;
	}

	free(scores);
}

static void logSearchResult(SearchArgs* args, SearchResult* result, GameState* state) {
	const int64_t nodes = result->nodes;
	const int64_t duration = result->durationMs;
	const double knodes = ((double) nodes) / 1000.0;
	const double seconds = ((double) duration) / 1000.0;
	const double score = friendlyScore(state, result->score);
	const double betaPct = ((double) result->betaCutoffs) / (double) result->nodes;

	log_write(args->log, "Search complete. Score %+.2f; %ld nodes in %ldms (%.2f KNps)", score, nodes, duration, knodes / seconds);
	log_write(args->log, "Beta cutoff in %i/%i of nodes (%.2f%%)", result->betaCutoffs, nodes, betaPct);
}

static void logIterativeResult(GameState* state, SearchArgs* searchArgs, MoveScore* scores, int32_t depth) {
	char moveStr[16];
	MoveScore top = scores[0];
	printShortAlg(&top.move, state, moveStr);
	double score = friendlyScore(state, top.score);
	log_write(searchArgs->log, "After depth=%i, best move is %s (%+0.2f)", depth, moveStr, score);
}

static void postSearchThinking(void* interState, GameState* state, int32_t depth, int32_t score, int64_t nodes, int64_t startTime, Move bestMove) {
	if (interState == NULL) {
		return;
	}

	const int64_t now = getCurrentTimeMillis();

	MoveBuffer mb;
	createMoveBuffer(&mb);
	mb.moves[0] = bestMove;
	mb.length = 1;
	const int32_t postScore = score * (state->current->toMove == COLOR_WHITE ? 1 : -1);
	postXBOutput(interState, depth, postScore, (now - startTime) / 10l, nodes, &mb);
	destroyMoveBuffer(&mb);
}

bool search(GameState* state, SearchArgs* searchArgs, SearchResult* result) {
	MoveBuffer buffer;
	result->searchStatus = SEARCH_STATUS_NONE;
	createMoveBuffer(&buffer);

	log_write(searchArgs->log, "Search starting with depth=%i", searchArgs->depth);

	const int64_t start = getCurrentTimeMillis();
	const int32_t moveCount = generateLegalMoves(state, &buffer);

	// Put captures and checks at the top.
	orderRootNode(state, &buffer);

	result->moveScoreLength = moveCount;
	result->score = INT_MIN;
	result->nodes = 0;
	result->betaCutoffs = 0;
	MoveScore* scores = result->moveScores;

	if (moveCount) {
		for (int32_t depth = 1; depth <= searchArgs->depth; depth++) {
			iterativeDeepen(state, result, scores, &buffer, depth, searchArgs->log);

			// After each iteration, reorder the move search order "best moves first."
			// This speeds up successive searches by creating beta cutoffs faster.
			reorderMovesFromMoveScores(scores, result->moveScoreLength, &buffer);

			logIterativeResult(state, searchArgs, scores, depth);

			postSearchThinking(searchArgs->chessInterfaceState, state, depth, scores[0].score, result->nodes, start, scores[0].move);

			// If we found a checkmate, just play that immediately. No need to deepen further!
			if (isEarlyCheckmate(scores[0].score)) {
				log_write(searchArgs->log, "Early checkmate found at depth %i", depth);
				break;
			}
		}
	} else {
		result->searchStatus = SEARCH_STATUS_NO_LEGAL_MOVES;
	}

	const int64_t end = getCurrentTimeMillis();

	result->durationMs = end - start;

	if (searchArgs->log != NULL) {
		logSearchResult(searchArgs, result, state);
	}

	postSearchThinking(searchArgs->chessInterfaceState, state, searchArgs->depth, result->score, result->nodes, start, result->move);

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
