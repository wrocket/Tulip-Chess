Tulip-Chess
===========

Purpose
-------
This is a chess engine written for my own amusement. My goal isn't to write a world-beating chess monster, but to have fun applying programming techniques to a recreational pursuit. 

I've been writing little chess engines ever since my undergraduate years using all sorts of languages and styles, and Tulip is my current endeavor. It's not yet my strongest engine, though I hope it achieves that distinction.

Requirements
------------
The application is intended to be built on any UNIX-ish system using a GCC-like C compiler. Windows compilation should be straightforward using a POSIX-like layer such as Cygwin.

Currently the default makefile uses Clang 3.7. At the time of this writing, 90% of development is done on a Linux machine, so build issues with OS/X and such might slowly creep in as I don't test the build as frequently on that platform as I should.

SQLite is used as a storage engine for opening books and such. The application expects that the `sqlite3` command is available on the host system somewhere on the default path.

Principles of Development
-------------------------
* Tulip should be reasonably self-contained.
  * Ideally, the only dependencies would be standard POSIX headers and programs supplied by the OS.
  * SQLite is used as an opening book storage engine.
* Tulip should be readily portable to any UNIX-like system.
* Tulip should be easily buildable, ideally with a one-step invocation of 'make'.
* Code should favor clarity over cleverness.
  * Except where cleverness is truly clever.

Current Status
--------------
Currently, Tulip can play a relatively weak game of chess, taking advantage of obvious blunders, using the XBoard front-end.

Technical Concepts
------------------
### Internals
#### Core Chess Logic
Tulip uses a hybrid approach of both bitboard and array-based game state representation. This allows reasonably efficient attack detection and so on with the convenience of array-index board lookup. Tulip is designed to take full advantage of modern 64-bit processors in this regard.

#### Search
Tulip uses a relatively conventional alpha/beta search tactic, employing iterative deepening, null-moves, and Zobrist hashing to increase search speed. Current development is around adding search extensions such as quiescence search.

### Interop
#### Testing and Development
The automated tests and book generation are written in Python. Communication with Tulip is done via Unix-style text pipes. Many of Tulip's functionality (move generation, etc) can be invoked via command line options and the output is in a JSON structure. For example, to see the status of a game position given as a FEN string:
<pre>
src > ./tulip -gamestatus "7k/8/8/4r3/6b1/6n1/2PP1P2/4K3 w - - 0 1"
{"fenString": "7k/8/8/4r3/6b1/6n1/2PP1P2/4K3 w - - 0 1", "status": "whiteCheckmated"}
</pre>
The tests can now parse that output and make assertions against the results:
<pre>
self.assertEqual('whiteCheckmated', result['status'])
</pre>

#### Gameplay
Currently the XBoard protocol is used due to its simplicity. The interface is a very primitive synchronous `stdin`/`stdout`-based system. Eventually, the search will happen in a separate thread, enabling interrupting via the UI and such possible.

#### Opening Books
Tulip uses a SQLite database for its opening books. Generating these books from a set of games in a PGN format is accomplished via the enclosed Python scripts in `/utils`:

<pre>
utils > ./digest_pgn.py some_pgn_file.pgn | ./build_book.py
Read 1419 line(s) from stdin.
Wrote 14378 moves(s) and 12639 unique position(s) to opening file "tulip_openings.sqlite"
</pre>