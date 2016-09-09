// The MIT License (MIT)

// Copyright (c) 2016 Brian Wray (brian@wrocket.org)

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

#include "config.h"

static char* allocate(const char* message) {
	const int32_t lineLength = 1024;
	char* line = calloc(lineLength, sizeof(char));
	if (!line) {
		perror(message);
		return NULL;
	}

	return line;
}

static bool parseKvp(char* line, size_t lineLength, char* key, char* value) {
	memset(key, 0, lineLength);
	memset(value, 0, lineLength);
	char* src = line;
	char* dest = key;
	bool inKey = true;
	while (*src) {
		char c = *src;
		if (inKey && c == '=') {
			inKey = false;
			dest = value;
		} else if (!isspace(c) && !iscntrl(c)) {
			*dest++ = c;
		}

		src++;
	}

	return true;
}

bool config_read(const char* fileName, AppConfig* config) {
	bool result = false;

	FILE* inFile;
	inFile = fopen(fileName, "r");
	if (inFile) {
		char* line = allocate("Unable to allocate memory for configuration line buffer.");
		if (!line) {
			goto CLEANUP_FILE;
		}

		while (fgets(line, lineLength, inFile)) {

			if(parseKvp(line, lineLength, key, value)) {
				char* end;
				if(strcmp(key, "test.search.depth") == 0) {
					const long n = strtol(value, &end, 10);
					if(end) {
						sprintf(stderr, "Invalid number: %s", value);
					} else {
						if(long > 0 && long < MAX_MOVE_BUFFER) {
							config->testSearchDepth = (int) n;
						}
					}
					
				}
			}
		}

		result = true;
	}

CLEANUP_KEY:
	free(line);
CLEANUP_FILE:
	fclose(inFile);
	return result;
}



void config_destroy(AppConfig* config) {

}
