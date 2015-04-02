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

import subprocess
import json
import unittest

def call_tulip(args):
    cmd = ['../../src/tulip']
    cmd.extend(args)
    out = subprocess.check_output(cmd)
    return out.decode('utf-8')

class TestBasicMoveList(unittest.TestCase):
    def setUp(self):
        None

    def listPsuedoMoves(self, fen):
        result = call_tulip(['-listmoves', 'pseudo', fen])
        parsed_output = json.loads(result)
        return parsed_output['moveList']

    def listPsuedoMovesDetails(self, fen):
        result = call_tulip(['-listmoves', 'pseudo', fen])
        parsed_output = json.loads(result)
        moves = parsed_output['moveDetails']
        return {m['move']: m for m in moves}

    def test_knight_01(self):
        moves = self.listPsuedoMoves('7K/8/4n3/2N5/8/3P4/8/k7 w - - 0 1')
        knight_moves = list(filter(lambda m: m.startswith('c5'), moves))
        self.assertEqual(7, len(knight_moves))
        self.assertTrue('c5d7' in moves)
        self.assertTrue('c5b7' in moves)
        self.assertTrue('c5a6' in moves)
        self.assertTrue('c5a4' in moves)
        self.assertTrue('c5b3' in moves)
        self.assertTrue('c5e4' in moves)
        self.assertTrue('c5e6' in moves)

    def test_knight_02(self):
        moves = self.listPsuedoMoves('7K/8/1N2P3/8/8/8/8/k7 w - - 0 1')
        knight_moves = list(filter(lambda m: m.startswith('b6'), moves))
        self.assertEqual(6, len(knight_moves))
        self.assertTrue('b6a8' in moves)
        self.assertTrue('b6d7' in moves)
        self.assertTrue('b6d5' in moves)
        self.assertTrue('b6c4' in moves)
        self.assertTrue('b6a4' in moves)
        self.assertTrue('b6c8' in moves)


    def test_initial_position(self):
        moves = self.listPsuedoMoves('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
        self.assertEqual(20, len(moves))
        self.assertTrue('b1a3' in moves)
        self.assertTrue('b1c3' in moves)
        self.assertTrue('g1f3' in moves)
        self.assertTrue('g1h3' in moves)
        self.assertTrue('a2a3' in moves)
        self.assertTrue('a2a4' in moves)
        self.assertTrue('b2b3' in moves)
        self.assertTrue('b2b4' in moves)
        self.assertTrue('c2c3' in moves)
        self.assertTrue('c2c4' in moves)
        self.assertTrue('d2d3' in moves)
        self.assertTrue('d2d4' in moves)
        self.assertTrue('e2e3' in moves)
        self.assertTrue('e2e4' in moves)
        self.assertTrue('f2f3' in moves)
        self.assertTrue('f2f4' in moves)
        self.assertTrue('g2g3' in moves)
        self.assertTrue('g2g4' in moves)
        self.assertTrue('h2h3' in moves)
        self.assertTrue('h2h4' in moves)


    def test_position_01(self):
        moves = self.listPsuedoMoves('5b1k/1pN4p/3P2p1/2pr1P2/Kp3p1P/5nn1/PPPB4/5RR1 w - - 1 41')
        self.assertEqual(35, len(moves))
        self.assertTrue('f1f2' in moves)
        self.assertTrue('f1f3' in moves)
        self.assertTrue('f1e1' in moves)
        self.assertTrue('f1d1' in moves)
        self.assertTrue('f1c1' in moves)
        self.assertTrue('f1b1' in moves)
        self.assertTrue('f1a1' in moves)
        self.assertTrue('g1g2' in moves)
        self.assertTrue('g1g3' in moves)
        self.assertTrue('g1h1' in moves)
        self.assertTrue('a2a3' in moves)
        self.assertTrue('b2b3' in moves)
        self.assertTrue('c2c3' in moves)
        self.assertTrue('c2c4' in moves)
        self.assertTrue('d2e3' in moves)
        self.assertTrue('d2f4' in moves)
        self.assertTrue('d2c3' in moves)
        self.assertTrue('d2b4' in moves)
        self.assertTrue('d2e1' in moves)
        self.assertTrue('d2c1' in moves)
        self.assertTrue('a4a5' in moves)
        self.assertTrue('a4a3' in moves)
        self.assertTrue('a4b4' in moves)
        self.assertTrue('a4b5' in moves)
        self.assertTrue('a4b3' in moves)
        self.assertTrue('h4h5' in moves)
        self.assertTrue('f5f6' in moves)
        self.assertTrue('f5g6' in moves)
        self.assertTrue('d6d7' in moves)
        self.assertTrue('c7d5' in moves)
        self.assertTrue('c7b5' in moves)
        self.assertTrue('c7e8' in moves)
        self.assertTrue('c7e6' in moves)
        self.assertTrue('c7a8' in moves)
        self.assertTrue('c7a6' in moves)

    def test_ep_white_01(self):
        moves = self.listPsuedoMovesDetails('2k5/8/8/5pP1/8/8/8/2K5 w - f6 0 1')
        self.assertTrue('g5f6' in moves.keys())
        epMove = moves['g5f6']
        self.assertEqual('epCapture', epMove['moveCode'])
        self.assertEqual('p', epMove['capturedPiece'])
        self.assertEqual('P', epMove['movingPiece'])

    def test_ep_white_02(self):
        moves = self.listPsuedoMovesDetails('2k5/8/8/1Pp5/8/8/8/2K5 w KQkq c6 0 1')
        self.assertTrue('b5c6' in moves.keys())
        epMove = moves['b5c6']
        self.assertEqual('epCapture', epMove['moveCode'])
        self.assertEqual('p', epMove['capturedPiece'])
        self.assertEqual('P', epMove['movingPiece'])

    def test_promote_pawn_no_captures(self):
        moves = self.listPsuedoMovesDetails('2k5/5P2/8/8/8/8/8/2K5 w - - 0 1')
        desired = {'f7f8=q': 'promoteQueen', 'f7f8=n': 'promoteKnight', 'f7f8=r': 'promoteRook', 'f7f8=b': 'promoteBishop'}
        for move, promote in desired.items():
            self.assertTrue(move in moves.keys(), move + ' not in move list.')
            m = moves[move]
            self.assertEqual(promote, m['moveCode'])
            self.assertEqual('-', m['capturedPiece'])
            self.assertEqual('P', m['movingPiece'])
        
    def test_promote_pawn_captures(self):
        moves = self.listPsuedoMovesDetails('2k1brn1/5P2/8/8/8/8/8/2K5 w - c6 0 1')
        desired = {'f7g8=q': 'promoteQueen', 'f7g8=n': 'promoteKnight', 'f7g8=r': 'promoteRook', 'f7g8=b': 'promoteBishop'}
        for move, promote in desired.items():
            self.assertTrue(move in moves.keys(), move + ' not in move list.')
            m = moves[move]
            self.assertEqual(promote, m['moveCode'])
            self.assertEqual('n', m['capturedPiece'])
            self.assertEqual('P', m['movingPiece'])

        desired = {'f7e8=q': 'promoteQueen', 'f7e8=n': 'promoteKnight', 'f7e8=r': 'promoteRook', 'f7e8=b': 'promoteBishop'}
        for move, promote in desired.items():
            self.assertTrue(move in moves.keys(), move + ' not in move list.')
            m = moves[move]
            self.assertEqual(promote, m['moveCode'])
            self.assertEqual('b', m['capturedPiece'])
            self.assertEqual('P', m['movingPiece'])

        # Make sure no f7f8 moves exist.
        self.assertEqual(0, len(list(filter(lambda m: m.startswith('f7f8'), moves.keys()))))

if __name__ == '__main__':
    unittest.main()
