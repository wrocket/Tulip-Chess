#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdbool.h>

static void parseline(char* line) {

	char* key = calloc(1024, sizeof(char));
	char* value = calloc(1024, sizeof(char));

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

	printf("[%s] -> [%s]\n", key, value);

	free(key);
	free(value);
}

int main(void) {
	parseline("foobar=asdf");
	parseline("      foobar   =       asdf==");

}
