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

#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "cmdargs.h"

TulipContext cmd_parseArgs(int argc, char** argv) {
	TulipContext result;
	result.useOpeningBook = true;
	result.argc = 0;
	result.argv = NULL;
	result.bookFile = "tulip_openings.sqlite";

	for (int i = 0; i < argc; i++) {
		char* arg = argv[i];
		if (0 == strcmp("--action", arg)) {
			result.argc = argc - i - 1;
			result.argv = &argv[i] + 1;
			return result;
		} else if (0 == strcmp("--nobook", arg)) {
			result.useOpeningBook = false;
		} else if (0 == strcmp("--bookfile", arg)) {
			if (i < argc - 1) {
				result.bookFile = argv[++i];
			} else {
				fprintf(stderr, "--bookfile option given without valid argument.\n");
			}
		}
	}

	return result;
}
