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

# This script generates the square/piece score tables. Basically, tables that
# describe the "goodness" of a particular piece being on a particular square.

def write_row(scores):
	return '\t' + ', '.join([str(x).rjust(4) for x in scores]) + ','


def add_row_border(ordered_rows):
	border = [0] * 12
	side = [0] * 2
	result = [border, border]
	for r in ordered_rows:
		result.append(side + r + side)
	result.append(border)
	result.append(border)
	return result


def make_12x12(white_version_map, perspective='white'):
	result = []
	key_list = list(white_version_map.keys())
	if perspective == 'black':
		key_list = reversed(key_list)
	for k in key_list:
		result.append(white_version_map[k])
	result = add_row_border(result)
	return result


def write_array(name, whiteVersion):
	white_name = 'SQ_SCORE_%s_WHITE' % name.upper()
	black_name = 'SQ_SCORE_%s_BLACK' % name.upper()
	lines = []

	lines.append('const int %s[144] = {' % white_name)
	for row in make_12x12(whiteVersion, perspective='white'):
		lines.append(write_row(row))
	lines.append('};')

	lines.append('')

	lines.append('const int %s[144] = {' % black_name)
	for row in make_12x12(whiteVersion, perspective='black'):
		lines.append(write_row(row))
	lines.append('};')

	print('\n'.join(lines))


white_pawn_opening_scores = {
	8: [0, 0, 0, 0, 0, 0, 0, 0],
	7: [0, 0, 0, 0, 0, 0, 0, 0],
	6: [0, 0, 0, 0, 0, 0, 0, 0],
	5: [0, 0, 0, 15, 15, 0, 0, 0],
	4: [0, 0, 8, 15, 15, 8, 0, 0],
	3: [0, 0, 10, 10, 10, 10, 0, 0],
	2: [0, 0, -5, -5, -5, -5, 0, 0],
	1: [0, 0, 0, 0, 0, 0, 0, 0]
}

write_array('PAWN_OPENING', white_pawn_opening_scores)

white_knight_opening_scores = {
	8: [0, 0, 0, 0, 0, 0, 0, 0],
	7: [0, 0, 0, 0, 0, 0, 0, 0],
	6: [0, 0, 0, 0, 0, 0, 0, 0],
	5: [0, 0, 0, 0, 0, 0, 0, 0],
	4: [0, 0, 0, 0, 0, 0, 0, 0],
	3: [-10, 0, 10, 0, 0, 10, 0, -10],
	2: [-10, 0, 0, 5, 0, 5, 0, -10],
	1: [-20, -5, 0, 0, 0, 0, -5, -20]
}

write_array('KNIGHT_OPENING', white_knight_opening_scores)


white_queen_opening_scores = {
	8: [-10, -10, -10, -10, -10, -10, -10, -10],
	7: [-10, -10, -10, -10, -10, -10, -10, -10],
	6: [-10, -10, -10, -10, -10, -10, -10, -10],
	5: [-10, -10, -10, -10, -10, -10, -10, -10],
	4: [-10, -10, -10, -10, -10, -10, -10, -10],
	3: [-10, -10, -10, -10, -10, -10, -10, -10],
	2: [0, 0, 0, 0, 0, 0, 0, 0],
	1: [0, 0, 0, 0, 0, 0, 0, 0],
}

write_array('QUEEN_OPENING', white_queen_opening_scores)
