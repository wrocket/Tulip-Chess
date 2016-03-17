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

#include "hash.h"
#include "hashconsts.h"
#include "tulip.h"
#include "gamestate.h"
#include "ztable.h"

int32_t hash_probe(GameState* state, int currentDepth) {
    const uint64_t hash = state->current->hash;
    ZTable* table = &state->zTable;

    ZTableEntry* entry = &table->data[hash % ZTABLE_SIZE];

    // Lower depths mean the hash was computer "higher" on the tree, so it's a more accurate score.
    // E.g. a depth of MAX_DEPTH would be a leaf node, which has pretty shallow evaluation.
    const bool foundEntry = entry->hash == hash && currentDepth < entry->depth;

    return foundEntry ? entry->score : HASH_NOT_FOUND;
}

void hash_put(GameState* state, int32_t score, int32_t depth) {
    const uint64_t hash = state->current->hash;
    ZTable* table = &state->zTable;

    ZTableEntry* entry = &table->data[hash % ZTABLE_SIZE];

    entry->score = score;
    entry->depth = depth;
    entry->hash = hash;
}

void hash_createZTable(ZTable* table) {
    table->data = calloc(ZTABLE_SIZE, sizeof(ZTableEntry));
    if (!table->data) {
        perror("Unable to allocate Zobrist table.");
        exit(-1);
    }
}

void hash_destroyZTable(ZTable* table) {
    free(table->data);
}

uint64_t computeHash(GameState* gameState) {
    uint64_t h = 0;

    if (gameState->current->toMove == COLOR_WHITE) {
        h ^= HASH_WHITE_TO_MOVE;
    }

    for (int32_t i = 0; i < 64; i++) {
        const int32_t sq = BOARD_SQUARES[i];
        const Piece* piece = gameState->board[sq];

        h ^= HASH_PIECE_SQ[sq][piece->ordinal];
    }

    h ^= HASH_EP_FILE[gameState->current->epFile];
    h ^= HASH_PIECE_CASTLE[gameState->current->castleFlags];

    // TODO: Fifty-move count?

    return h;
}
