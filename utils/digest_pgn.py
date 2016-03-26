#!/usr/bin/python3

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
# books in the main C code or elsewhere.

# The output is a simple list of moves to the given depth found in the PGN.
# No attempt to parse or otherwise validate the move strings is made, so
# checking for illegal moves needs to be done by the consumer of this
# output.

import sys
import re
from itertools import chain

_max_moves = 14

if len(sys.argv) <= 1:
    print('Usage: %s [pgn file 1] [pgn file 2] ...' % sys.argv[0])
    exit(-1)


# Turn a token like '1.e4' to 'e4'
def remove_move_numbers(t):
    m = re.match(r'^\d+\.(.*)', t)
    return m.group(1) if m else t


# Eliminate empty tokens and move number tokens.
def is_move_token(str):
    return len(str) and not re.match(r'^\d+\.*', str) and str != '*'


def parse_moves(game_lines):
    joined_lines = ' '.join(game_lines)
    comments_removed = re.sub(r'{.*?}', '', joined_lines)
    split = re.split('\s+', comments_removed)
    no_numbers = [remove_move_numbers(t) for t in split]
    move_strs = filter(lambda s: is_move_token(s), no_numbers)
    return list(move_strs)[:_max_moves]


def parse_pgn_lines(line_source):
    results = []
    inside_game = False
    game_lines = []
    for line in line_source:
        line = line.strip()
        if len(line) == 0 or line.startswith('['):
            if inside_game and len(game_lines):
                results.append(parse_moves(game_lines))
                game_lines = []
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
    with open(file_name, 'r') as in_file:
        try:
            results = parse_pgn_lines(in_file)
        except ValueError:
            return []
    return results


def write_digest_file(results):
    flattened = chain.from_iterable(results)
    non_empty = filter(lambda r: len(r) > 0, flattened)
    lines = set([' '.join(r) for r in non_empty])
    for l in sorted(lines):
        print(l)


all_games = [read_pgn_file(f) for f in sys.argv[1:]]
write_digest_file(all_games)
