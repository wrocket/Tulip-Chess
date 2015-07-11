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

// A "simple" tokenizer for strings.
// Given an input string, and a token buffer, and a maximum number of tokens,
// this will split the string on whitespace. See createTokenBuffer() to create
// the token buffer. This returns the number of tokens found.
int tokenize(const char* str, char** tokenBuffer, const int maxTokens);

// Creates an array of arrays to store token data.
char** createTokenBuffer(const int maxTokens, const int maxTokenLength);

// Free the array of arrays storing token data.
void freeTokenBuffer(char** tokenBuffer, const int numberOfTokens);

long getCurrentTimeMillis();

#endif
