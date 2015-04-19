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

class TestNotation(unittest.TestCase):
    def setUp(self):
        None

    def get_moves(self, fen):
        result = call_tulip(['-listmoves', 'legal', fen])
        parsed_output = json.loads(result)
        moves = parsed_output['shortAlgebraicMoves']
        # Make sure there are no duplicates
        self.assertEqual(len(moves), len(set(moves)))
        return moves

    def test_endgame_position(self):
        moves = self.get_moves('5k2/8/5K2/8/8/1Q6/8/8 w - - 0 1')
        self.assertTrue('Qb8#' in moves)
        self.assertTrue('Qf7#' in moves)
        self.assertTrue('Qg8+' in moves)

    def test_castle_white_kingside(self):
        moves = self.get_moves('4k3/8/8/8/8/8/8/4K2R w K - 0 1')
        self.assertTrue('O-O' in moves)

    def test_castle_white_kingside_check(self):
        moves = self.get_moves('5k2/8/8/8/8/8/8/4K2R w K - 0 1')
        self.assertTrue('O-O+' in moves)

    def test_castle_white_kingside_checkmate(self):
        moves = self.get_moves('4rkr1/4p1p1/8/8/8/8/8/4K2R w K - 0 1')
        self.assertTrue('O-O#' in moves)

    def test_castle_white_queenside(self):
        moves = self.get_moves('4k3/8/8/8/8/8/8/R3K3 w Q - 0 1')
        self.assertTrue('O-O-O' in moves)

    def test_castle_white_queenside_check(self):
        moves = self.get_moves('3k4/8/8/8/8/8/8/R3K3 w Q - 0 1')
        self.assertTrue('O-O-O+' in moves)

    def test_castle_white_queenside_checkmate(self):
        moves = self.get_moves('2rkr3/2p1p3/8/8/8/8/8/R3K3 w Q - 0 1')
        self.assertTrue('O-O-O#' in moves)

    def test_castle_black_kingside(self):
        moves = self.get_moves('4k2r/8/8/8/8/8/8/4K3 b k - 0 1')
        self.assertTrue('O-O' in moves)

    def test_castle_black_kingside_check(self):
        moves = self.get_moves('4k2r/8/8/8/8/8/8/5K2 b k - 0 1')
        self.assertTrue('O-O+' in moves)

    def test_castle_black_kingside_checkmate(self):
        moves = self.get_moves('4k2r/8/8/8/8/8/4P1P1/4RKR1 b k - 0 1')
        self.assertTrue('O-O#' in moves)

    def test_castle_black_queenside(self):
        moves = self.get_moves('r3k3/8/8/8/8/8/8/4K3 b Kkq - 0 1')
        self.assertTrue('O-O-O' in moves)

    def test_castle_black_queenside_check(self):
        moves = self.get_moves('r3k3/8/8/8/8/8/8/3K4 b Kkq - 0 1')
        self.assertTrue('O-O-O+' in moves)

    def test_castle_black_queenside_checkmate(self):
        moves = self.get_moves('r3k3/8/8/8/8/8/2P1P3/2RKR3 b q - 0 1')
        self.assertTrue('O-O-O#' in moves)

    def test_ambiguous_moves_file_only(self):
        moves = self.get_moves('8/R7/4k3/8/8/8/2R1K3/8 w - - 0 1')
        self.assertTrue('Rcc7' in moves)
        self.assertTrue('Rca2' in moves)
        self.assertTrue('Rac7' in moves)
        self.assertTrue('Raa2' in moves)

    def test_ambiguous_moves_rank_only(self):
        moves = self.get_moves('8/5k2/R7/8/8/R4K2/8/8 w - - 0 1')
        self.assertTrue('R3a5' in moves)
        self.assertTrue('R3a4' in moves)
        self.assertTrue('R6a5' in moves)
        self.assertTrue('R6a4' in moves)

    def test_ambiguous_moves_rank_and_file(self):
        moves = self.get_moves('8/5k2/N3N3/8/N7/5K2/8/8 w - - 0 1')
        self.assertTrue('Na6c5' in moves)
        self.assertTrue('N4c5' in moves)
        self.assertTrue('Nec7' in moves)
        self.assertTrue('Nac7' in moves)
        self.assertTrue('Nec5' in moves)

    def test_pawn_moves_normal_white(self):
        moves = self.get_moves('8/4k3/8/5p2/4P3/8/4K3/8 w - - 0 1')
        self.assertTrue('e5' in moves)
        self.assertTrue('exf5' in moves)

    def test_pawn_moves_normal_black(self):
        moves = self.get_moves('8/4k3/8/5p2/4P3/8/4K3/8 b - - 0 1')
        self.assertTrue('f4' in moves)
        self.assertTrue('fxe4' in moves)

    def test_pawn_moves_ep_white(self):
        moves = self.get_moves('4k3/8/8/4Pp2/8/8/8/4K3 w - f6 0 1')
        self.assertTrue('exf6' in moves)

    def test_pawn_moves_ep_black(self):
        moves = self.get_moves('4k3/8/8/8/4Pp2/8/8/4K3 b - e3 0 1')
        self.assertTrue('fxe3' in moves)

    def test_pawn_move_capture_mate(self):
        moves = self.get_moves('3bkr2/3qpp2/1NP5/8/8/8/8/4K3 w - - 0 1')
        self.assertTrue('cxd7#' in moves)

    def test_pawn_promote(self):
        moves = self.get_moves('4k3/8/8/8/8/8/1p6/4K3 b - - 0 1')
        self.assertTrue('b1=Q+' in moves)
        self.assertTrue('b1=R+' in moves)
        self.assertTrue('b1=B' in moves)
        self.assertTrue('b1=N' in moves)

if __name__ == '__main__':
    unittest.main()
