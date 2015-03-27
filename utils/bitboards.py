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

# Script for generating the bitmask header file.

import itertools

_rank_strs = [str(r) for r in range(1, 9)]
_file_strs = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']

def board_index(file, rank):
	return 12 * (rank + 2) + file + 2

def compute_piece_bit_array(bit_name):
	result = ['0x0' for x in range(144)]
	for file, rank in itertools.product(range(0, 8), range(0, 8)):
		result[board_index(file, rank)] = '%s_%s' % (bit_name, print_sq(file, rank))
	return result

def print_square_array(name, array):
	start = 0
	print("%s = {" % name);
	while(start < len(array)):
		slice = array[start: start + 12]
		print('\t', ', '.join(slice) + ',')
		start += 12
	print("};")

def print_hex(n):
	h = hex(n)
	zeros = '0' * (18 - len(h))
	return h.replace('0x', '0x' + zeros)


def square_index(file, rank):
	return 8 * rank + file


def print_sq(file, rank):
	return _file_strs[file] + _rank_strs[rank]


def get_sq_bit(index):
	return 1 << index

def compute_rank_mask(rank):
	result = 0
	for file in range(0, 8):
		result = result | get_sq_bit(square_index(file, rank))
	return result


def compute_file_mask(file):
	result = 0
	for rank in range(0, 8):
		result = result | get_sq_bit(square_index(file, rank))
	return result


def compute_bishop_mask(file, rank):
	result = 0
	offsets = itertools.product([-1, 1], [-1, 1])
	for o in offsets:
		offset_f = o[0]
		offset_r = o[1]
		current_f = file + offset_f
		current_r = rank + offset_r
		while current_f in range(0, 8) and current_r in range(0, 8):
			result = result | get_sq_bit(square_index(current_f, current_r))
			current_r += offset_r
			current_f += offset_f
	return result


def compute_rook_mask(file, rank):
		# combine the file and rank mask
		# then remove the square we're on from the mask (punch a hole where the file and rank meet)
		sq_bit = get_sq_bit(square_index(file, rank))
		return (compute_rank_mask(rank) | compute_file_mask(file)) & ~sq_bit


def compute_queen_mask(file, rank):
	return compute_bishop_mask(file, rank) | compute_rook_mask(file, rank)

def compute_wpawn_mask(file, rank):
	result = 0
	if rank > 0:
		if file > 0:
			result = result | get_sq_bit(square_index(file - 1, rank - 1))
		if file < 7:
			result = result | get_sq_bit(square_index(file + 1, rank - 1))
	return result

def compute_bpawn_mask(file, rank):
	result = 0
	if rank < 7:
		if file > 0:
			result = result | get_sq_bit(square_index(file - 1, rank + 1))
		if file < 7:
			result = result | get_sq_bit(square_index(file + 1, rank + 1))
	return result

# Useful for debugging
def print_mask_as_board(mask):
	for rank in reversed(range(0, 8)):
		line = []
		for file in range(0, 8):
			if mask & get_sq_bit(square_index(file, rank)) != 0:
				line.append('X')
			else:
				line.append('-')
		print(''.join(line))

def compute_nonslider_mask(file, rank, offsets):
	result = 0
	for o in offsets:
		offset_f = file + o[0]
		offset_r = rank + o[1]
		if offset_f in range(0, 8) and offset_r in range(0, 8):
			bit = get_sq_bit(square_index(file + o[0], rank + o[1]))
			result |= bit
	return result


def compute_knight_mask(file, rank):
	pairs = itertools.product([2, -2, 1, -1], [2, -2, 1, -1])
	offsets = filter(lambda t: abs(t[0]) != abs(t[1]), pairs)
	return compute_nonslider_mask(file, rank, offsets)


def compute_king_mask(file, rank):
	pairs = itertools.product([-1, 0, 1], [-1, 0, 1])
	offsets = filter(lambda t: (t[0] | t[1]) != 0, pairs)
	return compute_nonslider_mask(file, rank, offsets)


