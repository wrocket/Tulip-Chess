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
	print("extern const int HASH_PIECE_SQ[ORD_MAX + 1][144];")
	print("extern const int HASH_PIECE_CASTLE[16];");
	print("extern const int HASH_EP_FILE[9];")

def generate_single_hash_values():
	wtm = "WHITE_TO_MOVE"
	wtm_hash = hash_str(wtm)
	check_hash(wtm, wtm_hash)
	print("// %s" % wtm)
	print("#define HASH_WHITE_TO_MOVE %s" % wtm_hash)

def generate_ep_hash():
	for ep_file in ['NO_EP_FILE', 'FILE_A', 'FILE_B', 'FILE_C', 'FILE_D', 'FILE_E', 'FILE_F', 'FILE_G', 'FILE_H']:
		hash_key = 'EP_%s' % ep_file
		hash_val = hash_str(hash_key)
		check_hash(hash_key, hash_val)
		print('HASH_EP_FILE[%s] = %s; // %s' % (ep_file, hash_val, hash_key))

def generate_castle_flag_hash():
	for i in range(16):
		hash_key = 'CASTLE_FLAG_%i' % i
		hash_val = hash_str(hash_key)
		check_hash(hash_key, hash_val)
		print('HASH_PIECE_CASTLE[%i] = %s; // %s' % (i, hash_val, hash_key))

def generate_piece_square_hashes():
	files = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
	ranks = ['1', '2', '3', '4', '5', '6', '7', '8']
	pieces = ['EMPTY', 'WPAWN', 'BPAWN', 'WKNIGHT', 'BKNIGHT', 'WBISHOP', 'BBISHOP', 'WROOK', 'BROOK', 'WQUEEN', 'BQUEEN', 'WKING', 'BKING']
	for (f, r, p) in itertools.product(files, ranks, pieces):
		hash_key = p + '_' + f + r
		hash_val = hash_str(hash_key)
		check_hash(hash_key, hash_val)
		print('HASH_PIECE_SQ[&%s->ordinal][SQ_%s%s] = %s; // %s' % (p, f, r, hash_val, hash_key))

declare_array_hashes()
generate_single_hash_values()
generate_castle_flag_hash()
generate_ep_hash()
print()
generate_piece_square_hashes()
