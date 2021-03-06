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

CC=clang
CFLAGS=-Wall -Werror -Wconversion -pedantic -std=c99
OBJ_FILES = tulip.o board.o piece.o statedata.o movegen.o move.o util.o gamestate.o json.o fen.o bitboard.o attack.o \
makemove.o notation.o hash.o hashconsts.o draw.o result.o book.o eval.o evalconsts.o search.o xboard.o log.o \
interactive.o env.o time.o
FINAL_LINK_FLAGS=-lm -pthread -ldl

all: tulip

release: CFLAGS += -O2
release: tulip

debug: CFLAGS += -g
debug: tulip

tulip: $(OBJ_FILES)
	$(CC) $(OBJ_FILES) $(FINAL_LINK_FLAGS) -o tulip

tulip.o: tulip.c tulip.h
	$(CC) $(CFLAGS) -c tulip.c

board.o: board.c board.h
	$(CC) $(CFLAGS) -c board.c

piece.o: piece.c piece.h
	$(CC) $(CFLAGS) -c piece.c

statedata.o: statedata.c statedata.h
	$(CC) $(CFLAGS) -c statedata.c

movegen.o: movegen.c movegen.h
	$(CC) $(CFLAGS) -c movegen.c

interactive.o: interactive.c interactive.h
		$(CC) $(CFLAGS) -c interactive.c

makemove.o: makemove.c makemove.h
	$(CC) $(CFLAGS) -c makemove.c

move.o: move.c move.h
	$(CC) $(CFLAGS) -c move.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

gamestate.o: gamestate.c gamestate.h
	$(CC) $(CFLAGS) -c gamestate.c

fen.o: fen.c fen.h
	$(CC) $(CFLAGS) -c fen.c

json.o: json.c json.h
	$(CC) $(CFLAGS) -c json.c

bitboard.o: bitboard.c bitboard.h
	$(CC) $(CFLAGS) -c bitboard.c

attack.o: attack.c attack.h
	$(CC) $(CFLAGS) -c attack.c

notation.o: notation.c notation.h
	$(CC) $(CFLAGS) -c notation.c

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c

hashconsts.o: hashconsts.c hashconsts.h
	$(CC) $(CFLAGS) -c hashconsts.c

draw.o: draw.c draw.h
	$(CC) $(CFLAGS) -c draw.c

result.o: result.c result.h
	$(CC) $(CFLAGS) -c result.c

book.o: book.c book.h
	$(CC) $(CFLAGS) -c book.c

eval.o: eval.c eval.h evalconsts.h evalconsts.c
	$(CC) $(CFLAGS) -c eval.c

evalconsts.o: evalconsts.c evalconsts.h
	$(CC) $(CFLAGS) -c evalconsts.c

search.o: search.c search.h
	$(CC) $(CFLAGS) -c search.c

xboard.o: xboard.c xboard.h
	$(CC) $(CFLAGS) -c xboard.c

log.o: log.c log.h
	$(CC) $(CFLAGS) -c log.c

env.o: env.c env.h
	$(CC) $(CFLAGS) -c env.c

time.o: time.c time.h
	$(CC) $(CFLAGS) -c time.c

clean:
	rm *.o tulip
