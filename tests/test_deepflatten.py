# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_cls import T

string_types = basestring if iteration_utilities.PY2 else str


deepflatten = iteration_utilities.deepflatten


def test_deepflatten_empty1():
    assert list(deepflatten([])) == []

    def test():
        list(deepflatten([]))
    assert not memory_leak(test)


def test_deepflatten_normal1():
    assert list(deepflatten([1, 2, 3])) == [1, 2, 3]

    def test():
        list(deepflatten([T(1), T(2), T(3)]))
    assert not memory_leak(test)


def test_deepflatten_normal2():
    assert list(deepflatten([[1], 2, [[3]]])) == [1, 2, 3]

    def test():
        list(deepflatten([[T(1)], T(2), [[T(3)]]]))
    assert not memory_leak(test)


def test_deepflatten_normal3():
    # really deeply nested thingy
    assert list(deepflatten([[[[[[[[[[[5, 4, 3, 2, 1, 0]]]]], range(3)]]],
                               (i for i in range(5))]]])
                ) == [5, 4, 3, 2, 1, 0, 0, 1, 2, 0, 1, 2, 3, 4]

    def test():
        list(deepflatten([[[[[[[[[[[T(5), T(4), T(3), T(2), T(1), T(0)]]]]],
                               map(T, range(3))]]],
                            (T(i) for i in range(5))]]]))
    assert not memory_leak(test)


def test_deepflatten_normal4():
    # really deeply nested thingy with types
    assert list(deepflatten([[[[[[[[[[[5, 4, 3, 2, 1, 0]]]]], [0, 1, 2]]]],
                               [0, 1, 2, 3, 4]]]], types=list)
                ) == [5, 4, 3, 2, 1, 0, 0, 1, 2, 0, 1, 2, 3, 4]

    def test():
        list(deepflatten([[[[[[[[[[[T(5), T(4), T(3), T(2), T(1), T(0)]]]]],
                               [T(0), T(1), T(2)]]]],
                            [T(0), T(1), T(2), T(3), T(4)]]]]))
    assert not memory_leak(test)


def test_deepflatten_depth1():
    assert list(deepflatten([1, 2, 3], 1)) == [1, 2, 3]

    def test():
        list(deepflatten([T(1), T(2), T(3)], 1))
    assert not memory_leak(test)


def test_deepflatten_depth2():
    assert list(deepflatten([[1], 2, [[3]]], 1)) == [1, 2, [3]]

    def test():
        list(deepflatten([[T(1)], T(2), [[T(3)]]], 1))
    assert not memory_leak(test)


def test_deepflatten_types1():
    assert list(deepflatten([[1], 2, [[3]]], types=list)) == [1, 2, 3]

    def test():
        list(deepflatten([[T(1)], T(2), [[T(3)]]], types=list))
    assert not memory_leak(test)


def test_deepflatten_types2():
    assert list(deepflatten([[1], 2, [[3]]], types=tuple)) == [[1], 2, [[3]]]

    def test():
        list(deepflatten([[T(1)], T(2), [[T(3)]]], types=tuple))
    assert not memory_leak(test)


def test_deepflatten_types3():
    assert list(deepflatten([[1], 2, ([3], )],
                            types=(list, tuple))) == [1, 2, 3]

    def test():
        list(deepflatten([[T(1)], T(2), ([T(3)], )], types=(list, tuple)))
    assert not memory_leak(test)


def test_deepflatten_ignore1():
    assert list(deepflatten([[1], 2, [[3, 'abc']]],
                            ignore=string_types)) == [1, 2, 3, 'abc']

    def test():
        list(deepflatten([[T(1)], T(2), [[T(3), 'abc']]], ignore=string_types))
    assert not memory_leak(test)


def test_deepflatten_ignore2():
    assert list(deepflatten([[1], 2, ([3, 'abc'], )],
                            ignore=(tuple, string_types))
                ) == [1, 2, ([3, 'abc'], )]

    def test():
        list(deepflatten([[T(1)], T(2), ([T(3), 'abc'], )],
                         ignore=(tuple, string_types)))
    assert not memory_leak(test)


def test_deepflatten_failure1():
    with pytest.raises(TypeError):
        list(deepflatten([1, 2, 3], None, 'a'))

    def test():
        with pytest.raises(TypeError):
            list(deepflatten([T(1), T(2), T(3)], None, T('a')))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_deepflatten_pickle1():
    dpflt = deepflatten([[1], [2], [3], [4]])
    assert next(dpflt) == 1
    x = pickle.dumps(dpflt)
    assert list(pickle.loads(x)) == [2, 3, 4]

    def test():
        dpflt = deepflatten([[T(1)], [T(2)], [T(3)], [T(4)]])
        next(dpflt)
        x = pickle.dumps(dpflt)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)
