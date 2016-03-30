#!/usr/bin/python3

# The MIT License (MIT)

# Copyright (c) 2015 Brian Wray (brian@wrocket.org)

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Script that reads a bunch of lines containing move tokens and condenses it into a tree structure.
# This eliminates duplicate openings and is designed as a post-processor of sorts to the digest_pgn script.

import sys

class Node:
	def __init__(self, val):
		self.value = val
		self.children = []

def add_line(root, children):
	if not children or len(children) == 0:
		return
	head = children[0]
	for c in root.children:
		if c.value == head:
			add_line(c, children[1:])
			return
	new_node = Node(head)
	root.children.append(new_node)
	add_line(new_node, children[1:])

def listify_recursive(node, stack, results):
	stack.append(node.value)
	if len(node.children) == 0:
		results.append(list(stack))
	else:
		for c in sorted(node.children, key=lambda n: n.value):
			listify_recursive(c, stack, results)
	stack.pop()
			
def listify_tree(node):
	all_lines = []
	listify_recursive(node, [], all_lines)
	return all_lines

root = Node('*')
for line in sys.stdin:
	split = list(filter(lambda s: len(s) > 0, line.strip().split(' ')))
	add_line(root, split)

all_lines = listify_tree(root)

for l in all_lines:
	print(' '.join(l[1:]))