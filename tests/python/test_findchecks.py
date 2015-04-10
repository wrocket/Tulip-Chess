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

    def get_status(self, fen):
        result = call_tulip(['-checkstatus', fen])
        return json.loads(result)

    def test_simple_check_black(self):
        result = self.get_status('4k3/8/8/1B6/8/8/8/4K3 b - e3 0 1')
        self.assertTrue(result['isCheck'])

    def test_simple_check_black_blocked(self):
        result = self.get_status('4k3/3p4/8/1B6/8/8/8/4K3 b - e3 0 1')
        self.assertFalse(result['isCheck'])

    def test_simple_check_white(self):
        result = self.get_status('4k3/8/8/4r3/8/8/8/4K3 w - e3 0 1')
        self.assertTrue(result['isCheck'])

    def test_simple_check_white_blocked(self):
        result = self.get_status('4k3/8/8/4r3/8/8/4P3/4K3 w - e3 0 1')
        self.assertFalse(result['isCheck'])

if __name__ == '__main__':
    unittest.main()
