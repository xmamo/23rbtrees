# 2-3 red-black trees

This repository implements a variant of the red-black tree, namely the 2-3 red-black tree.

The implementation is based on the algorithms for 2-3 red-black trees described in the following
paper: https://arxiv.org/abs/2004.04344. It is written to be hopefully more readable and
understandable than the implementation provided by the authors at
https://github.com/k-ghiasi/RedBlackTrees/blob/main/ParitySeeking-23RedBlackBST.h.

Compared to the original implementation, no sentinel node has been used; furthermore, each node
holds additional information to determine if it is the left or right child of its parent.
