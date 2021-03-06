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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "board.h"

const int32_t BOARD_SQUARES[64] = {SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
								   SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
								   SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
								   SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
								   SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
								   SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
								   SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
								   SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8};

const int32_t BOARD_SQ_COLORS[144] = {COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_WHITE, COLOR_BLACK, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD,
									  COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD, COLOR_OFFBOARD};

int parseFileChar(const char c) {
		switch(tolower(c)) {
		case 'a': return FILE_A;
		case 'b': return FILE_B;
		case 'c': return FILE_C;
		case 'd': return FILE_D;
		case 'e': return FILE_E;
		case 'f': return FILE_F;
		case 'g': return FILE_G;
		case 'h': return FILE_H;
		default: return INVALID_FILE;
		}
}

int parseRankChar(const char c) {
		switch(tolower(c)) {
		case '1': return RANK_1;
		case '2': return RANK_2;
		case '3': return RANK_3;
		case '4': return RANK_4;
		case '5': return RANK_5;
		case '6': return RANK_6;
		case '7': return RANK_7;
		case '8': return RANK_8;
		default: return INVALID_RANK;
		}
}

char fileToChar(const int32_t fileIndex) {
		switch(fileIndex) {
		case FILE_A: return 'a';
		case FILE_B: return 'b';
		case FILE_C: return 'c';
		case FILE_D: return 'd';
		case FILE_E: return 'e';
		case FILE_F: return 'f';
		case FILE_G: return 'g';
		case FILE_H: return 'h';
		default: return '?';
		}
}

char rankToChar(const int32_t rankIndex) {
		switch(rankIndex) {
		case RANK_1: return '1';
		case RANK_2: return '2';
		case RANK_3: return '3';
		case RANK_4: return '4';
		case RANK_5: return '5';
		case RANK_6: return '6';
		case RANK_7: return '7';
		case RANK_8: return '8';
		default: return '?';
		}
}

char indexToRankChar(const int32_t index) {
		return rankToChar(RANK_IDX(index));
}

char indexToFileChar(const int32_t index) {
		return fileToChar(FILE_IDX(index));
}

int printSquareIndex(const int32_t index, char* buffer) {
		int strIndex = 0;

		buffer[strIndex++] = indexToFileChar(index);
		buffer[strIndex++] = indexToRankChar(index);
		buffer[strIndex] = '\0';
		return strIndex;
}
