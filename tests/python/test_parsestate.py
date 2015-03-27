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

class TestBasicStateParse(unittest.TestCase):
    def setUp(self):
        None

    def parseState(self, fen):
        result = call_tulip(['-printstate', fen])
        parsed_output = json.loads(result)
        return parsed_output

    def test_initial_position_board_position(self):
        state = self.parseState('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
        board = state['board']
        
        for sq in ['a2', 'b2', 'c2', 'd2', 'e2', 'f2', 'g2', 'h2']:
            self.assertEqual('P', board[sq])
        self.assertEqual('R', board['a1'])
        self.assertEqual('N', board['b1'])
        self.assertEqual('B', board['c1'])
        self.assertEqual('Q', board['d1'])
        self.assertEqual('K', board['e1'])
        self.assertEqual('B', board['f1'])
        self.assertEqual('N', board['g1'])
        self.assertEqual('R', board['h1'])

        for sq in ['a7', 'b7', 'c7', 'd7', 'e7', 'f7', 'g7', 'h7']:
            self.assertEqual('p', board[sq])
        self.assertEqual('r', board['a8'])
        self.assertEqual('n', board['b8'])
        self.assertEqual('b', board['c8'])
        self.assertEqual('q', board['d8'])
        self.assertEqual('k', board['e8'])
        self.assertEqual('b', board['f8'])
        self.assertEqual('n', board['g8'])
        self.assertEqual('r', board['h8'])

    def test_initial_position_piece_counts(self):
        state = self.parseState('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
        piece_counts = state['pieceCounts']
        for p in ['P', 'p']:
            self.assertEqual(8, piece_counts[p])
        for p in ['R', 'r', 'N', 'n', 'B', 'b']:
            self.assertEqual(2, piece_counts[p])
        for p in ['K', 'k', 'Q', 'q']:
            self.assertEqual(1, piece_counts[p])
        self.assertEqual(32, piece_counts['-'])

    def test_initial_position_bitboards(self):
        state = self.parseState('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
        bitboards = state['bitboards']
        self.assertEqual('000000000000FF00', bitboards['P'])
        self.assertEqual('00FF000000000000', bitboards['p'])
        self.assertEqual('0000000000000042', bitboards['N'])
        self.assertEqual('4200000000000000', bitboards['n'])
        self.assertEqual('0000000000000024', bitboards['B'])
        self.assertEqual('2400000000000000', bitboards['b'])
        self.assertEqual('0000000000000081', bitboards['R'])
        self.assertEqual('8100000000000000', bitboards['r'])
        self.assertEqual('0000000000000008', bitboards['Q'])
        self.assertEqual('0800000000000000', bitboards['q'])
        self.assertEqual('0000000000000010', bitboards['K'])
        self.assertEqual('1000000000000000', bitboards['k'])
        self.assertEqual('0000FFFFFFFF0000', bitboards['-'])

    def test_initial_position_basics(self):
        state = self.parseState('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
        self.assertEqual('white', state['toMove'])
        self.assertEqual('e8', state['blackKingSquare'])
        self.assertEqual('e1', state['whiteKingSquare'])
        self.assertTrue(state['castleWhiteKingside'])
        self.assertTrue(state['castleBlackKingside'])
        self.assertTrue(state['castleWhiteQueenside'])
        self.assertTrue(state['castleBlackQueenside'])
        
        self.assertEqual('none', state['epFile'])

if __name__ == '__main__':
    unittest.main()
