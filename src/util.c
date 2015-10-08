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

#define _POSIX_C_SOURCE 200112L // Needed on Linux systems using GCC for clock_gettime() with -std=c99

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#include "tulip.h"

// Turns out that clock_gettime() is not implemented on OSX.
// Here's a handy replacement found here: http://stackoverflow.com/a/9781275
// Thanks, StackOverflow!
#ifdef __MACH__
#include <sys/time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
int clock_gettime(int *clk_id, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif

int tokenize(const char* str, char** tokenBuffers, const int maxTokens) {
    char* currentBuffer = tokenBuffers[0];
    int currentBufferIdx = -1;
    int currentBufferStrIdx = 0;
    unsigned long len = strlen(str);
    int isInToken = false;

    for (int idx = 0; idx < len && currentBufferIdx < maxTokens - 1; idx++) {
        const char c = str[idx];
        if (isspace(c)) {
            if (isInToken) {
                isInToken = false;
                currentBuffer[currentBufferStrIdx++] = '\0';
            }
        } else {
            if (!isInToken) {
                isInToken = true;
                currentBuffer = tokenBuffers[++currentBufferIdx];
                currentBufferStrIdx = 0;
            }

            currentBuffer[currentBufferStrIdx++] = c;
        }
    }

    if (currentBufferStrIdx > 0 && currentBufferIdx < maxTokens) {
        currentBuffer[currentBufferStrIdx++] = '\0';
        currentBufferIdx++;
    }

    return currentBufferIdx;
}

char** createTokenBuffer(const int tokens, const int maxLength) {
    char** buffer = ALLOC_ZERO((unsigned long) tokens, char*, buffer, "Unable to allocate token buffer");
    for (int i = 0; i < tokens; i++) {
        buffer[i] = ALLOC_ZERO((unsigned long) maxLength, char, buffer[i], "Unable to allocate token buffer]");
    }
    return buffer;
}

void freeTokenBuffer(char** buffer, const int length) {
    for (int i = 0; i < length; i++) {
        free(buffer[i]);
    }

    free(buffer);
}

long getCurrentTimeMillis() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    const double seconds = (double) spec.tv_sec;
    const long nanos = spec.tv_nsec / 1000000;
    return (long)((seconds * 1000.0) + round((double) nanos));
}

bool parseInteger(char* str, int* result) {
    char* endToken = NULL;
    long longResult = strtol(str, &endToken, 10);

    if (errno != 0) {
        fprintf(stderr, "Invalid integer: %s (%s)\n", str, strerror(errno));
        return false;
    }

    if ((endToken != NULL && strlen(endToken) > 0)
            || longResult >= INT_MAX
            || longResult <= INT_MIN) {
        fprintf(stderr, "Invalid integer: %s\n", str);
        return false;
    }

    *result = (int) longResult;
    return true;
}
