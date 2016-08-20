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
#include "draw.h"

#define USE_Q_SEARCH false

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
	args->timeToThinkMillis = 5 * 1000;
}

// Perform a "quiet" search, which basically means keep playing captures and whatnot until a "quiet" position is reached.
// This combats the horizon effect where nasty moves (captures, checks) lurk one ply beyond the max search depth.
static int32_t qsearch(GameState* state, SearchResult* result, const int32_t depth, int32_t maxDepth, int32_t alpha, int32_t beta) {
	if (draw_isThreefold(state) || draw_isMaterial(state)) {
		return 0;
	}

	const int32_t evalScore = evaluate(state);

	// Hmm...use hash table?
	if (depth > maxDepth) {
		return evalScore;
	}

	if (evalScore >= beta) {
		return beta;
	}

	if (evalScore > alpha) {
		alpha = evalScore;
	}

	MoveBuffer* buffer = &state->moveBuffers[depth];
	const int32_t moveCount = generatePseudoMoves(state, buffer);

	for (int32_t i = 0; i < moveCount; i++) {
		Move* m = &buffer->moves[i];

		makeMove(state, m);

		// Only search moves that:
		// 1. Are legal.
		// 2. Result in a check state OR are captures.
		if (!isLegalPosition(state) || !isCheck(state) || m->captures == &EMPTY) {
			unmakeMove(state, m);
		} else {
			const int32_t moveScore =  -1 * qsearch(state, result, depth + 1, maxDepth, -1 * beta, -1 * alpha);
			unmakeMove(state, m);

			if (moveScore >= beta) {
				result->betaCutoffs++;
				return beta;
			}

			if (moveScore > alpha) {
				alpha = moveScore;
			}
		}
	}

	return alpha;
}

static int32_t alphaBeta(GameState* state, SearchResult* result, const int32_t depth, const int32_t maxDepth,
                         int32_t alpha, int32_t beta, bool allowNullMove) {
	result->nodes++;

	int32_t hashf = HASHF_ALPHA;

	if (draw_isThreefold(state) || draw_isMaterial(state)) {
		return 0;
	}

	if (depth >= maxDepth) {
		const int32_t evalScore = USE_Q_SEARCH ? qsearch(state, result, depth, Q_SEARCH_DEPTH, alpha, beta) :  evaluate(state);
		if (allowNullMove) {
			hash_put(state, evalScore, depth, HASHF_EXACT);
		}
		return evalScore;
	}

	const int32_t storedScore = hash_probe(state, depth, alpha, beta);
	if (storedScore != HASH_NOT_FOUND) {
		return storedScore;
	}

	const bool check = isCheck(state);

	// Apply the null-move heuristic if the situation warrants.
	//
	// Doing null-move in check is *not* a good idea.
	// Null move relies on letting the opponent move twice being the worst possible thing.
	// Forcing them to move out of a checking position, however, is good.
	if (allowNullMove && !check) {
		makeNullMove(state);
		const int32_t nullScore = -1 * alphaBeta(state, result, depth + 1 + NULL_MOVE_RADIUS, maxDepth, -beta, -beta + 1, false);
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
		Move* m = &buffer->moves[i];

		makeMove(state, m);

		if (isLegalPosition(state)) {
			noLegalMoves = false;
			const int32_t moveScore =  -1 * alphaBeta(state, result, depth + 1, maxDepth, -1 * beta, -1 * alpha, allowNullMove);
			unmakeMove(state, m);

			if (moveScore >= beta) {
				result->betaCutoffs++;
				if (allowNullMove) {
					hash_put(state, beta, depth, HASHF_BETA);
				}
				return beta;
			}

			if (moveScore > alpha) {
				hashf = HASHF_EXACT;
				alpha = moveScore;
			}
		} else {
			unmakeMove(state, m);
		}
	}

	if (noLegalMoves) {
		// No legal moves and check? Checkmate. Else, stalemate.
		// Add the search depth to encourage "faster" checkmates; so longer checkmates are worth slightly less.
		const int32_t score = check ? -INFINITY + depth : 0;
		hash_put(state, score, 0, HASHF_EXACT); // We know exactly what the score is here, searching deeper doesn't change it.
		return score;
	}

	hash_put(state, alpha, depth, hashf);

	return alpha;
}

