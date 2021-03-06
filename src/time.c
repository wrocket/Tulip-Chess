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

#include <inttypes.h>

#include "gamestate.h"

int64_t time_thinkTimeMillis(GameState* state, int64_t myTimeLeftMillis, int64_t opponentsTimeLeftMillis) {
	const int64_t defTime = 5000;

	// On the off chance we get some nonsense input, make sure we don't return a negative time!
	if (myTimeLeftMillis <= 0) {
		myTimeLeftMillis = 60 * 1000;
	}

	// If we have at least four times the default time left, use the default time.
	// Otherwise don't use more than a quarter of the remaining time.
	return myTimeLeftMillis > (defTime * 4) ? defTime : myTimeLeftMillis / 4;
}
