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

#ifndef BOARD_H
#define BOARD_H

#include "piece.h"

// A few words about the board representation.
// The board is an 8x8 grid with a 2-square "border" of sorts:
//
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    |    |    |    |    |    |    |    |    |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    |    |    |    |    |    |    |    |    |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A8 | B8 | C8 | D8 | E8 | F8 | G8 | H8 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A7 | B7 | C7 | D7 | E7 | F7 | G7 | H7 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A6 | B6 | C6 | D6 | E6 | F6 | G6 | H6 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A5 | B5 | C5 | D5 | E5 | F5 | G5 | H5 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A4 | B4 | C4 | D4 | E4 | F4 | G4 | H4 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A3 | B3 | C3 | D3 | E3 | F3 | G3 | H3 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    | A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1 |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    |    |    |    |    |    |    |    |    |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
// |    |    |    |    |    |    |    |    |    |    |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+
//
// This enables very simple and reasonable checking of moves.
// A sliding piece, for example, simply walks across the board until it
// meets a square that isn't empty. The edge of the board is detected
// by reaching a square occupied by an "off board" marker. Similarly,
// knights on the edge of the board check that they can move to a square
// by ensuring that a "off board" marker isn't on the target square. This
// is why the border is two squares, since a knight can move 2 ranks or files.
//
// The board structure itself is represented as 144 (that is 12x12) element
// flat array, where the square at (file, rank) is found by:
//
// array_index(file, rank) = 12 * (rank + 2) + (file + 2).
//
// All ranks and files are zero-indexed, so rank 1 = file A = 0.
//
// Given one of these indexes, squares relative to that square can be found
// using simple and fast addition. For example, to find the index of square S'
// F files and R ranks away from a square S, you have:
//
// S' = S + F + 12 * R
//
// Most of these offsets are constants below for efficient pre-processor
// calculation.

// Zero-based indexes for files.
#define FILE_A	0
#define FILE_B	1
#define FILE_C	2
#define FILE_D	3
#define FILE_E	4
#define FILE_F	5
#define FILE_G	6
#define FILE_H	7
#define INVALID_FILE 8

// Zero-based indexes for ranks.
#define RANK_1	0
#define RANK_2	1
#define RANK_3	2
#define RANK_4	3
#define RANK_5	4
#define RANK_6	5
#define RANK_7	6
#define RANK_8	7

// The 144-element board array can be thought of as a 8x8 chessboard with a
// two-square border around it. These constants are the indexes in to that
// array for the squares of the chessboard - "board indexes."
#define SQ_A1	26
#define SQ_A2	38
#define SQ_A3	50
#define SQ_A4	62
#define SQ_A5	74
#define SQ_A6	86
#define SQ_A7	98
#define SQ_A8	110
#define SQ_B1	27
#define SQ_B2	39
#define SQ_B3	51
#define SQ_B4	63
#define SQ_B5	75
#define SQ_B6	87
#define SQ_B7	99
#define SQ_B8	111
#define SQ_C1	28
#define SQ_C2	40
#define SQ_C3	52
#define SQ_C4	64
#define SQ_C5	76
#define SQ_C6	88
#define SQ_C7	100
#define SQ_C8	112
#define SQ_D1	29
#define SQ_D2	41
#define SQ_D3	53
#define SQ_D4	65
#define SQ_D5	77
#define SQ_D6	89
#define SQ_D7	101
#define SQ_D8	113
#define SQ_E1	30
#define SQ_E2	42
#define SQ_E3	54
#define SQ_E4	66
#define SQ_E5	78
#define SQ_E6	90
#define SQ_E7	102
#define SQ_E8	114
#define SQ_F1	31
#define SQ_F2	43
#define SQ_F3	55
#define SQ_F4	67
#define SQ_F5	79
#define SQ_F6	91
#define SQ_F7	103
#define SQ_F8	115
#define SQ_G1	32
#define SQ_G2	44
#define SQ_G3	56
#define SQ_G4	68
#define SQ_G5	80
#define SQ_G6	92
#define SQ_G7	104
#define SQ_G8	116
#define SQ_H1	33
#define SQ_H2	45
#define SQ_H3	57
#define SQ_H4	69
#define SQ_H5	81
#define SQ_H6	93
#define SQ_H7	105
#define SQ_H8	117

// Contains the board indexes of the chessboard.
extern const int BOARD_SQUARES[64];

// Contains the square color of a given square.
extern const int BOARD_SQ_COLORS[144];

// Defines constants to add to board indexes to get the indexes of other
// squares on the board. "North" is towards black's side of the board.
// So, SQ_E4 + OFFSET_N = SQ_E5, and so on.
#define OFFSET_N	12
#define OFFSET_S	-12
#define OFFSET_E	1
#define OFFSET_W	-1
#define OFFSET_NE	(OFFSET_N + OFFSET_E)
#define OFFSET_NW	(OFFSET_N + OFFSET_W)
#define OFFSET_SE	(OFFSET_S + OFFSET_E)
#define OFFSET_SW	(OFFSET_S + OFFSET_W)

// Defines offsets that describe a knight move.
#define OFFSET_KNIGHT_1	(OFFSET_N + OFFSET_NE)
#define OFFSET_KNIGHT_2	(OFFSET_N + OFFSET_NW)
#define OFFSET_KNIGHT_3	(OFFSET_S + OFFSET_SE)
#define OFFSET_KNIGHT_4	(OFFSET_S + OFFSET_SW)
#define OFFSET_KNIGHT_5	(OFFSET_E + OFFSET_NE)
#define OFFSET_KNIGHT_6	(OFFSET_E + OFFSET_SE)
#define OFFSET_KNIGHT_7	(OFFSET_W + OFFSET_NW)
#define OFFSET_KNIGHT_8	(OFFSET_W + OFFSET_SW)

// Macro to calculate the board index from a zero-indexed file and rank.
#define B_IDX(FILE, RANK)	(12 * ((RANK) + 2) + (FILE) + 2)

// Derive the zero-indexed rank from a board index.
#define RANK_IDX(RANK)	(((RANK) / 12) - 2)

// Derive the zero-indexed file from a board index.
#define FILE_IDX(FILE)	(((FILE) % 12) - 2)

// Given a human-readable character for a file, return the zero-based file index.
// E.g.: an input of 'c' returns FILE_C.
int parseFileChar(const char c);

// Given a zero-based file index, return a human-readable respresentation.
// E.g.: an input of FILE_C returns 'c'.
char fileToChar(const int);

// Given a zero-based rank index, return a human-readable respresentation.
// E.g.: an input of RANK_3 returns '3'.
char rankToChar(const int rankIndex);

// Given a board index and a character buffer of at least two characters,
// print a human-readable representation of the square, and return the
// number of characters written to the given buffer. It is the caller's
// responsibility to null-terminate given that information.
// E.g.: An input of (26, char[2]) will writer ['A' | '1'] to the buffer
// and return the number 2.
int printSquareIndex(const int, char*);

// Given a zero-based rank index, return a human-readable representation.
// E.g.: An input of RANK_3 returns '3'.
char indexToRankChar(const int);

// Given a zero-based file index, return a human-readable representation.
// E.g.: An input of FILE_C returns 'c'.
char indexToFileChar(const int);

#endif
