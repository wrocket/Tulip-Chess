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

    def get_moves(self, fen, mode):
        result = call_tulip(['-listmoves', mode, fen])
        parsed_output = json.loads(result)
        moves = parsed_output['moveList']
        # Make sure there are no duplicates
        self.assertEqual(len(moves), len(set(moves)))
        return moves

    def listPsuedoMoves(self, fen):
        return self.get_moves(fen, 'pseudo')

    def listLegalMoves(self, fen):
        return self.get_moves(fen, 'legal')

    def getMoveFromDetails(self, move_str, moves):
        self.assertTrue(move_str in moves, move_str + ' not in move list: ' + str(', '.join(sorted(moves.keys()))))
        return moves[move_str]

    def listPsuedoMovesDetails(self, fen):
        result = call_tulip(['-listmoves', 'pseudo', fen])
        parsed_output = json.loads(result)
        moves = parsed_output['moveDetails']
        move_dict = {m['move']: m for m in moves}
        # Make sure there are no duplicates
        self.assertEqual(len(move_dict.keys()), len(moves))
        return move_dict

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

    def test_knight_03(self):
        moves = self.listPsuedoMoves('7K/8/1n6/3p4/2P5/8/8/k7 b - - 0 1')
        knight_moves = list(filter(lambda m: m.startswith('b6'), moves))
        self.assertEqual(5, len(knight_moves))
        self.assertTrue('b6a8' in moves)
        self.assertTrue('b6d7' in moves)
        self.assertTrue('b6c4' in moves)
        self.assertTrue('b6a4' in moves)
        self.assertTrue('b6c8' in moves)

    def test_bking_01(self):
        moves = self.listPsuedoMovesDetails('8/8/8/3p4/2k2K2/2P5/8/8 b - e3 0 1')
        expected = ['c4c5', 'c4c3', 'c4b4', 'c4b3', 'c4b5', 'c4d3', 'c4d4']
        self.assertEqual(len(expected), len(list(filter(lambda m: m.startswith('c4'), moves.keys()))))
        for m in expected:
            move = self.getMoveFromDetails(m, moves)
            self.assertEqual('k', move['movingPiece'])
        capture = self.getMoveFromDetails('c4c3', moves)
        self.assertEqual('P', capture['capturedPiece'])

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

    def test_initial_position_after_e4(self):
        moves = self.listPsuedoMoves('rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1')
        for m in ['a7a6', 'b7b6', 'c7c6', 'd7d6', 'e7e6', 'f7f6', 'g7g6', 'h7h6']:
            self.assertTrue(m in moves)
        for m in ['a7a5', 'b7b5', 'c7c5', 'd7d5', 'e7e5', 'f7f5', 'g7g5', 'h7h5']:
            self.assertTrue(m in moves)
        for m in ['b8c6', 'b8a6', 'g8f6', 'g8h6']:
            self.assertTrue(m in moves)
        self.assertEqual(20, len(moves))

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
        m = self.getMoveFromDetails('g5f6', moves)
        self.assertEqual('epCapture', m['moveCode'])
        self.assertEqual('p', m['capturedPiece'])
        self.assertEqual('P', m['movingPiece'])

    def test_ep_white_02(self):
        moves = self.listPsuedoMovesDetails('2k5/8/8/1Pp5/8/8/8/2K5 w KQkq c6 0 1')
        m = self.getMoveFromDetails('b5c6', moves)
        self.assertEqual('epCapture', m['moveCode'])
        self.assertEqual('p', m['capturedPiece'])
        self.assertEqual('P', m['movingPiece'])

    def test_ep_black_01(self):
        moves = self.listPsuedoMovesDetails('4k3/8/8/8/4Pp2/8/8/4K3 b - e3 0 1')
        m = self.getMoveFromDetails('f4e3', moves)
        self.assertEqual('epCapture', m['moveCode'])
        self.assertEqual('P', m['capturedPiece'])
        self.assertEqual('p', m['movingPiece'])

    def test_ep_black_02(self):
        moves = self.listPsuedoMovesDetails('4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1')
        m = self.getMoveFromDetails('d4e3', moves)
        self.assertEqual('epCapture', m['moveCode'])
        self.assertEqual('P', m['capturedPiece'])
        self.assertEqual('p', m['movingPiece'])

    def test_promote_wpawn_no_captures(self):
        moves = self.listPsuedoMovesDetails('2k5/5P2/8/8/8/8/8/2K5 w - - 0 1')
        desired = {'f7f8=q': 'promoteQueen', 'f7f8=n': 'promoteKnight', 'f7f8=r': 'promoteRook', 'f7f8=b': 'promoteBishop'}
        for move, promote in desired.items():
            m = self.getMoveFromDetails(move, moves)
            self.assertEqual(promote, m['moveCode'])
            self.assertEqual('-', m['capturedPiece'])
            self.assertEqual('P', m['movingPiece'])

    def test_promote_bpawn_no_captures(self):
        moves = self.listPsuedoMovesDetails('2k5/8/8/8/8/8/5p2/2K5 b - - 0 1')
        desired = {'f2f1=q': 'promoteQueen', 'f2f1=n': 'promoteKnight', 'f2f1=r': 'promoteRook', 'f2f1=b': 'promoteBishop'}
        for move, promote in desired.items():
            m = self.getMoveFromDetails(move, moves)
            self.assertEqual(promote, m['moveCode'])
            self.assertEqual('-', m['capturedPiece'])
            self.assertEqual('p', m['movingPiece'])

    def test_promote_wpawn_captures(self):
        moves = self.listPsuedoMovesDetails('2k1brn1/5P2/8/8/8/8/8/2K5 w - c6 0 1')
        desired = {'f7g8=q': 'promoteQueen', 'f7g8=n': 'promoteKnight', 'f7g8=r': 'promoteRook', 'f7g8=b': 'promoteBishop'}
        for move, promote in desired.items():
            m = self.getMoveFromDetails(move, moves)
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

    def test_promote_bpawn_captures(self):
        moves = self.listPsuedoMovesDetails('2k5/8/8/8/8/8/5p2/2K1BRN1 b - c6 0 1')
        desired = {'f2g1=q': 'promoteQueen', 'f2g1=n': 'promoteKnight', 'f2g1=r': 'promoteRook', 'f2g1=b': 'promoteBishop'}
        for move, promote in desired.items():
            m = self.getMoveFromDetails(move, moves)
            self.assertEqual(promote, m['moveCode'])
            self.assertEqual('N', m['capturedPiece'])
            self.assertEqual('p', m['movingPiece'])

        desired = {'f2e1=q': 'promoteQueen', 'f2e1=n': 'promoteKnight', 'f2e1=r': 'promoteRook', 'f2e1=b': 'promoteBishop'}
        for move, promote in desired.items():
            self.assertTrue(move in moves.keys(), move + ' not in move list.')
            m = moves[move]
            self.assertEqual(promote, m['moveCode'])
            self.assertEqual('B', m['capturedPiece'])
            self.assertEqual('p', m['movingPiece'])

        # Make sure no f2f1 moves exist.
        self.assertEqual(0, len(list(filter(lambda m: m.startswith('f2f1'), moves.keys()))))

    def test_white_castle_noproblems(self):
        moves = self.listPsuedoMovesDetails('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1')
        for move in ['e1g1', 'e1c1']:
            castle = self.getMoveFromDetails(move, moves)
            self.assertEqual('none', castle['moveCode'])
            self.assertEqual('-', castle['capturedPiece'])
            self.assertEqual('K', castle['movingPiece'])

    def test_black_castle_noproblems(self):
        moves = self.listPsuedoMovesDetails('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1')
        for move in ['e8g8', 'e8c8']:
            castle = self.getMoveFromDetails(move, moves)
            self.assertEqual('none', castle['moveCode'])
            self.assertEqual('-', castle['capturedPiece'])
            self.assertEqual('k', castle['movingPiece'])

    def test_white_castle_blocked(self):
        moves = self.listPsuedoMovesDetails('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RN2K1NR w KQkq - 0 1')
        self.assertTrue('e1g1' not in moves.keys(), 'e1g1 incorrectly in move list.')
        self.assertTrue('e1c1' not in moves.keys(), 'e1c1 incorrectly in move list.')

    def test_black_castle_blocked(self):
        moves = self.listPsuedoMovesDetails('rn2k1nr/pppppppp/8/8/8/8/PPPPPPPP/R3K1NR b KQkq - 0 1')
        self.assertTrue('e8g8' not in moves.keys(), 'e8g8 incorrectly in move list.')
        self.assertTrue('e8c8' not in moves.keys(), 'e8c8 incorrectly in move list.')

    def test_white_castle_move_through_check(self):
        moves = self.listPsuedoMovesDetails('r3k2r/pppppppp/3q4/8/8/6n1/PPP1PPPP/R3K2R w KQkq - 0 1')
        self.assertTrue('e1g1' not in moves.keys(), 'e1g1 incorrectly in move list.')
        self.assertTrue('e1c1' not in moves.keys(), 'e1c1 incorrectly in move list.')

    def test_white_castle_move_out_of_check(self):
        moves = self.listPsuedoMovesDetails('r3k2r/ppp1pppp/8/8/1b6/8/PPP1PPPP/R3K2R w KQkq - 0 1')
        self.assertTrue('e1g1' not in moves.keys(), 'e1g1 incorrectly in move list.')
        self.assertTrue('e1c1' not in moves.keys(), 'e1c1 incorrectly in move list.')

    def test_black_castle_move_through_check(self):
        moves = self.listPsuedoMovesDetails('r3k2r/ppp1pppp/6N1/8/8/8/PPPQPPPP/R3K2R b KQkq - 0 1')
        self.assertTrue('e1g8' not in moves.keys(), 'e8g8 incorrectly in move list.')
        self.assertTrue('e1c8' not in moves.keys(), 'e8c8 incorrectly in move list.')

    def test_black_castle_move_out_of_check(self):
        moves = self.listPsuedoMovesDetails('r3k2r/ppp1pppp/8/1B6/8/8/PPP1PPPP/R3K2R b KQkq - 0 1')
        self.assertTrue('e1g8' not in moves.keys(), 'e8g8 incorrectly in move list.')
        self.assertTrue('e1c8' not in moves.keys(), 'e8c8 incorrectly in move list.')

    def test_white_castle_rook_can_be_threatened(self):
        moves = self.listPsuedoMovesDetails('r3k2r/pppppppp/1q6/8/8/6n1/P1P1PPPP/R3K2R w KQkq - 0 1')
        self.getMoveFromDetails('e1c1', moves)

    def test_black_castle_rook_can_be_threatened(self):
        moves = self.listPsuedoMovesDetails('r3k2r/p1pppppp/8/8/8/1Q4n1/PPP1PPPP/R3K2R b KQkq - 0 1')
        self.getMoveFromDetails('e8c8', moves)

    def test_white_castle_no_castling(self):
        moves = self.listPsuedoMovesDetails('r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w kq c6 0 1')
        self.assertTrue('e1g1' not in moves.keys(), 'e1g1 incorrectly in move list.')
        self.assertTrue('e1c1' not in moves.keys(), 'e1c1 incorrectly in move list.')

    def test_black_pawn_blocked(self):
        moves = self.listLegalMoves('6k1/6p1/6K1/8/8/8/8/8 b - - 0 1')
        self.assertTrue('g8f8' in moves)
        self.assertTrue('g8h8' in moves)
        self.assertEqual(2, len(moves))

    def test_white_pawn_blocked(self):
        moves = self.listLegalMoves('8/8/8/8/8/6k1/6P1/6K1 w - - 0 1')
        self.assertTrue('g1f1' in moves)
        self.assertTrue('g1h1' in moves)
        self.assertEqual(2, len(moves))

    def test_legal_moves_01(self):
        moves = self.listLegalMoves('5k2/8/5K2/8/8/1Q6/8/8 w - - 0 1')
        self.assertFalse('f6f7' in moves)
        self.assertFalse('f7e7' in moves)
        self.assertFalse('f6g7' in moves)

if __name__ == '__main__':
    unittest.main()
