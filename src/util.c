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
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "tulip.h"

#ifdef __MACH__
// Thanks to http://stackoverflow.com/a/9781275
#include <sys/time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
//clock_gettime is not implemented on OSX
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

    for (int idx = 0; idx < len && currentBufferIdx < maxTokens; idx++) {
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
    time_t s;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    s  = spec.tv_sec;
    return (long)((s * 1000.0) + round(spec.tv_nsec / 1.0e6));
}

