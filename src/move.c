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
#include <stdbool.h>
#include <strings.h>

#include "tulip.h"
#include "gamestate.h"
#include "movegen.h"
#include "board.h"
#include "move.h"

void createMoveBuffer(MoveBuffer* buff) {
    Move* moves = ALLOC(MOVE_BUFFER_LENGTH, Move, moves, "Moves not allocated.");
    buff->moves = moves;
    buff->length = 0;
    buff->created = true;
}

void destroyMoveBuffer(MoveBuffer* buff) {
    if(!(buff->created)) {
        fprintf(stderr, "Attempting to destroy move buffer that hasn't been created (or was already destroyed)\n");
        return;
    }

    free(buff->moves);
    buff->length = 0;
    buff->created = false;
}

void printMovelist(MoveBuffer* buffer) {
    char strBuff[16];

    printf("%i move(s)\n", buffer->length);
    printf("----------\n");
    for(int i=0; i<buffer->length; i++) {
        printMoveCoordinate(&(buffer->moves[i]), strBuff);
        printf("%i\t%s\n", i, strBuff);
    }
}

bool matchPseudoMoveCoord(GameState* gameState, char* moveStr, Move* m) {
    MoveBuffer moveBuff;
    int moveCount;
    bool result = false;
    char moveStrBuff[8];

    createMoveBuffer(&moveBuff);
    moveCount = generatePseudoMoves(gameState, &moveBuff);

    for (int i=0; i<moveCount; i++) {
        Move current = moveBuff.moves[i];
        printMoveCoordinate(&current, moveStrBuff);

        if (strcasecmp(moveStrBuff, moveStr) == 0) {
            result = true;
            *m = current;
            break;
        }
    }

    destroyMoveBuffer(&moveBuff);

    return result;
}

int printMoveCoordinate(Move* move, char* buffer) {
    int index = 0;

    index += printSquareIndex(move->from, buffer);
    index += printSquareIndex(move->to, buffer + index);

    switch(move->moveCode) {
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
