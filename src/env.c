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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>

#include "env.h"
#include "posix.h"

static bool runCommand(const char* command, char* buff, size_t len) {
	if (len <= 0) {
		return false;
	}

	FILE *fp;
	bool result = false;
	fp = popen(command, "r");
	if (fp) {
		if (fgets(buff, (int32_t) len, fp)) {
			result = true;
			char* idx = buff + strlen(buff) - 1;
			while (idx > buff && iscntrl(*idx)) {
				*idx-- = '\0';
			}
		}

		pclose(fp);
	} else {
		buff[0] = '\0';
	}

	return result;
}

bool env_getCpuInfo(char* buff, size_t len) {
	char cpuInfo[64];
	const char* getCPU = "grep -m 1 \"model name\" /proc/cpuinfo | sed -e \"s/model name\\s\\+:\\s\\+//g\"";

	const bool result = runCommand(getCPU, cpuInfo, 64);
	const int64_t cores = env_getCoreCount();

	snprintf(buff, (size_t) len, "%s (%" PRId64 " core%s)", cpuInfo, cores, cores == 1 ? "" : "s");

	return result;
}

bool env_getOsInfo(char* buff, size_t len) {
	return runCommand("uname -o -r", buff, len);
}

int64_t env_getCoreCount() {
	char buff[16];
	const char* command = "cat /proc/cpuinfo | grep vendor_id | wc -l";

	runCommand(command, buff, 16);

	int64_t count = strtol(buff, NULL, 10);

	const bool validResult = count <= 0 || count == LONG_MIN || count == LONG_MAX;

	return validResult ? -1 : count;
}
