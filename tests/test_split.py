# Built-ins
from __future__ import absolute_import, division, print_function
from functools import partial
from operator import eq
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


split = iteration_utilities.split


class T(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        if type(self.value) != type(other.value):
            raise TypeError('simulated failure.')
        return self.value == other.value


equalsthree = partial(eq, 3)
equalsthreeT = partial(eq, T(3))


def test_split_empty1():
    assert list(split([], lambda x: False)) == []

    def test():
        list(split([], lambda x: False)) == []
    assert not memory_leak(test)


def test_split_normal1():
    assert list(split([1, 2, 3], lambda x: x == 2)) == [[1], [3]]

    def test():
        list(split([T(1), T(2), T(3)], lambda x: x.value == 2))
    assert not memory_leak(test)


def test_split_normal2():
    assert list(split([1, 2, 3], lambda x: x == 3)) == [[1, 2]]

    def test():
        list(split([T(1), T(2), T(3)], lambda x: x.value == 3))
    assert not memory_leak(test)


def test_split_keep1():
    assert list(split([1, 2, 3], lambda x: x == 2,
                      keep=True)) == [[1], [2], [3]]

    def test():
        list(split([T(1), T(2), T(3)], lambda x: x.value == 2,
                   keep=True))
    assert not memory_leak(test)


def test_split_keep2():
    assert list(split([1, 2, 3], lambda x: x == 3,
                      keep=True)) == [[1, 2], [3]]

    def test():
        list(split([T(1), T(2), T(3)], lambda x: x.value == 3,
                   keep=True))
    assert not memory_leak(test)


def test_split_maxsplit1():
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=1)) == [[1], [3, 4, 5]]

    def test():
        list(split([T(1), T(2), T(3), T(4), T(5)],
                   lambda x: x.value % 2 == 0, maxsplit=1))
    assert not memory_leak(test)


def test_split_maxsplit2():
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=2)) == [[1], [3], [5]]

    def test():
        list(split([T(1), T(2), T(3), T(4), T(5)],
                   lambda x: x.value % 2 == 0, maxsplit=2))
    assert not memory_leak(test)


def test_split_eq1():
    assert list(split([1, 2, 3, 2, 5], 2,
                      eq=True)) == [[1], [3], [5]]

    def test():
        list(split([T(1), T(2), T(3), T(2), T(5)], T(2),
                   eq=True))
    assert not memory_leak(test)


def test_split_failure1():
    # not iterable
    with pytest.raises(TypeError):
        split(1, lambda x: False)

    def test():
        with pytest_raises(TypeError):
            split(T(1), lambda x: False)
    assert not memory_leak(test)


def test_split_failure2():
    # func fails
    with pytest.raises(TypeError):
        list(split([1, 2, 3], lambda x: x + 'a'))

    def test():
        with pytest_raises(TypeError):
            list(split([T(1), T(2), T(3)],
                       lambda x: T(x.value + 'a')))
    assert not memory_leak(test)


def test_split_failure3():
    # cmp fails
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], T('a'), eq=True))

    def test():
        with pytest_raises(TypeError):
            list(split([T(1), T(2), T(3)], T('a'), eq=True))
    assert not memory_leak(test)


def test_split_pickle1():
    l = [1, 2, 3, 4, 5, 3, 7, 8]
    spl = split(l, equalsthree)
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [[1, 2], [4, 5], [7, 8]]

    def test():
        l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
        spl = split(l, equalsthreeT)
        x = pickle.dumps(spl)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


def test_split_pickle2():
    l = [1, 2, 3, 4, 5, 3, 7, 8]
    spl = split(l, equalsthree)
    assert next(spl) == [1, 2]
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [[4, 5], [7, 8]]

    def test():
        l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
        spl = split(l, equalsthreeT)
        next(spl)
        x = pickle.dumps(spl)
        list(pickle.loads(x))
    assert not memory_leak(test)


def test_split_pickle3():
    l = [1, 2, 3, 4, 5, 3, 7, 8]
    spl = split(l, equalsthree, keep=True)
    assert next(spl) == [1, 2]
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [[3], [4, 5], [3], [7, 8]]

    def test():
        l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
        spl = split(l, equalsthreeT, keep=True)
        next(spl)
        x = pickle.dumps(spl)
        list(pickle.loads(x))
    assert not memory_leak(test)


def test_split_pickle4():
    l = [1, 2, 3, 4, 5, 3, 7, 8]
    spl = split(l, equalsthree, maxsplit=1)
    assert next(spl) == [1, 2]
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [[4, 5, 3, 7, 8]]

    def test():
        l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
        spl = split(l, equalsthreeT, maxsplit=1)
        next(spl)
        x = pickle.dumps(spl)
        list(pickle.loads(x))
    assert not memory_leak(test)


def test_split_pickle5():
    l = [1, 2, 3, 4, 5, 3, 7, 8]
    spl = split(l, 3, eq=True)
    assert next(spl) == [1, 2]
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [[4, 5], [7, 8]]

    def test():
        l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
        spl = split(l, T(3), eq=True)
        next(spl)
        x = pickle.dumps(spl)
        list(pickle.loads(x))
    assert not memory_leak(test)
