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
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "hash.h"
#include "tulip.h"

#define HASH_BUFF_LEN 128

uint64_t hashStr(char* str, const int strLen) {
    // TODO: Add support for GNU md5sum in addition to BSD's md5
    // TODO: Better error handling if the "md5" application doesn't respond as expected.

    char* buffer = ALLOC(HASH_BUFF_LEN, char, buffer, "Unable to allocate input buffer for hashing.");
    const char* cmd = "md5 -q -s ";
    char* commandStr = ALLOC(strLen + strlen(cmd), char, commandStr, "Unable to allocate command string for hashing.");
    strcat(commandStr, cmd);
    strncat(commandStr, str, strLen);
    uint64_t result = 0;

    FILE* pipe = popen(commandStr, "r");
    if (!pipe) {
        fprintf(stderr, "Unable to execute command: %s\n", commandStr);
        fprintf(stderr, "Got error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(fgets(buffer, HASH_BUFF_LEN, pipe) != NULL) {
        char* endPtr;
        buffer[16] = '\0';
        result = strtoull(buffer, &endPtr, 16);

        if (endPtr == buffer || errno != 0) {
            fprintf(stderr, "Error executing \"%s\": %s\n", commandStr, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    pclose(pipe);
    free(buffer);
    free(commandStr);
    return result;
}

#undef HASH_BUFF_LEN
