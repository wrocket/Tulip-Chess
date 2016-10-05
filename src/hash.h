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

#ifndef HASH_H
#define HASH_H

#include <inttypes.h>
#include <limits.h>

#include "ztable.h"
#include "gamestate.h"
#include "cmdargs.h"
#include "hash.h"

#define HASH_NOT_FOUND INT_MAX

#define HASHF_EXACT 0
#define HASHF_ALPHA 1
#define HASHF_BETA 2

// Probe the hash table for a given state. Returns the score (if found) or HASH_NOT_FOUND otherwise.
int32_t hash_probe(GameState* state, int32_t currentDepth, int32_t alpha, int32_t beta);

// Put a new value in the hash table.
void hash_put(GameState* state, int32_t score, int32_t depth, int32_t flag);

// Creates a new ZTable of the given size in bits.
void hash_createZTable(GameLog* log, ZTable* table, int64_t sizeBits);

// Clean up a ZTable
void hash_destroyZTable(GameLog* log, ZTable* table);

// Reset a ZTable without reallocating memory
void hash_clearZTable(GameLog* log, ZTable* table);

// Get the size of the ZTable in bytes.
size_t hash_zTableSizeBytes();

// Prints a "friendly" size of the hash table.
void hash_friendlySize(TulipContext* cxt, char* str, size_t size);

// Computes the hash of the given state.
// SLOW. Do not use in tight loops.
uint64_t computeHash(GameState* gameState);

#endif
