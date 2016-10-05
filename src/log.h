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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdbool.h>

typedef enum { DEBUG = 1, INFO = 2 } LogLevel;

typedef struct {
	FILE* fh;
	LogLevel level;
} GameLog;

// The maximum size of a log message.
#define LOG_BUFFER_SIZE 2048

// Open up a game log for writing.
// Returns true on success, false on failure.
bool log_open(GameLog* log);

// Write a formatted message to the log.
void log_write(GameLog* log, const char* format, ...);

// Write a formatted message to the log.
void log_debug(GameLog* log, const char* format, ...);

// Determine if the DEBUG level is enabled.
bool log_isDebug(GameLog* log);

// Close a log.
void log_close(GameLog* log);

#endif
