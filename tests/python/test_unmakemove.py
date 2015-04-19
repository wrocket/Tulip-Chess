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

class TestMakeUnmakeMove(unittest.TestCase):
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

    def test_castle_white_kingside(self):
        result = self.make_unmake_move('4k3/8/8/8/8/8/8/4K2R w K - 0 1', 'e1g1')
        board = result['board']
        bitboards = result['bitboards']

        self.assertTrue('K', board['e1'])
        self.assertTrue('R', board['h1'])
        self.assertEqual('0000000000000080', bitboards['R'])
        self.assertEqual('0000000000000010', bitboards['K'])
        self.assertEqual('EFFFFFFFFFFFFF6F', bitboards['-'])

        self.assertTrue(result['castleWhiteKingside'])
        self.assertFalse(result['castleBlackKingside'])
        self.assertFalse(result['castleWhiteQueenside'])
        self.assertFalse(result['castleBlackQueenside'])
        self.assertEqual('white', result['toMove'])
        self.assertEqual('e8', result['blackKingSquare'])
        self.assertEqual('e1', result['whiteKingSquare'])

    def test_castle_white_queenside(self):
        result = self.make_unmake_move('4k3/8/8/8/8/8/8/R3K3 w Q - 0 1', 'e1c1')
        board = result['board']
        bitboards = result['bitboards']

        self.assertTrue('K', board['e1'])
        self.assertTrue('R', board['a1'])
        self.assertEqual('0000000000000001', bitboards['R'])
        self.assertEqual('0000000000000010', bitboards['K'])
        self.assertEqual('EFFFFFFFFFFFFFEE', bitboards['-'])

        self.assertFalse(result['castleWhiteKingside'])
        self.assertFalse(result['castleBlackKingside'])
        self.assertTrue(result['castleWhiteQueenside'])
        self.assertFalse(result['castleBlackQueenside'])
        self.assertEqual('white', result['toMove'])
        self.assertEqual('e8', result['blackKingSquare'])
        self.assertEqual('e1', result['whiteKingSquare'])

    def test_castle_black_kingside(self):
        result = self.make_unmake_move('4k2r/8/8/8/8/8/8/4K3 b k - 0 1', 'e8g8')
        board = result['board']
        bitboards = result['bitboards']

        self.assertTrue('k', board['e8'])
        self.assertTrue('r', board['h8'])
        self.assertEqual('8000000000000000', bitboards['r'])
        self.assertEqual('1000000000000000', bitboards['k'])
        self.assertEqual('6FFFFFFFFFFFFFEF', bitboards['-'])

        self.assertFalse(result['castleWhiteKingside'])
        self.assertTrue(result['castleBlackKingside'])
        self.assertFalse(result['castleWhiteQueenside'])
        self.assertFalse(result['castleBlackQueenside'])
        self.assertEqual('black', result['toMove'])
        self.assertEqual('e8', result['blackKingSquare'])
        self.assertEqual('e1', result['whiteKingSquare'])

    def test_castle_black_queenside(self):
        result = self.make_unmake_move('r3k3/8/8/8/8/8/8/4K3 b q - 0 1', 'e8c8')
        board = result['board']
        bitboards = result['bitboards']

        self.assertTrue('k', board['e8'])
        self.assertTrue('r', board['a8'])
        self.assertEqual('0100000000000000', bitboards['r'])
        self.assertEqual('1000000000000000', bitboards['k'])
        self.assertEqual('EEFFFFFFFFFFFFEF', bitboards['-'])

        self.assertFalse(result['castleWhiteKingside'])
        self.assertFalse(result['castleBlackKingside'])
        self.assertFalse(result['castleWhiteQueenside'])
        self.assertTrue(result['castleBlackQueenside'])
        self.assertEqual('black', result['toMove'])
        self.assertEqual('e8', result['blackKingSquare'])
        self.assertEqual('e1', result['whiteKingSquare'])

    def test_white_promote_no_captures(self):
        result = self.make_unmake_move('4k3/1P6/8/8/8/8/8/4K3 w - - 0 1', 'b7b8=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']

        self.assertEqual('P', board['b7'])

        self.assertEqual(0, piece_counts['Q'])
        self.assertEqual(1, piece_counts['P'])

        self.assertEqual('0002000000000000', bitboards['P'])
        self.assertEqual('0000000000000000', bitboards['Q'])
        self.assertEqual('EFFDFFFFFFFFFFEF', bitboards['-'])

    def test_black_promote_no_captures(self):
        result = self.make_unmake_move('4k3/8/8/8/8/8/1p6/4K3 b - - 0 1', 'b2b1=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']

        self.assertEqual('p', board['b2'])

        self.assertEqual(0, piece_counts['q'])
        self.assertEqual(1, piece_counts['p'])

        self.assertEqual('0000000000000200', bitboards['p'])
        self.assertEqual('0000000000000000', bitboards['q'])
        self.assertEqual('EFFFFFFFFFFFFDEF', bitboards['-'])

    def test_white_promote_with_captures(self):
        result = self.make_unmake_move('2b1k3/1P6/8/8/8/8/8/4K3 w KQkq - 0 1', 'b7c8=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']

        self.assertEqual('P', board['b7'])

        self.assertEqual(1, piece_counts['b'])
        self.assertEqual(0, piece_counts['Q'])
        self.assertEqual(1, piece_counts['P'])

        self.assertEqual('0400000000000000', bitboards['b'])
        self.assertEqual('0002000000000000', bitboards['P'])
        self.assertEqual('0000000000000000', bitboards['Q'])
        self.assertEqual('EBFDFFFFFFFFFFEF', bitboards['-'])

    def test_black_promote_with_captures(self):
        result = self.make_unmake_move('4k3/8/8/8/8/8/1p6/2B1K3 b - - 0 1', 'b2c1=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']

        self.assertEqual('p', board['b2'])

        self.assertEqual(1, piece_counts['B'])
        self.assertEqual(0, piece_counts['q'])
        self.assertEqual(1, piece_counts['p'])

        self.assertEqual('0000000000000004', bitboards['B'])
        self.assertEqual('0000000000000200', bitboards['p'])
        self.assertEqual('0000000000000000', bitboards['q'])
        self.assertEqual('EFFFFFFFFFFFFDEB', bitboards['-'])

    def test_white_ep_capture(self):
        result = self.make_unmake_move('4k3/8/8/4pP2/8/8/8/4K3 w KQkq e6 0 1', 'f5e6')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']

        self.assertEqual('p', board['e5'])
        self.assertEqual('P', board['f5'])

        self.assertEqual(1, piece_counts['p'])
        self.assertEqual(1, piece_counts['P'])

        self.assertEqual('e', result['epFile'])
        self.assertEqual('0000001000000000', bitboards['p'])
        self.assertEqual('0000002000000000', bitboards['P'])
        self.assertEqual('EFFFFFCFFFFFFFEF', bitboards['-'])

    def test_black_ep_capture(self):
        result = self.make_unmake_move('4k3/8/8/8/4pP2/8/8/4K3 b KQkq f3 0 1', 'e4f3')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']

        self.assertEqual('p', board['e4'])
        self.assertEqual('P', board['f4'])

        self.assertEqual(1, piece_counts['p'])
        self.assertEqual(1, piece_counts['P'])

        self.assertEqual('f', result['epFile'])
        self.assertEqual('0000000010000000', bitboards['p'])
        self.assertEqual('0000000020000000', bitboards['P'])
        self.assertEqual('EFFFFFFFCFFFFFEF', bitboards['-'])


if __name__ == '__main__':
    unittest.main()
