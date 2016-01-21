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

#ifndef BOOK_H
#define BOOK_H

#include <inttypes.h>
#include "gamestate.h"
#include "move.h"
#include "sqlite/sqlite3.h"

// Structure defining a book. Essentially a "handle" to an open book.
typedef struct {
        sqlite3* database;
} OpenBook;

// Opens a book given a file name. Returns true on success, else false.
bool openBook(const char* fileName, OpenBook* book);

// Closes a book. Returns true on success, else false.
bool closeBook(OpenBook* book);

// Looks for moves in the book for the given position.
// Places those moves in buffer, and returns the number of moves placed in the buffer.
// Not suitable for tight loops.
int32_t getMovesFromBook(GameState* gameState, MoveBuffer* buffer, OpenBook* book);

#endif
