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

    def make_unmake_move(self, fen, move):
        result = call_tulip(['-makeunmake', move, fen])
        return json.loads(result)

    def test_initial_position_e2e4(self):
        result = self.make_unmake_move('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 'e2e4')
        bitboards = result['bitboards']
        board = result['board']
        self.assertFalse('e4' in board.keys())
        self.assertEqual('P', board['e2'])
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
        self.assertEqual('white', result['toMove'])
        self.assertEqual('e8', result['blackKingSquare'])
        self.assertEqual('e1', result['whiteKingSquare'])
        self.assertTrue(result['castleWhiteKingside'])
        self.assertTrue(result['castleBlackKingside'])
        self.assertTrue(result['castleWhiteQueenside'])
        self.assertTrue(result['castleBlackQueenside'])
        self.assertEqual(0, result['fiftyMoveCount'])
        self.assertEqual('none', result['epFile'])
        piece_counts = result['pieceCounts']
        for p in ['P', 'p']:
            self.assertEqual(8, piece_counts[p])
        for p in ['R', 'r', 'N', 'n', 'B', 'b']:
            self.assertEqual(2, piece_counts[p])
        for p in ['K', 'k', 'Q', 'q']:
            self.assertEqual(1, piece_counts[p])
        self.assertEqual(32, piece_counts['-'])

    def test_simple_capture(self):
        result = self.make_unmake_move('8/4k3/2r5/8/1N2K3/8/8/8 w - - 13 1', 'b4c6')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(4, len(board))
        self.assertTrue('b4' in board.keys())
        self.assertEqual('r', board['c6'])
        self.assertEqual('N', board['b4'])
        self.assertEqual(1, piece_counts['r'])
        self.assertEqual(1, piece_counts['N'])
        self.assertEqual(60, piece_counts['-'])
        self.assertEqual('0000000000000000', bitboards['n'])
        self.assertEqual('0000000002000000', bitboards['N'])
        self.assertEqual('FFEFFBFFEDFFFFFF', bitboards['-'])


if __name__ == '__main__':
    unittest.main()
