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

class TestBasicMoveApplication(unittest.TestCase):
    def setUp(self):
        None

    def print_hex(self, n):
        h = hex(n).replace('0x', '')
        zeros = '0' * (16 - len(h))
        return zeros + h.upper()

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
        self.assertTrue('r' not in board.values())
        self.assertEqual(0, piece_counts['r'])
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
        self.assertEqual(61, piece_counts['-'])

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
        self.assertEqual(61, piece_counts['-'])

    def test_hash_simple_move(self):
        result = self.make_move('4k3/8/8/8/8/1Q6/8/4K3 w - - 0 1', 'b3c4')
        orig_hash = 0x618555f7c07711f8
        wqueen_b3 = 0xb23fe2851af11c0b
        wqueen_c4 = 0x4cf17ca889590e6e
        empty_b3 = 0x577f452b5eb3fc01
        empty_c4 = 0x620a3972d8fd6daf
        white_to_move = 0x77e554c3ddafb8c6
        mask = wqueen_c4 ^ wqueen_b3 ^ empty_c4 ^ empty_b3 ^ white_to_move
        self.assertEqual(self.print_hex(orig_hash ^ mask), result['hash'])

    def test_hash_white_kingside_castle(self):
        result = self.make_move('4k3/8/8/8/8/8/8/4K2R w K - 0 1', 'e1g1')
        orig_hash = 0xc74d8add75536c23

        mask = 0xc8df942dcdd9d3e3 # K on e1
        mask ^= 0x5165bdb57f3e5d48 # K on g1
        mask ^= 0xca56b19fbc285239 # Empty on e1
        mask ^= 0xd6a2781d1760be4e # Empty of g1
        mask ^= 0xe9f98801eded53f7 # Rook on h1
        mask ^= 0x394345456ac4fb80 # Rook on f1
        mask ^= 0x381c599bd1a38fd8 # Empty on h1
        mask ^= 0x70d7c44806003548 # Empty on f1
        mask ^= 0x77e554c3ddafb8c6 # White to move
        mask ^= 0x7ac3fac33fa2a123 # Original K castle flags
        mask ^= 0xdc0b25e9f28ae0dd # The "no" castle flag

        desired_hash = orig_hash ^ mask
        self.assertEqual(self.print_hex(desired_hash), result['hash'])

    def test_hash_white_queenside_castle(self):
        result = self.make_move('4k3/8/8/8/8/8/8/R3K3 w Q - 0 1', 'e1c1')
        orig_hash = 0x8bc0657deb4b3031

        mask = 0xc8df942dcdd9d3e3 # K on E1
        mask ^= 0x63dab1c7ac4a2570 # K on C1
        mask ^= 0xca56b19fbc285239 # Empty on E1
        mask ^= 0x0c5d579da3336099 # Empty on C1
        mask ^= 0xfc0f0e854e8a0fcf # Rook on A1
        mask ^= 0x17d426030a67bc7c # Rook on D1
        mask ^= 0x82c3e240217eb87c # empty on A1
        mask ^= 0x0d25aae264535842 # empty on D1
        mask ^= 0x77e554c3ddafb8c6 # White to move
        mask ^= 0x364e1563f20096ad # Original Q castle flag
        mask ^= 0xdc0b25e9f28ae0dd # The "no" castle flag

        desired_hash = orig_hash ^ mask
        self.assertEqual(self.print_hex(desired_hash), result['hash'])

    def test_hash_black_kingside_castle(self):
        result = self.make_move('4k2r/8/8/8/8/8/8/4K3 b k - 0 1', 'e8g8')
        orig_hash = 0xc0a922018b2ed9e9

        mask = 0x79ab469f2417a80c # K on e8
        mask ^= 0x498757567d0304f6 # K on g8
        mask ^= 0xe88948c88c965e82 # Empty on e8
        mask ^= 0x6e9b57f33fde478c # Empty of g8
        mask ^= 0x1c8a15dfafd8d934 # Rook on h8
        mask ^= 0x66c959bbb905a316 # Rook on f8
        mask ^= 0x5e8d1435579d024a # Empty on h8
        mask ^= 0x57cd66545b16183c # Empty on f8
        mask ^= 0x77e554c3ddafb8c6 # White to move
        mask ^= 0xf53df923d87bab7e # Original K castle flags
        mask ^= 0xdc0b25e9f28ae0dd # The "no" castle flag

        desired_hash = orig_hash ^ mask
        self.assertEqual(self.print_hex(desired_hash), result['hash'])

    def test_hash_black_queenside_castle(self):
        result = self.make_move('r3k3/8/8/8/8/8/8/4K3 b q - 0 1', 'e8c8')
        orig_hash = 0xcc8a5b5f6d786956

        mask = 0x79ab469f2417a80c # K on e8
        mask ^= 0x2c09faad8766ee13 # K on c8
        mask ^= 0xe88948c88c965e82 # Empty on e8
        mask ^= 0x44e15261d0800493 # Empty of c8
        mask ^= 0x04325c381abf132e # Rook on a8
        mask ^= 0xd9653edc79fd29b7 # Rook on d8
        mask ^= 0xf0aa7d665aa3a8fa # Empty on a8
        mask ^= 0xd53bcbf41a7b69ba # Empty on d8
        mask ^= 0x77e554c3ddafb8c6 # White to move
        mask ^= 0x06e17f8286747b6b # Original K castle flags
        mask ^= 0xdc0b25e9f28ae0dd # The "no" castle flag

        desired_hash = orig_hash ^ mask
        self.assertEqual(self.print_hex(desired_hash), result['hash'])

if __name__ == '__main__':
    unittest.main()
