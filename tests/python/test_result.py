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

class TestGameResultDetection(unittest.TestCase):
    def setUp(self):
        None

    def get_status(self, fen):
        result = call_tulip(['-gamestatus', fen])
        return json.loads(result)['status']

    def get_status_after_moves(self, fen, moves):
        args = ['-movegamestatus', fen]
        args.extend(moves)
        result = call_tulip(args)
        return json.loads(result)['status']

    def test_black_checkmated(self):
        result = self.get_status('4R1k1/5ppp/8/8/8/8/8/4K3 b - - 0 1')
        self.assertEqual('blackCheckmated', result)

    def test_white_checkmated(self):
        result = self.get_status('7k/8/8/4r3/6b1/6n1/2PP1P2/4K3 w - - 0 1')
        self.assertEqual('whiteCheckmated', result)

    def test_stalemate(self):
        result = self.get_status('4k3/4P3/4K3/8/8/8/8/8 b - - 0 1')
        self.assertEqual('stalemate', result)

    def test_material_draw_KvK(self):
        result = self.get_status('3k4/8/8/8/8/8/8/3K4 w - - 0 1')
        self.assertEqual('materialDraw', result)

    def test_material_draw_KvKQ(self):
        result = self.get_status('3kq3/8/8/8/8/8/8/3K4 w - - 0 1')
        self.assertEqual('none', result)

    def test_material_draw_KvKN(self):
        result = self.get_status('3k4/8/8/8/8/N7/8/3K4 w - - 0 1')
        self.assertEqual('materialDraw', result)

    def test_material_draw_KvKB(self):
        result = self.get_status('3k4/8/5b2/8/8/8/8/3K4 w - - 0 1')
        self.assertEqual('materialDraw', result)

    def test_material_draw_KNvKN(self):
        result = self.get_status('3k4/8/4n3/8/8/N7/8/3K4 w - - 0 1')
        self.assertEqual('none', result)

    def test_material_draw_KBvKB_same_color_light(self):
        result = self.get_status('3k4/8/4b3/8/6B1/8/8/3K4 w - - 0 1')
        self.assertEqual('materialDraw', result)

    def test_material_draw_KBvKB_same_color_dark(self):
        result = self.get_status('3k4/8/5b2/8/7B/8/8/3K4 w - - 0 1')
        self.assertEqual('materialDraw', result)

    def test_material_draw_KBvKB_different_color(self):
        result = self.get_status('3k4/8/5b2/8/6B1/8/8/3K4 w - - 0 1')
        self.assertEqual('none', result)

    def test_material_draw_many_bishops_same_color(self):
        result = self.get_status('3k1b2/8/1B3b2/b7/7B/b1b1B1B1/5B2/3K4 b - - 0 1')
        self.assertEqual('materialDraw', result)

    def test_material_draw_many_bishops_different_color(self):
        result = self.get_status('3k1b2/8/1B3b2/b7/2b4B/b1b1B1B1/5B2/3K4 b - - 0 1')
        self.assertEqual('none', result)

    def test_material_draw_many_bishops_extra_piece(self):
        result = self.get_status('3k1b2/8/1B1P1b2/b7/7B/b1b1B1B1/5B2/3K4 b - - 0 1')
        self.assertEqual('none', result)

    def test_material_draw_many_bishops_only_one_side_with_bishops_w(self):
        result = self.get_status('3k4/8/1B6/8/7B/4B1B1/5B2/3K4 b - - 0 1')
        self.assertEqual('none', result)

    def test_material_draw_many_bishops_only_one_side_with_bishops_b(self):
        result = self.get_status('3k4/8/1b6/8/7b/4b1b1/5b2/3K4 b - - 0 1')
        self.assertEqual('none', result)

    def test_threefold_draw_simple_draw(self):
        moves = ['Rh2', 'Ke8', 'Rh1', 'Ke7', 'Rh2', 'Ke8', 'Rh1', 'Ke7']
        result = self.get_status_after_moves('8/4k3/8/8/8/8/4K3/7R w - - 0 1', moves)
        self.assertEqual('threefoldDraw', result)

    def test_threefold_draw_simple_not_repeated(self):
        moves = ['Rh2', 'Ke8', 'Rh1', 'Ke7', 'Rh2', 'Ke8', 'Rh1', 'Kf8']
        result = self.get_status_after_moves('8/4k3/8/8/8/8/4K3/7R w - - 0 1', moves)
        self.assertEqual('none', result)

    def test_threefold_draw_pawn_breaks_draw(self):
        moves = ['Rh2', 'Ke8', 'Rh1', 'Ke7', 'b5', 'Ke8', 'Rh2', 'Ke7', 'Rh1']
        result = self.get_status_after_moves('8/4k3/8/8/1P6/8/4K3/7R w - - 0 1', moves)
        self.assertEqual('none', result)

    def test_threefold_draw_capture_breaks_draw(self):
        moves = ['Rh2', 'Ke8', 'Rh1', 'Ke7', 'Rxb5', 'Ke8', 'Rb4', 'Ke7', 'Rh2', 'Ke8', 'Rh1', 'Ke7']
        result = self.get_status_after_moves('8/4k3/8/1p6/1R6/8/4K3/7R w - - 0 1', moves)
        self.assertEqual('none', result)

if __name__ == '__main__':
    unittest.main()
