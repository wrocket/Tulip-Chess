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

    def zero_depth_eval(self, fen):
        result = call_tulip(['-evalposition', fen])
        parsed_output = json.loads(result)
        return int(parsed_output['score'])
        return state

    def assert_score_approx(self, expected, actual, tolerance=5):
        self.assertTrue(abs(expected - actual) < abs(tolerance), "Expected score %i, got %i" % (expected, actual))

    def assert_score_better_than_w(self, better, worse, by_at_least=1, by_no_more_than=100):
        diff = better - worse
        self.assertTrue(diff > by_at_least, "Expected score to be better than %i by at least %i, got %i" % (worse, by_at_least, better))
        self.assertTrue(diff <= by_no_more_than, "Expected score to be better than %i by no more than %i, got %i" % (worse, by_no_more_than, better))

    def assert_score_better_than_b(self, better, worse, by_at_least=1, by_no_more_than=100):
        diff = abs(better - worse)
        self.assertTrue(diff > by_at_least, "Expected score to be better than %i by at least %i, got %i" % (worse, by_at_least, better))
        self.assertTrue(diff <= by_no_more_than, "Expected score to be better than %i by no more than %i, got %i" % (worse, by_no_more_than, better))

    def test_zerodepth_initial_position(self):
        result = self.zero_depth_eval('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
        self.assert_score_approx(0, result)

    def test_zerodepth_initial_position_no_bqueen(self):
        result = self.zero_depth_eval('rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1')
        self.assert_score_approx(900, result)

    def test_zerodepth_rr_white_same_rank_strong(self):
        on_same_rank = self.zero_depth_eval('2R1k3/8/8/8/8/8/8/2R1K3 b - - 0 1')
        not_on_same_rank = self.zero_depth_eval('1R2k3/8/8/8/8/8/8/2R1K3 b - - 0 1')
        self.assert_score_better_than_w(on_same_rank, not_on_same_rank)

    def test_zerodepth_rq_white_same_rank_strong(self):
        on_same_rank = self.zero_depth_eval('2Q1k3/8/8/8/8/8/8/2R1K3 b - - 0 1')
        not_on_same_rank = self.zero_depth_eval('1Q2k3/8/8/8/8/8/8/2R1K3 b - - 0 1')
        self.assert_score_better_than_w(on_same_rank, not_on_same_rank)

    def test_zerodepth_rr_black_same_rank_strong(self):
        on_same_rank = self.zero_depth_eval('2r1k3/8/8/8/8/8/8/2r1K3 w - - 0 1')
        not_on_same_rank = self.zero_depth_eval('2r1k3/8/8/8/8/8/8/1r2K3 w - - 0 1')
        self.assert_score_better_than_b(on_same_rank, not_on_same_rank)

    def test_zerodepth_rq_black_same_rank_strong(self):
        on_same_rank = self.zero_depth_eval('2q1k3/8/8/8/8/8/8/2r1K3 w - - 0 1')
        not_on_same_rank = self.zero_depth_eval('2q1k3/8/8/8/8/8/8/1r2K3 w - - 0 1')
        self.assert_score_better_than_b(on_same_rank, not_on_same_rank)

if __name__ == '__main__':
    unittest.main()