static void logMoveScoreList(GameLog* log, GameState* state, MoveScore* scores, int32_t size) {
	const size_t buffSize = 2048;
	char* buff = calloc(buffSize, sizeof(char));
	uint32_t pos = 0;
	char moveStr[8];
	char scoreStr[16];
	buff[pos++] = '[';
	// TODO: Better checking to ensure we don't exceed buff's size.
	for (int32_t i = 0; i < size; i++) {
		notation_printShortAlg(&scores[i].move, state, moveStr);
		snprintf(scoreStr, 16, "%s: %+.2f", moveStr, (double) scores[i].score / 100.0);

		// TODO: memcopy() is probably better here.
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

static bool outOfTime(SearchArgs* args, int64_t start) {
	const int64_t span = getCurrentTimeMillis() - start;
	const bool result = span > args->timeToThinkMillis;

	if (result) {
		log_write(args->log, "Out of time (%0.2fs > %0.2fs)", (double) span / 1000.0, (double) args->timeToThinkMillis / 1000.0);
	}

	return result;
}

// Performs the "deep" search out to a given depth (plus any extensions).
static void deepSearch(GameState* state, SearchArgs* searchArgs, SearchResult* result, MoveScore* moveScores, int32_t moveLen, int32_t maxDepth, GameLog* log, int64_t startTime) {
	log_write(log, "Starting deep search with depth=%i", maxDepth);
	int32_t alpha = -INFINITY;
	int32_t beta = INFINITY;
	MoveScore best;
	best.score = INT_MIN;
	char moveStr[16];
	for (int32_t i = 0; i < moveLen; i++) {
		Move m = moveScores[i].move;

		makeMove(state, &m);
		const int32_t score = -1 * alphaBeta(state, result, 0, maxDepth, -beta, -alpha, true);
		unmakeMove(state, &m);

		if (score > alpha) {
			alpha = score;
		}

		if (score > best.score) {
			notation_printShortAlg(&m, state, moveStr);
			log_write(log, "Improved score in deep search: %s, %+0.2f", moveStr, friendlyScore(state, score));
			postSearchThinking(searchArgs->chessInterfaceState, state, maxDepth, score, result->nodes, startTime, m);
			best.score = score;
			best.move = m;
		}

		if (outOfTime(searchArgs, startTime)) {
			break;
		}
	}

	result->score = best.score;
	result->move = best.move;

	log_write(log, "Completed deep search to depth=%i", maxDepth);
}

// Run a search to a given depth, assuming no previous knowledge.
static void iterativeDeepen(
    GameState* state,
    SearchResult* result,
    MoveScore* moveScores,
    MoveBuffer* legalMoves,
    int32_t maxDepth,
    int64_t startTime,
    SearchArgs* args) {
	for (int32_t i = 0; i < legalMoves->length; i++) {
		Move m = legalMoves->moves[i];

		makeMove(state, &m);
		const int32_t score = -1 * alphaBeta(state, result, 0, maxDepth, -INFINITY, INFINITY, true);
		unmakeMove(state, &m);

		// We don't update alpha/beta here because doing so can lead to misleading elements in the move score list.
		// For example, if we find a checkmate, every score past that will end up as checkmate since technically
		// they are part of the same position.
		moveScores[i].move = m;
		moveScores[i].score = score;
		moveScores[i].depth = maxDepth;

		// We want to make sure at least one full pass of this function is completed.
		// Otherwise we'll have incompletely initiaized data in moveScores.
		// Ignore the clock if we're at depth 0.
		if (maxDepth > 1 && outOfTime(args, startTime)) {
			break;
		}
	}

	sortMoveScores(moveScores, legalMoves->length);

	result->score = moveScores[0].score;
	result->move = moveScores[0].move;

	log_write(args->log, "Completed iterative deepening to depth=%i", maxDepth);
	logMoveScoreList(args->log, state, moveScores, legalMoves->length);
}

// Reorders a move buffer according to a sorted list of move scores.
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
	notation_printShortAlg(&top.move, state, moveStr);
	double score = friendlyScore(state, top.score);
	log_write(searchArgs->log, "After depth=%i, best move is %s (%+0.2f)", depth, moveStr, score);
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

	// Do successively deeper searches out to a reasonably shallow depth, sorting the moves by score after each search.
	// This accomplishes two things:
	// 1. Orders the moves according to best first, which increases the speed of subsequent searches.
	// 2. Ensures that we at least have a "decent" move in a given time frame, since the first few iterations should only take a few millis.
	const int32_t iterationDeepenDepth = searchArgs->depth > ITERATIVE_DEEPEN_DEPTH ? ITERATIVE_DEEPEN_DEPTH : searchArgs -> depth;
	if (moveCount) {
		bool doDeepSearch = true;
		for (int32_t depth = 1; depth <= iterationDeepenDepth; depth++) {
			iterativeDeepen(state, result, scores, &buffer, depth, start, searchArgs);

			// After each iteration, reorder the move search order "best moves first."
			// This speeds up successive searches by creating beta cutoffs faster.
			reorderMovesFromMoveScores(scores, result->moveScoreLength, &buffer);

			logIterativeResult(state, searchArgs, scores, depth);

			postSearchThinking(searchArgs->chessInterfaceState, state, depth, scores[0].score, result->nodes, start, scores[0].move);

			// If we found a checkmate, just play that immediately. No need to deepen further!
			if (isEarlyCheckmate(scores[0].score)) {
				doDeepSearch = false;
				log_write(searchArgs->log, "Early checkmate found at depth %i", depth);
				break;
			}

			// If we're out of time, just go with whatever we have now.
			if (outOfTime(searchArgs, start)) {
				doDeepSearch = false;
				break;
			}
		}

		// With the moves nicely ordered, start the deep search that might take a while.
		if (doDeepSearch) {
			deepSearch(state, searchArgs, result, scores, buffer.length, DEEP_SEARCH_DEPTH, searchArgs->log, start);
		}
	} else {
		result->searchStatus = SEARCH_STATUS_NO_LEGAL_MOVES;
	}

	const int64_t end = getCurrentTimeMillis();

	result->durationMs = end - start;

	logSearchResult(searchArgs, result, state);

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
