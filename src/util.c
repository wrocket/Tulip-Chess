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
#include <stdio.h>

#include "tulip.h"

int tokenize(const char* str, char** tokenBuffers, const int maxTokens) {
    char* currentBuffer = tokenBuffers[0];
    int currentBufferIdx = -1;
    int currentBufferStrIdx = 0;
    int len = strlen(str);
    int isInToken = false;

    for(int idx = 0; idx < len && currentBufferIdx < maxTokens; idx++) {
        const char c = str[idx];
        if(isspace(c)) {
            if(isInToken) {
                isInToken = false;
                currentBuffer[currentBufferStrIdx++] = '\0';
            }
        } else {
            if(!isInToken) {
                isInToken = true;
                currentBuffer = tokenBuffers[++currentBufferIdx];
                currentBufferStrIdx = 0;
            }

            currentBuffer[currentBufferStrIdx++] = c;
        }
    }

    if(currentBufferStrIdx > 0 && currentBufferIdx < maxTokens) {
        currentBuffer[currentBufferStrIdx++] = '\0';
        currentBufferIdx++;
    }

    return currentBufferIdx;
}

char** createTokenBuffer(const int tokens, const int maxLength) {
    char** buffer = ALLOC_ZERO(tokens, char*, buffer, "Unable to allocate token buffer");
    for(int i=0; i<tokens; i++) {
        buffer[i] = ALLOC_ZERO(maxLength, char, buffer[i], "Unable to allocate token buffer]");
    }
    return buffer;
}

void freeTokenBuffer(char** buffer, const int length) {
    for(int i=0; i<length; i++) {
        free(buffer[i]);
    }

    free(buffer);
}

