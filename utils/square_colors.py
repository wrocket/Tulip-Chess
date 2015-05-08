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

def sq_file(sq_indx):
	return (sq_indx % 12) - 2

def sq_rank(sq_indx):
	return (sq_indx // 12) - 2

def get_color(sq_indx):
	r = sq_rank(sq_indx)
	f = sq_file(sq_indx)
	if r not in range(8) or f not in range(8):
		return 'COLOR_OFFBOARD'
	if r & 1 == 0:
		return 'COLOR_BLACK' if f & 1 == 0 else 'COLOR_WHITE'
	else:
		return 'COLOR_WHITE' if f & 1 == 0 else 'COLOR_BLACK'

elements = []
for sq_indx in range(144):
	elements.append(get_color(sq_indx))

print("const int BOARD_SQ_COLORS[144] = {%s}" %', '.join(elements))