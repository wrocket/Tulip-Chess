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

class SearchResult:
    def __init__(self, move, score, move_scores=[]):
        self.move = move
        self.score = score
        self.move_scores = move_scores

class TestSearch(unittest.TestCase):
    def setUp(self):
        None

    def get_result(self, fen):
        result = call_tulip(['-simplesearch', fen])
        parsed_output = json.loads(result)
        json_obj = parsed_output['searchResult']
        parsed_scores = [(x['move'], x['score']) for x in json_obj['rootNodeScores']]
        sorted_scores = sorted(parsed_scores, key=lambda x: -1* x[1]) # sort move scores by score
        return SearchResult(move=json_obj['move'], score=json_obj['score'], move_scores=sorted_scores)

    def test_easy_skewer(self):
        result = self.get_result('4q3/8/8/8/4k3/8/8/2KR4 w - - 0 1')
        self.assertEqual('Re1+', result.move)

    def test_friend_liver_qs(self):
        result = self.get_result('rnbqkb1r/1pp1ppp1/p4n1p/1N1p4/3P1B2/8/PPP1PPPP/R2QKBNR w KQkq - 0 5')
        self.assertEqual('Nxc7+', result.move)

    def test_simple_fork(self):
        result = self.get_result('7k/8/2n4p/8/2K3Q1/8/8/8 b - - 0 1')
        self.assertEqual('Ne5+', result.move)

    def test_play_for_draw(self):
        result = self.get_result('7k/8/2n5/8/2K3Q1/8/8/8 b - - 0 1')
        self.assertEqual('Ne5+', result.move)

    def test_short_mate(self):
        result = self.get_result('1rb2rk1/p1q1ppbp/n2p3B/3n1P2/1ppP4/3B1N2/PPPQN1PP/K2R3R w - - 0 1')
        self.assertEqual('Qg5', result.move)

if __name__ == '__main__':
    unittest.main()
