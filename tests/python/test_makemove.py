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

    def make_move(self, fen, move):
        result = call_tulip(['-makemove', move, fen])
        parsed_output = json.loads(result)
        return parsed_output['resultingState']

    def test_initial_position_nf3(self):
        result = self.make_move('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 'g1f3')
        self.assertEqual('black', result['toMove'])
        self.assertEqual(1, result['halfMoveCount'])
        self.assertEqual(1, result['fiftyMoveCount'])
        board = result['board']
        self.assertEqual(32, len(board.keys()))
        self.assertTrue('g1' not in board.keys())
        self.assertEqual('N', board['f3']) 
        self.assertEqual('0000000000200002', result['bitboards']['N'])
        self.assertEqual('0000FFFFFFDF0040', result['bitboards']['-'])
        self.assertEqual('none', result['epFile'])

    def test_initial_position_e2e4(self):
        result = self.make_move('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1', 'e2e4')
        self.assertEqual('black', result['toMove'])
        self.assertEqual(1, result['halfMoveCount'])
        self.assertEqual(0, result['fiftyMoveCount'])
        board = result['board']
        self.assertEqual(32, len(board.keys()))
        self.assertTrue('e2' not in board.keys())
        self.assertEqual('P', board['e4'])
        self.assertEqual('000000001000EF00', result['bitboards']['P'])
        self.assertEqual('0000FFFFEFFF1000', result['bitboards']['-'])
        self.assertEqual('e', result['epFile'])

    def test_basic_capture_01(self):
        result = self.make_move('8/4k3/2r5/8/1N2K3/8/8/8 w - - 13 1', 'b4c6') # note the fifty-move count of 13
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(0, result['fiftyMoveCount'])   # reset the fifty-move count on capture
        self.assertEqual(3, len(board))
        self.assertTrue('b4' not in board.keys())
        self.assertEqual('N', board['c6'])
        self.assertTrue('n' not in board.values())
        self.assertEqual(0, piece_counts['n'])
        self.assertEqual(1, piece_counts['N'])
        self.assertEqual(61, piece_counts['-'])
        self.assertEqual('0000000000000000', bitboards['n'])
        self.assertEqual('0000040000000000', bitboards['N'])
        self.assertEqual('FFEFFBFFEFFFFFFF', bitboards['-'])
        self.assertEqual('none', result['epFile'])

    def test_move_pawn_resets_counter(self):
        result = self.make_move('8/4k3/2r5/8/1P2K3/8/8/8 w - - 26 1', 'b4b5')
        self.assertEqual(0, result['fiftyMoveCount'])

    def test_white_castle_kingside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1', 'e1g1')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('R', board['f1'])
        self.assertEqual('R', board['a1'])
        self.assertEqual('K', board['g1'])
        self.assertTrue('h1' not in board.keys())
        self.assertFalse(result['castleWhiteKingside'])
        self.assertFalse(result['castleWhiteQueenside'])
        self.assertTrue(result['castleBlackKingside'])
        self.assertTrue(result['castleBlackQueenside'])
        self.assertEqual('0000000000000021', bitboards['R'])
        self.assertEqual('0000000000000040', bitboards['K'])
        self.assertEqual('6E00FFFFFFFF009E', bitboards['-'])
        self.assertEqual('g1', result['whiteKingSquare'])
        self.assertEqual('e8', result['blackKingSquare'])


    def test_white_castle_queenside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1', 'e1c1')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('R', board['h1'])
        self.assertEqual('R', board['d1'])
        self.assertEqual('K', board['c1'])
        self.assertTrue('a1' not in board.keys())
        self.assertFalse(result['castleWhiteKingside'])
        self.assertFalse(result['castleWhiteQueenside'])
        self.assertTrue(result['castleBlackKingside'])
        self.assertTrue(result['castleBlackQueenside'])
        self.assertEqual('0000000000000088', bitboards['R'])
        self.assertEqual('0000000000000004', bitboards['K'])
        self.assertEqual('6E00FFFFFFFF0073', bitboards['-'])
        self.assertEqual('c1', result['whiteKingSquare'])
        self.assertEqual('e8', result['blackKingSquare'])

    def test_black_castle_queenside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1', 'e8c8')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('r', board['h8'])
        self.assertEqual('r', board['d8'])
        self.assertEqual('k', board['c8'])
        self.assertTrue('a8' not in board.keys())
        self.assertTrue(result['castleWhiteKingside'])
        self.assertTrue(result['castleWhiteQueenside'])
        self.assertFalse(result['castleBlackKingside'])
        self.assertFalse(result['castleBlackQueenside'])
        self.assertEqual('8800000000000000', bitboards['r'])
        self.assertEqual('0400000000000000', bitboards['k'])
        self.assertEqual('7300FFFFFFFF006E', bitboards['-'])
        self.assertEqual('e1', result['whiteKingSquare'])
        self.assertEqual('c8', result['blackKingSquare'])

    def test_black_castle_kingside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1', 'e8g8')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('r', board['a8'])
        self.assertEqual('r', board['f8'])
        self.assertEqual('k', board['g8'])
        self.assertTrue('h8' not in board.keys())
        self.assertTrue(result['castleWhiteKingside'])
        self.assertTrue(result['castleWhiteQueenside'])
        self.assertFalse(result['castleBlackKingside'])
        self.assertFalse(result['castleBlackQueenside'])
        self.assertEqual('2100000000000000', bitboards['r'])
        self.assertEqual('4000000000000000', bitboards['k'])
        self.assertEqual('9E00FFFFFFFF006E', bitboards['-'])
        self.assertEqual('e1', result['whiteKingSquare'])
        self.assertEqual('g8', result['blackKingSquare'])

    def test_white_move_rook_kingside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1', 'h1g1')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('R', board['g1'])
        self.assertEqual('R', board['a1'])
        self.assertEqual('K', board['e1'])
        self.assertTrue('h1' not in board.keys())
        self.assertFalse(result['castleWhiteKingside'])
        self.assertTrue(result['castleWhiteQueenside'])
        self.assertTrue(result['castleBlackKingside'])
        self.assertTrue(result['castleBlackQueenside'])
        self.assertEqual('0000000000000041', bitboards['R'])
        self.assertEqual('0000000000000010', bitboards['K'])
        self.assertEqual('6E00FFFFFFFF00AE', bitboards['-'])

    def test_white_move_rook_queenside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1', 'a1b1')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('R', board['h1'])
        self.assertEqual('R', board['b1'])
        self.assertEqual('K', board['e1'])
        self.assertTrue('a1' not in board.keys())
        self.assertTrue(result['castleWhiteKingside'])
        self.assertFalse(result['castleWhiteQueenside'])
        self.assertTrue(result['castleBlackKingside'])
        self.assertTrue(result['castleBlackQueenside'])
        self.assertEqual('0000000000000082', bitboards['R'])
        self.assertEqual('0000000000000010', bitboards['K'])
        self.assertEqual('6E00FFFFFFFF006D', bitboards['-'])

    def test_black_move_rook_kingside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1', 'h8g8')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('r', board['g8'])
        self.assertEqual('r', board['a8'])
        self.assertEqual('k', board['e8'])
        self.assertTrue('h8' not in board.keys())
        self.assertTrue(result['castleWhiteKingside'])
        self.assertTrue(result['castleWhiteQueenside'])
        self.assertFalse(result['castleBlackKingside'])
        self.assertTrue(result['castleBlackQueenside'])
        self.assertEqual('4100000000000000', bitboards['r'])
        self.assertEqual('1000000000000000', bitboards['k'])
        self.assertEqual('AE00FFFFFFFF006E', bitboards['-'])

    def test_black_move_rook_queenside(self):
        result = self.make_move('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1', 'a8b8')
        board = result['board']
        bitboards = result['bitboards']
        self.assertEqual(22, len(board))
        self.assertEqual('r', board['b8'])
        self.assertEqual('r', board['h8'])
        self.assertEqual('k', board['e8'])
        self.assertTrue('a8' not in board.keys())
        self.assertTrue(result['castleWhiteKingside'])
        self.assertTrue(result['castleWhiteQueenside'])
        self.assertTrue(result['castleBlackKingside'])
        self.assertFalse(result['castleBlackQueenside'])
        self.assertEqual('8200000000000000', bitboards['r'])
        self.assertEqual('1000000000000000', bitboards['k'])
        self.assertEqual('6D00FFFFFFFF006E', bitboards['-'])

    def test_promote_pawn_white_q_no_capture(self):
        result = self.make_move('8/5P2/8/8/1k6/8/2K5/8 w - - 0 1', 'f7f8=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual('Q', board['f8'])
        self.assertEqual('k', board['b4'])
        self.assertEqual('K', board['c2'])
        self.assertEqual('2000000000000000', bitboards['Q'])
        self.assertEqual('0000000000000000', bitboards['P'])
        self.assertEqual('DFFFFFFFFDFFFBFF', bitboards['-'])
        self.assertEqual(0, piece_counts['P'])
        self.assertEqual(1, piece_counts['Q'])

    def test_promote_pawn_white_n_no_capture(self):
        result = self.make_move('8/5P2/8/8/1k6/8/2K5/8 w - - 0 1', 'f7f8=n')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual('N', board['f8'])
        self.assertEqual('k', board['b4'])
        self.assertEqual('K', board['c2'])
        self.assertEqual('2000000000000000', bitboards['N'])
        self.assertEqual('0000000000000000', bitboards['P'])
        self.assertEqual('0000000000000000', bitboards['Q'])
        self.assertEqual('DFFFFFFFFDFFFBFF', bitboards['-'])
        self.assertEqual(0, piece_counts['P'])
        self.assertEqual(1, piece_counts['N'])

    def test_promote_pawn_white_q_with_capture(self):
        result = self.make_move('5r2/6P1/8/8/1k6/8/2K5/8 w - - 0 1', 'g7f8=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual('Q', board['f8'])
        self.assertEqual('k', board['b4'])
        self.assertEqual('K', board['c2'])
        self.assertEqual('2000000000000000', bitboards['Q'])
        self.assertEqual('0000000000000000', bitboards['P'])
        self.assertEqual('0000000000000000', bitboards['n'])
        self.assertEqual('DFFFFFFFFDFFFBFF', bitboards['-'])
        self.assertEqual(0, piece_counts['P'])
        self.assertEqual(0, piece_counts['r'])
        self.assertEqual(1, piece_counts['Q'])

    def test_promote_pawn_black_q_no_capture(self):
        result = self.make_move('8/8/8/8/1k6/8/2K2p2/8 b - - 0 1', 'f2f1=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual('q', board['f1'])
        self.assertEqual('k', board['b4'])
        self.assertEqual('K', board['c2'])
        self.assertEqual('0000000000000020', bitboards['q'])
        self.assertEqual('0000000000000000', bitboards['p'])
        self.assertEqual('FFFFFFFFFDFFFBDF', bitboards['-'])
        self.assertEqual(0, piece_counts['p'])
        self.assertEqual(1, piece_counts['q'])

    def test_promote_pawn_black_n_no_capture(self):
        result = self.make_move('8/8/8/8/1k6/8/2K2p2/8 b - - 0 1', 'f2f1=n')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual('n', board['f1'])
        self.assertEqual('k', board['b4'])
        self.assertEqual('K', board['c2'])
        self.assertEqual('0000000000000020', bitboards['n'])
        self.assertEqual('0000000000000000', bitboards['p'])
        self.assertEqual('FFFFFFFFFDFFFBDF', bitboards['-'])
        self.assertEqual(0, piece_counts['p'])
        self.assertEqual(1, piece_counts['n'])

    def test_promote_pawn_black_q_with_capture(self):
        result = self.make_move('8/8/8/8/1k6/8/2K3p1/5R2 b - - 0 1', 'g2f1=q')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual('q', board['f1'])
        self.assertEqual('k', board['b4'])
        self.assertEqual('K', board['c2'])
        self.assertEqual('0000000000000020', bitboards['q'])
        self.assertEqual('0000000000000000', bitboards['p'])
        self.assertEqual('0000000000000000', bitboards['R'])
        self.assertEqual('FFFFFFFFFDFFFBDF', bitboards['-'])
        self.assertEqual(0, piece_counts['p'])
        self.assertEqual(0, piece_counts['R'])
        self.assertEqual(1, piece_counts['q'])

    def test_enpassant_white(self):
        result = self.make_move('4k3/8/8/3Pp3/8/8/8/4K3 w - e6 0 1', 'd5e6')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual(0, piece_counts['p'])
        self.assertEqual(1, piece_counts['P'])
        self.assertEqual('0000000000000000', bitboards['p'])
        self.assertEqual('0000100000000000', bitboards['P'])
        self.assertEqual('EFFFEFFFFFFFFFEF', bitboards['-'])
        self.assertEqual('none', result['epFile'])

    def test_enpassant_black(self):
        result = self.make_move('4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1', 'd4e3')
        board = result['board']
        bitboards = result['bitboards']
        piece_counts = result['pieceCounts']
        self.assertEqual(3, len(board))
        self.assertEqual(0, piece_counts['P'])
        self.assertEqual(1, piece_counts['p'])
        self.assertEqual('0000000000100000', bitboards['p'])
        self.assertEqual('0000000000000000', bitboards['P'])
        self.assertEqual('EFFFFFFFFFEFFFEF', bitboards['-'])
        self.assertEqual('none', result['epFile'])

if __name__ == '__main__':
    unittest.main()