print("// Bitmasks for individual squares.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	idx = square_index(file, rank)
	mask = get_sq_bit(idx)
	sq_str = print_sq(file, rank)
	hex_str = print_hex(mask)
	print('#define BIT_SQ_%s\t%s' % (sq_str, hex_str))


print()
print("// Bitmasks for particular ranks (basically an OR of all the square bitmasks on a given rank)")
for rank in range(0, 8):
	result = compute_rank_mask(rank)
	hex_str = print_hex(result)
	print('#define BIT_RANK_%s\t%s' % (_rank_strs[rank], hex_str))

print()
print("// Bitmasks for particular file (basically an OR of all the square bitmasks on a given file)")
for file in range(0, 8):
	result = compute_file_mask(file)
	hex_str = print_hex(result)
	print('#define BIT_FILE_%s\t%s' % (_file_strs[file], hex_str))

print()
print("// Bitmasks for rook moves from a particular square.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	mask = compute_rook_mask(file, rank)
	print('#define BIT_ROOK_%s\t%s' % (print_sq(file, rank), print_hex(mask)))

print()
print("// Bitmasks for bishop moves from a particular square.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	mask = compute_bishop_mask(file, rank)
	print('#define BIT_BISHOP_%s\t%s' % (print_sq(file, rank), print_hex(mask)))

print()
print("// Bitmasks for queen moves from a particular square.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	mask = compute_queen_mask(file, rank)
	print('#define BIT_QUEEN_%s\t%s' % (print_sq(file, rank), print_hex(mask)))

print()
print("// Bitmasks for knight moves from a particular square.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	mask = compute_knight_mask(file, rank)
	print('#define BIT_KNIGHT_%s\t%s' % (print_sq(file, rank), print_hex(mask)))

print()
print("// Bitmasks for king moves from a particular square.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	mask = compute_king_mask(file, rank)
	print('#define BIT_KING_%s\t%s' % (print_sq(file, rank), print_hex(mask)))

print()
print("// Bitmasks for white pawn attacks.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	mask = compute_wpawn_mask(file, rank)
	print('#define BIT_WPAWN_%s\t%s' % (print_sq(file, rank), print_hex(mask)))

print()
print("// Bitmasks for black pawn attacks.")
for file, rank in itertools.product(range(0, 8), range(0, 8)):
	mask = compute_bpawn_mask(file, rank)
	print('#define BIT_BPAWN_%s\t%s' % (print_sq(file, rank), print_hex(mask)))

print()
print("// Bitmasks for individual masks, addressable by square index.")
print("extern const uint64_t BITS_SQ[144];")
print("extern const uint64_t BITS_BISHOP[144];")
print("extern const uint64_t BITS_KNIGHT[144];")
print("extern const uint64_t BITS_ROOK[144];")
print("extern const uint64_t BITS_QUEEN[144];")
print("extern const uint64_t BITS_KING[144];")
print("extern const uint64_t BITS_WPAWN[144];")
print("extern const uint64_t BITS_BPAWN[144];")


print("//// ---------- Bitboard.c starts here ----------- ////")

print()
print("// Array of bitmasks for squares indexable by individual square indexes")
print_square_array('const uint64_t BITS_SQ[144]', compute_piece_bit_array('BIT_SQ'))

print()
print_square_array('const uint64_t BITS_BISHOP[144]', compute_piece_bit_array('BIT_BISHOP'))

print()
print_square_array('const uint64_t BITS_KNIGHT[144]', compute_piece_bit_array('BIT_KNIGHT'))

print()
print_square_array('const uint64_t BITS_ROOK[144]', compute_piece_bit_array('BIT_ROOK'))

print()
print_square_array('const uint64_t BITS_QUEEN[144]', compute_piece_bit_array('BIT_QUEEN'))

print()
print_square_array('const uint64_t BITS_KING[144]', compute_piece_bit_array('BIT_KING'))

print()
print_square_array('const uint64_t BITS_WPAWN[144]', compute_piece_bit_array('BIT_WPAWN'))

print()
print_square_array('const uint64_t BITS_BPAWN[144]', compute_piece_bit_array('BIT_BPAWN'))



