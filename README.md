Tulip-Chess
===========

Purpose
-------
This is a chess engine written for my own amusement. Not designed to be the absolute strongest, but rather a fun programming project. The current goal is play strength >= 2000 ELO. I've been writing chess engines since my undergraduate years using various technologies and techniques. Tulip represents what development I find "fun" at the moment.

Requirements
------------
The application is intended to be built on any UNIX-ish system using a GCC-like C compiler.

SQLite is used as a storage engine. A version of SQLite 3.10 or greater will need to be provided.

Place the compiled sqlite3.o and sqlite3.h files in src/sqlite before running the build.

The relevant source files can be obtained from http://sqlite.org.

Principles
----------
* Tulip should be reasonably self-contained.
  * Ideally, the only dependencies would be standard POSIX headers and programs supplied by the OS.
  * SQLite is used as an opening book storage engine.
* Tulip should be readily portable to any UNIX-like system.
* Tulip should be buildable, ideally with a one-step invocation of 'make'.

Current Status
--------------
Currently, the foundations of the chess engine (board representation, move generation, transposition tables, etc) are being developed. The search engine will be implemented once these are complete.


Technical Concepts
------------------
### Internals
#### Core Chess Logic
Tulip uses a hybrid approach of both bitboard and array-based game state representation. This allows reasonably efficient attack detection and so on with the convenience of array-index board lookup. Tulip is designed to take full advantage of modern 64-bit processors in this regard.

#### Search
While not yet implemented, the plan is to employ a reasonably effective search based on the common Alpha/Beta search.

### Interop
#### Testing and Development
The automated tests and book generation are written in Python. Communication with Tulip is done via Unix-style text pipes. Many of Tulip's functionality (move generation, etc) can be invoked via command line options and the output is in a JSON structure. For example, to see the status of a game position given as a FEN string:
<pre>
src > ./tulip -gamestatus "7k/8/8/4r3/6b1/6n1/2PP1P2/4K3 w - - 0 1"
{"fenString": "7k/8/8/4r3/6b1/6n1/2PP1P2/4K3 w - - 0 1", "status": "whiteCheckmated"}
</pre>

#### Gameplay
The end goal is to implements the XBoard protocol for interface with front-ends such as XBoard, and perhaps the UCI protocol for more modern front-ends.
