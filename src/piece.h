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

#ifndef PIECE_H
#define PIECE_H

#include <inttypes.h>

// This is the color white pieces have.
#define COLOR_WHITE     0

// This is the color black piece have.
#define COLOR_BLACK     1

// This is the "color" the empty squares have.
#define COLOR_NEITHER   2

// This is the "color" that off-board "squares" have.
#define COLOR_OFFBOARD  3

// Turns COLOR_WHITE to COLOR_BLACK and vice versa.
#define INVERT_COLOR(color) ((color) ^ 1)

#define ORD_WPAWN   0
#define ORD_BPAWN   1
#define ORD_WKNIGHT 2
#define ORD_BKNIGHT 3
#define ORD_WBISHOP 4
#define ORD_BBISHOP 5
#define ORD_WROOK   6
#define ORD_BROOK   7
#define ORD_WQUEEN  8
#define ORD_BQUEEN  9
#define ORD_WKING   10
#define ORD_BKING   11
#define ORD_EMPTY   12
#define ORD_OFFB    13
#define ORD_MAX     13

// Structure describing a piece on the board.
typedef struct {
        int32_t ordinal;    // A unique positive integer for this piece.
        int32_t color;      // The color of this piece.
        char name;      // The human-readable character for this piece.
        int32_t relativeValue; // A positive integer describing the general value of this piece compare to others.
} Piece;

extern const Piece WPAWN;
extern const Piece BPAWN;
extern const Piece WKNIGHT;
extern const Piece BKNIGHT;
extern const Piece WBISHOP;
extern const Piece BBISHOP;
extern const Piece WROOK;
extern const Piece BROOK;
extern const Piece WQUEEN;
extern const Piece BQUEEN;
extern const Piece WKING;
extern const Piece BKING;
extern const Piece EMPTY;
extern const Piece OFF_BOARD;

#define ALL_PIECES_LEN  13
extern const Piece* ALL_PIECES[13];

// Given a human-readable piece character, return a pointer to the relevant piece.
// E.g. "n" returns a pointer to BKNIGHT. Returns NULL if no piece matches.
const Piece* parsePiece(char);

#endif
