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
    cmd = ['../../src/tulip', '--action']
    cmd.extend(args)
    out = subprocess.check_output(cmd)
    return out.decode('utf-8')

class TestBasicAttacks(unittest.TestCase):
    def setUp(self):
        None

    def run_attacks(self, fen, expected_attacks):
        result = call_tulip(['-listattacks', fen])
        parsed_output = json.loads(result)
        attacked_squares = parsed_output['attackedSquares']
        for sq in expected_attacks:
            self.assertTrue(sq in attacked_squares, 'Missing attack on %s' % sq)
        self.assertEqual(len(attacked_squares), len(expected_attacks))

    def test_01(self):
        squares_attacked_by_king = ['f1', 'g1', 'h1', 'f2', 'h2', 'f3', 'g3', 'h3']
        squares_attacked_by_pawn = ['c4', 'e4']
        squares_attacked_by_rook = ['d4', 'd3', 'c5', 'b5', 'e5', 'f5', 'g5', 'h5', 'd6', 'd7', 'd8']
        expected_attacks = set(squares_attacked_by_king + squares_attacked_by_pawn + squares_attacked_by_rook)
        self.run_attacks('8/6k1/8/1b1R4/8/3P4/6K1/8 w - - 0 1', expected_attacks)

    def test_02(self):
        squares_attacked_by_king = ['f8', 'h8', 'f7', 'g7', 'h7']
        squares_attacked_by_knight = ['a4', 'b5', 'd5', 'e4', 'e2', 'd1', 'b1', 'a2']
        squares_attacked_by_pawn = ['g1']
        expected_attacks = set(squares_attacked_by_king + squares_attacked_by_pawn + squares_attacked_by_knight)
        self.run_attacks('6k1/8/8/8/8/2n5/5K1p/8 b - - 0 1', expected_attacks)

    def test_03(self):
        squares_attacked_by_bishop = ['g5', 'h6', 'e5', 'e3', 'e1', 'd2', 'c1', 'g3', 'h2']
        squares_attacked_by_king = ['d1', 'd2', 'd3', 'e3', 'f3', 'f2', 'f1']
        expected_attacks = set(squares_attacked_by_bishop + squares_attacked_by_king)
        self.run_attacks('4k3/8/8/4b3/5B2/8/4K3/8 w - - 0 1', expected_attacks)

    def test_04(self):
        squares_attacked_by_king = ['c6', 'c5', 'c4', 'd6', 'd4', 'e6', 'e5', 'e4']
        squares_attacked_by_queen = ['f6', 'f7', 'f8', 'f4', 'f3', 'f2', 'f1', 'g5', 'h5', 'e5', 'd5']
        squares_attacked_by_queen.extend(['e6', 'd7', 'c8', 'g4', 'h3', 'g6', 'h7', 'e4', 'd3', 'c2', 'b1'])
        expected_attacks = set(squares_attacked_by_king + squares_attacked_by_queen)
        self.run_attacks('8/8/8/3k1q2/8/8/3K4/8 b - - 0 1', expected_attacks)

if __name__ == '__main__':
    unittest.main()
