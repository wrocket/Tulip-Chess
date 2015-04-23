import hashlib
import itertools

def hash_str(some_str):
	return hashlib.md5(bytes(some_str, 'utf-8')).hexdigest()[:16]

previous_hashes = set()

def check_hash(hash_key, hash_val):
	if hash_val in previous_hashes:
		print("Fatal: Duplicate hash values detected: %s for key %s" % (hash_val, hash_key))
		exit(-1)
	previous_hashes.add(hash_val)

def declare_array_hashes():
	print("extern const uint64_t HASH_PIECE_SQ[144][ORD_MAX + 1];")
	print("extern const uint64_t HASH_PIECE_CASTLE[16];");
	print("extern const uint64_t HASH_EP_FILE[9];")

def generate_single_hash_values():
	wtm = "WHITE_TO_MOVE"
	wtm_hash = hash_str(wtm)
	check_hash(wtm, wtm_hash)
	print("// %s" % wtm)
	print("#define HASH_WHITE_TO_MOVE 0x%s" % wtm_hash)

def generate_ep_hash():
	files = ['FILE_A', 'FILE_B', 'FILE_C', 'FILE_D', 'FILE_E', 'FILE_F', 'FILE_G', 'FILE_H', 'NO_EP_FILE']
	print('const uint64_t HASH_EP_FILE[%i] = {' % len(files))
	for ep_file in files:
		hash_key = 'EP_%s' % ep_file
		hash_val = hash_str(hash_key)
		check_hash(hash_key, hash_val)
		print('\t0x%s,\t// %s' % (hash_val, hash_key))
	print("}")

def generate_castle_flag_hash():
	print('const uint64_t HASH_PIECE_CASTLE[16] = {')
	for i in range(16):
		hash_key = 'CASTLE_FLAG_%i' % i
		hash_val = hash_str(hash_key)
		check_hash(hash_key, hash_val)
		print('\t0x%s%s\t// %s' % (hash_val, ', ' if i < 15 else '', hash_key))
	print("}")

def generate_piece_square_hashes():
	files = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
	ranks = ['1', '2', '3', '4', '5', '6', '7', '8']
	pieces = ['WPAWN', 'BPAWN', 'WKNIGHT', 'BKNIGHT', 'WBISHOP', 'BBISHOP', 'WROOK', 'BROOK', 'WQUEEN', 'BQUEEN', 'WKING', 'BKING', 'EMPTY', 'OFF_BOARD']
	abbrev = {'WPAWN': 'P', 'BPAWN': 'p', 'WKNIGHT': 'N', 'BKNIGHT': 'n', 'WBISHOP': 'B', 'BBISHOP': 'b', 'WROOK': 'R', 'BROOK': 'r', 'WQUEEN': 'Q', 'BQUEEN': 'q', 'WKING': 'K', 'BKING': 'k', 'EMPTY': '-', 'OFF_BOARD': '?'}
	print('const uint64_t HASH_PIECE_SQ[144][ORD_MAX + 1] = {')

	for sq_idx in range(144):
		stuff = []
		file_idx = (sq_idx % 12) - 2
		rank_idx = int(sq_idx / 12) - 2
		on_board = file_idx >= 0 and file_idx <= 7 and rank_idx >= 0 and rank_idx <= 7
		sq_str = files[file_idx] + ranks[rank_idx] if on_board else '  '
		for piece in pieces:
			if 'OFF_BOARD' != piece and on_board:
				hash_key = piece + '_' + sq_str
				hash_val = hash_str(hash_key)
				check_hash(hash_key, hash_val)
				stuff.append('/* %s%s */ 0x%s' % (abbrev[piece], sq_str, hash_val))
			else:
				stuff.append('0x0')
		print('\t/* %s */ {%s},' % (sq_str, ', '.join(stuff)))
	print('};')

declare_array_hashes()
print()
generate_single_hash_values()
print()
generate_castle_flag_hash()
print()
generate_ep_hash()
print()
generate_piece_square_hashes()
