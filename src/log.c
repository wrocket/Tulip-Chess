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
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "log.h"

static void printDt(char* buff, const char* format) {
    struct tm* tm_info;
    time_t timer;

    time(&timer);
    tm_info = localtime(&timer);
    strftime(buff, 32, format, tm_info);
}

bool openLog(GameLog* log) {
    char* fname;
    char dateBuff[32];
    const int buffSize = 128;

    fname = malloc(buffSize * sizeof(char));
    if (!fname) {
        perror("Unable allocate memory for log file name.");
        exit(-1);
    }

    printDt(dateBuff, "%F-%H%M%S");
    snprintf(fname, buffSize, "game-%s-p%i.log", dateBuff, getpid());

    bool result = false;
    log->fh = fopen(fname, "w");

    if (log->fh) {
        result = true;
        writeEntry(log, "Opened log.");
    } else {
        perror("Unable to open log file.");
        goto open_log_fail;
    }

open_log_fail:
    free(fname);
    return result;
}

void writeEntry(GameLog* log, const char* message) {
    if (log != NULL) {
        char dateBuff[32];
        printDt(dateBuff, "%F %H:%M:%S%z");

        fprintf(log->fh, "%s\t%s\n", dateBuff, message);
        fflush(log->fh);
    }
}

void closeLog(GameLog* log) {
    fclose(log->fh);
}
