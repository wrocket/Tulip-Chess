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

#include <stdlib.h>

#include "tulip.h"
#include "piece.h"

const Piece WPAWN = {
	.ordinal = ORD_WPAWN,
	.color = COLOR_WHITE,
	.name = 'P'
};

const Piece BPAWN = {
	.ordinal = ORD_BPAWN,
	.color = COLOR_BLACK,
	.name = 'p'
};

const Piece WKNIGHT = {
	.ordinal = ORD_WKNIGHT,
	.color = COLOR_WHITE,
	.name = 'N'
};

const Piece BKNIGHT = {
	.ordinal = ORD_BKNIGHT,
	.color = COLOR_BLACK,
	.name = 'n'
};

const Piece WBISHOP = {
	.ordinal = ORD_WBISHOP,
	.color = COLOR_WHITE,
	.name = 'B'
};

const Piece BBISHOP = {
	.ordinal = ORD_BBISHOP,
	.color = COLOR_BLACK,
	.name = 'b'
};

const Piece WROOK = {
	.ordinal = ORD_WROOK,
	.color = COLOR_WHITE,
	.name = 'R'
};

const Piece BROOK = {
	.ordinal = ORD_BROOK,
	.color = COLOR_BLACK,
	.name = 'r'
};

const Piece WQUEEN = {
	.ordinal = ORD_WQUEEN,
	.color = COLOR_WHITE,
	.name = 'Q'
};

const Piece BQUEEN = {
	.ordinal = ORD_BQUEEN,
	.color = COLOR_BLACK,
	.name = 'q'
};

const Piece WKING = {
	.ordinal = ORD_WKING,
	.color = COLOR_WHITE,
	.name = 'K'
};

const Piece BKING = {
	.ordinal = ORD_BKING,
	.color = COLOR_BLACK,
	.name = 'k'
};

const Piece EMPTY = {
	.ordinal = ORD_EMPTY,
	.color = COLOR_NEITHER,
	.name = '-'
};

const Piece OFF_BOARD = {
	.ordinal = ORD_OFFB,
	.color = COLOR_OFFBOARD,
	.name = 'X'
};

const Piece* ALL_PIECES[13] = {&WPAWN, &BPAWN, &WKNIGHT, &BKNIGHT, &WBISHOP, &BBISHOP, &WROOK, &BROOK, &WQUEEN, &BQUEEN, &WKING, &BKING, &EMPTY};

const Piece* parsePiece(char p) {
	switch(p) {
		case 'P': return &WPAWN;
		case 'p': return &BPAWN;
		case 'N': return &WKNIGHT;
		case 'n': return &BKNIGHT;
		case 'B': return &WBISHOP;
		case 'b': return &BBISHOP;
		case 'R': return &WROOK;
		case 'r': return &BROOK;
		case 'Q': return &WQUEEN;
		case 'q': return &BQUEEN;
		case 'K': return &WKING;
		case 'k': return &BKING;
		default: return NULL;
	}
}
