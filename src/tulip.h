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

#ifndef TULIP_H
#define TULIP_H

#include <stdlib.h>

// malloc()s a chunk of memory and exits with an error if the allocation failed.
#define ALLOC(qty, type, ptr, msg)  malloc((qty) * sizeof(type)); if(!(ptr)) {fprintf(stderr, (msg)); exit(EXIT_FAILURE);}

// Same as ALLOC(), but also sets the allocated memory to zero.
#define ALLOC_ZERO(qty, type, ptr, msg) ALLOC((qty), type, (ptr), (msg)); memset((ptr), 0, (qty) * sizeof(type))

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#endif
