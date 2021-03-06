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
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "util.h"
#include "xboard.h"
#include "gamestate.h"
#include "fen.h"
#include "notation.h"
#include "makemove.h"
#include "book.h"
#include "move.h"
#include "search.h"
#include "log.h"
#include "result.h"
#include "time.h"
#include "env.h"

static void xBoardWrite(XBoardState* xbs, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	vsprintf(xbs->outputBuffer, format, argptr);
	va_end(argptr);

	log_write(&xbs->log, "<<\t%s", xbs->outputBuffer); // Write to the log.
	printf("%s\n", xbs->outputBuffer); // Write to stdout.
	fflush(stdout);
}

static void logInput(XBoardState* xbs, char* message) {
	log_write(&xbs->log, ">>\t%s", message);
}

static void logGameState(XBoardState* xbs) {
	char* outputMessage;
	outputMessage = malloc(1024);
	if (!outputMessage) {
		printf("Error: Unable to allocate log message buffer.\n");
		exit(-1);
	}

	printFen(&xbs->gameState, outputMessage, 1024);
	log_write(&xbs->log, "Current game state: %s", outputMessage);

	free(outputMessage);
}

static void xBoardNew(XBoardState* xbs) {
	if (!parseFenWithPrint(&xbs->gameState, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", true)) {
		xBoardWrite(xbs, "Error: Unable to set initial board position; invalid FEN. (?!?!)");
	}

	xbs->forceMode = false;
}

static void xBoardForce(XBoardState* xbs) {
	xbs->forceMode = true;
}

static void xBoardApplyMove(XBoardState* xbs, Move* move) {
	makeMove(&xbs->gameState, move);
}

static bool xBoardThinkAndMove(XBoardState* xbs) {
	MoveBuffer mb;
	char moveStr[16];
	bool foundMove = false;
	Move move;

	// First check the book for this position.
	if (xbs->bookOpen) {
		createMoveBuffer(&mb);
		const int bookMoveCount = book_getMoves(&xbs->gameState, &mb, &xbs->currentBook);
		if (bookMoveCount > 0) {
			foundMove = true;
			move = mb.moves[rand() % bookMoveCount];
			log_write(&xbs->log, "Found move from book.");
		}
		destroyMoveBuffer(&mb);
	} else {
		log_write(&xbs->log, "Book not open, proceeding to search.");
	}

	// If nothing was found in the book, perform a search.
	if (!foundMove) {
		SearchResult searchResult;
		SearchArgs args;
		initSearchArgs(&args);
		args.depth = 10;
		args.log = &xbs->log;
		args.timeToThinkMillis = time_thinkTimeMillis(&xbs->gameState, xbs->myTime, xbs->opponentTime);
		args.chessInterfaceState = (void*) xbs;
		createSearchResult(&searchResult);

		log_write(&xbs->log, "Allocating %.2fs of think time.", (double) args.timeToThinkMillis / 1000.0);

		search(&xbs->gameState, &args, &searchResult);

		if (searchResult.searchStatus != SEARCH_STATUS_NO_LEGAL_MOVES) {
			move = searchResult.move;
			foundMove = true;
		}

		destroySearchResult(&searchResult);
	}

	if (foundMove) {
		notation_printShortAlg(&move, &xbs->gameState, moveStr);
		xBoardWrite(xbs, "move %s", moveStr);
		xBoardApplyMove(xbs, &move);
		logGameState(xbs);
	}

	return foundMove;
}

static void printResult(XBoardState* xbs, int32_t result) {
	switch (result) {
	case STATUS_MATERIAL_DRAW:
		xBoardWrite(xbs, "1/2-1/2 {Draw by material.}");
		break;
	case STATUS_STALEMATE:
		xBoardWrite(xbs, "1/2-1/2 {Stalemate.}");
		break;
	case STATUS_THREEFOLD_DRAW:
		xBoardWrite(xbs, "1/2-1/2 {Draw by threefold repitition.");
		break;
	case STATUS_WHITE_CHECKMATED:
		xBoardWrite(xbs, "0-1 {White checkmated.}");
		break;
	case STATUS_BLACK_CHECKMATED:
		xBoardWrite(xbs, "1-0 {Black checkmated.}");
		break;
	}
}

static void xBoardGo(XBoardState* xbs) {
	xbs->forceMode = false;
	xBoardThinkAndMove(xbs);
	const int32_t result = getResult(&xbs->gameState);
	if (result != STATUS_NONE) {
		printResult(xbs, result);
	}
}

static void xBoardMove(XBoardState* xbs, char* move) {
	Move m;

	if (notation_matchMove(move, &xbs->gameState, &m)) {
		makeMove(&xbs->gameState, &m);
		logGameState(xbs);

		if (!xbs->forceMode) {
			const int32_t result = getResult(&xbs->gameState);
			if (result == STATUS_NONE) {
				xBoardThinkAndMove(xbs);
			} else {
				printResult(xbs, result);
			}
		}
	} else {
		xBoardWrite(xbs, "Illegal move: %s", move);
	}
}

static void xBoardProtover(XBoardState* xbs) {
	xBoardWrite(xbs, "feature ping=1 san=1 time=1 sigint=0 sigterm=0 setboard=1 done=1");
}

static void xBoardIcs(XBoardState* xbs, char** tokens, int tokenCount) {
	if (tokenCount >= 2) {
		// An ICS of - means we're local
		if (strcmp("-", tokens[1]) != 0) {
			xbs->onIcs = true;

			const size_t buffSize = 256;
			char* buff;
			buff = calloc(buffSize, sizeof(char));
			if (!buff) {
				perror("Unable to allocate ICS buffer space.");
				return;
			}

			env_getCpuInfo(buff, buffSize);
			xBoardWrite(xbs, "tellicsnoalias set note 2 CPU: %s", buff);
			env_getOsInfo(buff, buffSize);
			xBoardWrite(xbs, "tellicsnoalias set note 3 OS: %s", buff);

			free(buff);
		} else {
			xbs->onIcs = false;
		}
	}
}

static void xBoardPing(XBoardState* xbs, char** tokens, int tokenCount) {
	if (tokenCount >= 2) {
		xBoardWrite(xbs, "pong %s", tokens[1]);
	}
}

static void xBoardSetboard(XBoardState* xbs, char** tokens, int tokenCount) {
	char* fenStr;

	// Computing the length of the joined string.
	unsigned long bufferSize = 0;

	// Add the lengths of all the token strings.
	for (int i = 0; i < tokenCount; i++) {
		bufferSize += strlen(tokens[i]);
	}

	// Add the number of spaces needed to join them.
	if (bufferSize > 0) {
		bufferSize += ((unsigned long) tokenCount - 1);
	}

	// Add one for the null char.
	bufferSize++;

	fenStr = malloc(bufferSize * sizeof(char));
	if (!fenStr) {
		xBoardWrite(xbs, "Error: Unable to allocate memory for FEN.");
		return;
	}

	// Join the FEN tokens with single spaces.
	size_t idx = 0;
	for (int i = 1; i < tokenCount; i++) {
		if (i > 1) {
			fenStr[idx++] = ' ';
		}

		size_t len = strlen(tokens[i]);
		memcpy(fenStr + idx, tokens[i], len);
		idx += len;
	}

	fenStr[idx] = '\0';

	if (!parseFenWithPrint(&xbs->gameState, fenStr, false)) {
		xBoardWrite(xbs, "Error: Illegal position: %s", fenStr);
	}

	free(fenStr);
}

static bool isCommand(const char* command, char* str) {
	return strcmp(command, str) == 0;
}

static void chopNewline(char* buff, int length) {
	for (int i = 0; i < length; i++) {
		char c = buff[i];
		if (c == '\n' || c == '\r') {
			buff[i] = '\0';
			break;
		}
	}
}

static int parseTime(char** buff, int len) {
	int result = 0;
	if (len >= 2) {
		result = atoi(buff[1]) * 10; // Given unit is centiseconds, we'll use milliseconds always.
	}
	return result;
}

#define POST_BUFF_SIZE 1024
#define MAX_MOVE_SIZE 8
void postXBOutput(void* chessInterfaceState, int ply, int score, long timeCentiseconds, long nodes, MoveBuffer* pv) {
	XBoardState* xbs = (XBoardState*) chessInterfaceState;
	if (!xbs->postThinking) {
		return;
	}

	char* pvBuffer;
	bool freeBuffer;
	char moveBuff[MAX_MOVE_SIZE];
	if (pv == NULL || pv->length <= 0) {
		pvBuffer = "";
		freeBuffer = false;
	} else {
		pvBuffer = malloc(POST_BUFF_SIZE * sizeof(char));
		if (!pvBuffer) {
			xBoardWrite(xbs, "Error: Unable to allocate mamory for principal variation display.");
			return;
		}

		freeBuffer = true;
		int buffPos = 0;

		// Play the PV sequence forward, building up a list of move strings.
		// Remember the previous move must be played for the next move string to make any sense.
		for (int i = 0; i < pv->length && buffPos < POST_BUFF_SIZE - MAX_MOVE_SIZE; i++) {
			Move move = pv->moves[i];
			const int moveLen = notation_printShortAlg(&move, &xbs->gameState, moveBuff);
			makeMove(&xbs->gameState, &move);
			sprintf(pvBuffer + buffPos, " %s", moveBuff);
			buffPos += moveLen;
		}

		// Unmake the moves to return to the original game state.
		for (int i = pv->length - 1; i >= 0; i--) {
			unmakeMove(&xbs->gameState, &pv->moves[i]);
		}
	}

	xBoardWrite(xbs, "%i %i %i %i%s", ply, score, timeCentiseconds, nodes, pvBuffer);

	if (freeBuffer) {
		free(pvBuffer);
	}
}
#undef POST_BUFF_SIZE
#undef MAX_MOVE_SIZE


#define INPUT_BUFFER_SIZE 2048
#define MAX_INPUT_TOKENS 32
#define MAX_TOKEN_LEN 64
bool startXBoard() {
	char* inputBuffer;
	bool result = true;
	char** tb;
	XBoardState xbState;

	xbState.onIcs = false;

	// Initialize time in case we don't know better!
	xbState.opponentTime = 60 * 1000;
	xbState.myTime = 60 * 1000;

	inputBuffer = malloc(INPUT_BUFFER_SIZE * sizeof(char));
	if (!inputBuffer) {
		perror("Error: Unable to allocate memory for XBoard input buffer.");
		result = false;
		goto cleanup_inputBuff;
	}

	xbState.outputBuffer = malloc(XBOARD_BUFF_LEN * sizeof(char));
	if (!xbState.outputBuffer) {
		perror("Error: Unable to allocate memory for XBoard output buffer.");
		result = false;
		goto cleanup_outputBuff;
	}

	tb = createTokenBuffer(MAX_INPUT_TOKENS, MAX_TOKEN_LEN);
	if (!tb) {
		goto cleanup_inputBuff;
	}

	initializeGamestate(&xbState.gameState);

	// TODO: Define elsewhere
	const char* openingBook = "tulip_openings.sqlite";
	xbState.bookOpen = book_open(openingBook, &xbState.currentBook);
	xbState.postThinking = false;

	if (!log_open(&xbState.log)) {
		xBoardWrite(&xbState, "Error: Unable to open game log.");
		goto cleanup_book;
	}

	bool done = false;
	while (!done) {
		if (!fgets(inputBuffer, INPUT_BUFFER_SIZE, stdin)) {
			perror("Error: Unable to read from stdin.");
			result = false;
			goto cleanup_gamestate;
		}

		chopNewline(inputBuffer, INPUT_BUFFER_SIZE);
		logInput(&xbState, inputBuffer);

		int tokenCount = tokenize(inputBuffer, tb, MAX_INPUT_TOKENS);
		if (tokenCount > 0) {
			char* cmd = tb[0];
			if (isCommand("xboard", cmd)) {
				xBoardWrite(&xbState, ""); // Newline lets xboard know we're ready.
			} else if (isCommand("new", cmd)) {
				xBoardNew(&xbState);
			} else if (isCommand("protover", cmd)) {
				xBoardProtover(&xbState);
			} else if (isCommand("quit", cmd)) {
				done = true;
			} else if (isCommand("accepted", cmd)) {
			} else if (isCommand("force", cmd)) {
				xBoardForce(&xbState);
			} else if (isCommand("go", cmd)) {
				xBoardGo(&xbState);
			} else if (isCommand("setboard", cmd)) {
				xBoardSetboard(&xbState, tb, tokenCount);
			} else if (isCommand("ping", cmd)) {
				xBoardPing(&xbState, tb, tokenCount);
			} else if (isCommand("variant", cmd)) {
			} else if (isCommand("random", cmd)) {
			} else if (isCommand("playother", cmd)) {
			} else if (isCommand("white", cmd)) {
			} else if (isCommand("black", cmd)) {
			} else if (isCommand("level", cmd)) {
			} else if (isCommand("st", cmd)) {
			} else if (isCommand("sd", cmd)) {
			} else if (isCommand("nps", cmd)) {
			} else if (isCommand("time", cmd)) {
				xbState.myTime = parseTime(tb, tokenCount);
				log_write(&xbState.log, "My time: %.2fs", (double) xbState.myTime / 1000.0);
			} else if (isCommand("otim", cmd)) {
				xbState.opponentTime = parseTime(tb, tokenCount);
				log_write(&xbState.log, "Opponent time: %.2fs", (double) xbState.opponentTime / 1000.0);
			} else if (isCommand("usermove", cmd)) {
			} else if (isCommand("?", cmd)) {
			} else if (isCommand("draw", cmd)) {
			} else if (isCommand("result", cmd)) {
			} else if (isCommand("edit", cmd)) {
			} else if (isCommand("hint", cmd)) {
			} else if (isCommand("bk", cmd)) {
			} else if (isCommand("undo", cmd)) {
			} else if (isCommand("remove", cmd)) {
			} else if (isCommand("hard", cmd)) {
			} else if (isCommand("easy", cmd)) {
			} else if (isCommand("post", cmd)) {
				xbState.postThinking = true;
			} else if (isCommand("nopost", cmd)) {
				xbState.postThinking = false;
			} else if (isCommand("analyze", cmd)) {
			} else if (isCommand("name", cmd)) {
			} else if (isCommand("rating", cmd)) {
			} else if (isCommand("ics", cmd)) {
				xBoardIcs(&xbState, tb, tokenCount);
			} else if (isCommand("computer", cmd)) {
			} else if (isCommand("pause", cmd)) {
			} else if (isCommand("resume", cmd)) {
			} else if (isCommand("memory", cmd)) {
			} else if (isCommand("cores", cmd)) {
			} else if (isCommand("egtpath", cmd)) {
			} else if (isCommand("option", cmd)) {
			} else if (isCommand("exclude", cmd)) {
			} else if (isCommand("include", cmd)) {
			} else if (isCommand("setscore", cmd)) {
			} else if (isCommand("lift", cmd)) {
			} else if (isCommand("put", cmd)) {
			} else if (isCommand("hover", cmd)) {
			} else {
				xBoardMove(&xbState, cmd);
			}
		}
	}

	log_close(&xbState.log);
cleanup_book:
	if (xbState.bookOpen) {
		book_close(&xbState.currentBook);
	}
cleanup_gamestate:
	destroyGamestate(&xbState.gameState);
	freeTokenBuffer(tb, MAX_INPUT_TOKENS);
cleanup_outputBuff:
	free(inputBuffer);
cleanup_inputBuff:
	return result;
}

#undef INPUT_BUFFER_SIZE
#undef MAX_INPUT_TOKENS
#undef MAX_TOKEN_LEN

