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

class SearchResult:
    def __init__(self, move, score, move_scores=[]):
        self.move = move
        self.score = score
        self.move_scores = move_scores

class TestSearch(unittest.TestCase):
    def setUp(self):
        None

    def assert_score_is_checkmate(self, score):
        self.assertTrue(score >= 9000)

    def assert_is_checkmate(self, result):
        self.assert_score_is_checkmate(result.score)

    def get_result(self, fen):
        result = call_tulip(['-simplesearch', fen])
        parsed_output = json.loads(result)
        json_obj = parsed_output['searchResult']
        parsed_scores = [(x['move'], x['score']) for x in json_obj['rootNodeScores']]
        sorted_scores = sorted(parsed_scores, key=lambda x: -1* x[1]) # sort move scores by score
        return SearchResult(move=json_obj['move'], score=json_obj['score'], move_scores=sorted_scores)

    def get_mvv_lva(self, fen):
        result = call_tulip(['-ordermoves', fen])
        parsed_output = json.loads(result)
        return parsed_output['shortAlgebraicMoves']

    def test_easy_backrank(self):
        result = self.get_result('6k1/5ppp/8/8/8/8/4R3/4K3 w - - 0 1')
        self.assertEqual('Re8#', result.move)
        self.assert_is_checkmate(result)

    def test_easy_smothered_mate_in_two(self):
        result = self.get_result('r6k/6pp/4Q3/6N1/8/8/B7/3K4 w KQkq - 0 1')
        self.assertEqual('Qg8+', result.move)
        self.assert_is_checkmate(result)

    def test_dont_cause_stalemate(self):
        result = self.get_result('8/8/5r2/2k3q1/8/6Q1/8/7K b - - 0 1')
        self.assertFalse('Qxg3' == result.move)

    def test_forced_stalemate(self):
        result = self.get_result('7k/5Kp1/5p1p/5P1P/8/8/8/8 w - - 0 1')
        self.assertEqual('Kg6', result.move)
        self.assertEqual(0, result.score)

    def test_mate_in_two_puzzle01(self):
        result = self.get_result('6k1/5pbp/6pB/8/7P/6P1/5PK1/r2R4 w - - 0 1')
        self.assertEqual('Rd8+', result.move)
        self.assert_is_checkmate(result)

    def test_multiple_checkmates(self):
        result = self.get_result('k7/8/6q1/8/8/8/r6r/3K4 b - - 0 1')
        instant_checkmates = result.move_scores[:4]
        non_instant_checkmates = result.move_scores[4:]

        # Verify the four instant wins are present
        instant_win_moves = [x[0] for x in instant_checkmates]
        self.assertTrue('Qg1#' in instant_win_moves)
        self.assertTrue('Rh1#' in instant_win_moves)
        self.assertTrue('Qb1#' in instant_win_moves)
        self.assertTrue('Ra1#' in instant_win_moves)

        # Verify all instant-wins have the same score
        unique_win_scores = set([x[1] for x in instant_checkmates])
        self.assertEqual(1, len(unique_win_scores))

        # Verify all instant-wins have better scores than non-instant wins
        for instant_win in instant_checkmates:
            for later_win in non_instant_checkmates:
                self.assertTrue(instant_win[1] > later_win[1])

    def test_get_mvv_lva_order_1(self):
        result = self.get_mvv_lva('4q2k/5P2/2b5/8/1N2Q3/8/8/6K1 w - - 0 1')
        attack_queen_with_pawn = result[:4]
        self.assertTrue('fxe8=B' in attack_queen_with_pawn)
        self.assertTrue('fxe8=N' in attack_queen_with_pawn)
        self.assertTrue('fxe8=R+' in attack_queen_with_pawn)
        self.assertTrue('fxe8=Q+' in attack_queen_with_pawn)
        self.assertEqual('Qxe8+', result[4])
        self.assertEqual('Nxc6', result[5])
        self.assertEqual('Qxc6', result[6])


if __name__ == '__main__':
    unittest.main()
