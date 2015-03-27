#!/usr/bin/python

# The MIT License (MIT)
#
# Copyright (c) 2015 Brian Wray (brian@wrocket.org)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Simple util that takes in a bitboard hex string, parses it in to a binary bitboard,
# and prints a 8x8 chessboard to stdout simulating the bits on the chessboard.

import sys
import re

if len(sys.argv) != 2:
	print('Usage: %s (hex digits)' % sys.argv[0])
	exit(-1)

input = sys.argv[1]

regex = re.compile('[0-9a-fA-F]+')

if regex.match(input):
	input = '0x' + input

as_int = int(input, 0)

as_bin = "{0:b}".format(as_int)

zeros = 64 - len(as_bin)

if zeros > 0:
	as_bin = ('0' * zeros) + as_bin

print("Board is viewed from white's perspective");
start = 0;
while start < len(as_bin):
	print(as_bin[start:start + 8])
	start += 8
