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