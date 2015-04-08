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

# Script to digest a series of PGN files in to a simple list of move strings.
# The output can be used to easily build up transposition tables or opening
# books in the main C code.

import sys
import re
from itertools import chain

_max_moves = 20

if len(sys.argv) <= 1:
	print('Usage: %s [pgn file 1] [pgn file 2] ...' % sys.argv[0])
	exit(-1)


def is_move_token(str):
	return len(str) and not re.match(r'^\d+\.*', str)


def parse_moves(game_lines):
	joined_lines = ' '.join(game_lines)
	comments_removed = re.sub(r'{.*}', '', joined_lines)
	split = re.split('\s+', comments_removed)
	move_strs = filter(lambda s: is_move_token(s), split)
	return list(move_strs)[:_max_moves]


def parse_pgn_lines(line_source):
	results = []
	inside_game = False
	game_lines = []
	for line in line_source:
		line = line.strip()
		if len(line) == 0:
			if inside_game and len(game_lines):
				results.append(parse_moves(game_lines))
			inside_game = False
			game_lines = []
			continue
		if line.startswith('['):
			inside_game = False
			continue
		elif inside_game:
			game_lines.append(line)
		else:
			inside_game = True
			game_lines = [line]
	if len(game_lines):
		results.append(parse_moves(game_lines))
	return results


def read_pgn_file(file_name):
	print('Reading file %s...' % file_name)
	with open(file_name, 'r') as in_file:
		results = parse_pgn_lines(in_file)
	print('\t...found %i game(s) in %s.' % (len(results), file_name))
	return results


def write_digest_file(results):
	flattened = chain.from_iterable(results)
	for r in sorted(filter(lambda r: len(r) > 0, flattened)):
		print(' '.join(r))


all_games = [read_pgn_file(f) for f in sys.argv[1:]]
write_digest_file(all_games)