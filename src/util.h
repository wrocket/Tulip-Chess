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

#ifndef UTIL_H
#define UTIL_H

#include <inttypes.h>

// A "simple" tokenizer for strings.
// Given an input string, and a token buffer, and a maximum number of tokens,
// this will split the string on whitespace. See createTokenBuffer() to create
// the token buffer. This returns the number of tokens found.
int32_t tokenize(const char* str, char** tokenBuffer, const int32_t maxTokens);

// Creates an array of arrays to store token data.
char** createTokenBuffer(const int32_t maxTokens, const int32_t maxTokenLength);

// Free the array of arrays storing token data.
void freeTokenBuffer(char** tokenBuffer, const int32_t numberOfTokens);

// Retrieve the current UNIX epoch time in milliseconds.
int64_t getCurrentTimeMillis();

// Parse a given string as an integer, return success or failure.
// On failure, this will print an error message to stderr.
bool parseInteger(const char* str, int32_t* result);
#endif
