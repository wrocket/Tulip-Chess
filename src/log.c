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
#include <stdarg.h>

#include "log.h"

#define DATE_BUFF_SIZE 32
#define FILE_NAME_SIZE 128

static void printDt(char* buff, const char* format) {
	struct tm* tm_info;
	time_t timer;

	time(&timer);
	tm_info = localtime(&timer);
	strftime(buff, DATE_BUFF_SIZE, format, tm_info);
}

static void writeEntry(GameLog* log, const char* message) {
	if (log != NULL) {
		char dateBuff[DATE_BUFF_SIZE];
		printDt(dateBuff, "%F %H:%M:%S%z");

		fprintf(log->fh, "%s\t%s\n", dateBuff, message);
		fflush(log->fh);
	}
}

bool log_open(GameLog* log) {
	char* fname;
	char dateBuff[DATE_BUFF_SIZE];

	fname = malloc(FILE_NAME_SIZE * sizeof(char));
	if (!fname) {
		perror("Unable allocate memory for log file name.");
		exit(-1);
	}

	printDt(dateBuff, "%F-%H%M%S");
	snprintf(fname, FILE_NAME_SIZE, "game-%s-p%i.log", dateBuff, getpid());

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

void log_write(GameLog* log, const char* format, ...) {
	if (log != NULL) {
		char* outputMessage = malloc(LOG_BUFFER_SIZE * sizeof(char));

		if (!outputMessage) {
			printf("Error: Unable to allocate log message buffer.\n");
			exit(-1);
		}

		va_list argptr;
		va_start(argptr, format);
		vsnprintf(outputMessage, LOG_BUFFER_SIZE, format, argptr);
		va_end(argptr);

		writeEntry(log, outputMessage);

		free(outputMessage);
	}
}

void log_close(GameLog* log) {
	if (log != NULL) {
		fclose(log->fh);
	}
}
