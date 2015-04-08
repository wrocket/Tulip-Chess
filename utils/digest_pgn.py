import sys
import re

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
	for r in filter(lambda r: len(r) > 0, results):
		print(' '.join(r))

all_games = [read_pgn_file(f) for f in sys.argv[1:]]

for g in all_games:
	write_digest_file(g)