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

# This script reads a digest of a PGN file and stores the moves in a SQLite3
# database indexed by the position hash.
#
# The idea is to use this in conjunction with the digest_pgn.py script, e.g.:
# $ ./digest_pgn.py my_openings.pgn | ./build_book.py

import datetime
import json
import os
import re
import sqlite3 as lite
import subprocess
import sys


class MoveLine:
    def __init__(self, initial_hash, move_hashes):
        self.initial_hash = initial_hash
        self.move_hashes = move_hashes


def build_line(line):
    cmd = ['../src/tulip', '-hashseq', 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1']
    cmd.extend(line)
    try:
        out = subprocess.check_output(cmd)
        result = json.loads(out.decode('utf-8'))
        return MoveLine(result['initialHash'], [(x['move'], x['resultingHash']) for x in result['hashSequence']])
    except (BrokenPipeError, subprocess.CalledProcessError):
        print('Unplayable line: %s' % ' '.join(line))
        return None


def digest_line(move_line):
    if not move_line:
        return None
    d = {}
    current_hash = move_line.initial_hash
    for item in move_line.move_hashes:
        d[current_hash] = [item[0]]
        current_hash = item[1]
    return d

def combine_lines(line_dicts):
    result = {}
    for d in line_dicts:
        for (k, v) in d.items():
            if k not in result:
                result[k] = set(v)
            else:
                result[k].update(v)
    return result


def write_to_database(data, database_name):
    if os.path.exists(database_name):
        os.remove(database_name)
    with lite.connect(database_name) as con:
        cur = con.cursor()
        cur.execute('create table BOOK_METADATA(PROPERTY_KEY VARCHAR2(32), PROPERTY_VALUE VARCHAR2(200))')
        cur.execute('create table OPENING_BOOK(POSITION_HASH VARCHAR2(16), MOVE VARCHAR2(8))')
        cur.execute('create index POSITION_IDX on OPENING_BOOK(POSITION_HASH)')
        properties = {}
        properties['version'] = '1'
        properties['created'] = str(datetime.datetime.now())
        for kvp in properties.items():
            cur.execute('insert into BOOK_METADATA values(?, ?)', kvp)
        move_count = 0
        for (position, moves) in data.items():
            move_count += len(moves)
            for m in moves:
                cur.execute('insert into OPENING_BOOK values (?, ?)', (position, m))
        print('Wrote %i moves(s) and %i unique position(s) to opening file "%s"' % (move_count, len(data.keys()), database_name))


def read_input():
    raw_lines = [line.strip() for line in sys.stdin]
    return list([re.split(r'\s+', line) for line in raw_lines])


all_lines = read_input()
line_len = len(all_lines)
print("Read %i line(s) from stdin." % line_len)
digested_lines = []
count = 0
for x in all_lines:
    count += 1
    if count % 10 == 0 or count == line_len:
        sys.stdout.write('\rCompleted %0.2f%%' % (count / line_len * 100.0))
    digested = digest_line(build_line(x))
    if digested != None:
        digested_lines.append(digested)
print()
print('Combining positions...');
file_lines = combine_lines(digested_lines)
print('Writing to database file...')
write_to_database(file_lines, 'tulip_openings.sqlite')
print('Done.')