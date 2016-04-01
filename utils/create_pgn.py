#!/usr/bin/python3

# The MIT License (MIT)

# Copyright (c) 2015 Brian Wray (brian@wrocket.org)

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# This script reads a bunch of chess lines from stdin and reformats them into a PGN file.
# A common usage here is to digest several large PGN files into a single more compact one.
# Example:
#
# ./digest_pgn.py ../book_files/*.pgn | ./generate_tree.py | ./create_pgn.py > basic_book.pgn
#
# This digests a set of large PGNs, combines them into a single tree, and
# creates a single PGN file out of that tree.

import re
import sys
import math
import datetime

_whitespace_split = re.compile('\s+')
_now = str(datetime.date.today()).replace('-', '.')

def parse_line(line):
	return _whitespace_split.split(line.strip())

def create_pgn(line_moves):
	print('[Event "Generated Opening Book"]')
	print('[Date "%s"]' % _now)
	print('[Result "*"]')
	count = 1
	for m in line_moves:
		if (count % 2) == 1:
			sys.stdout.write(str(math.floor(count / 2) + 1))
			sys.stdout.write('. ')
		sys.stdout.write('%s ' % m)
		count += 1
	print()
	print()


for line in sys.stdin:
	create_pgn(parse_line(line))