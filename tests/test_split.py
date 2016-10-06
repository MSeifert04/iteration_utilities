# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


split = iteration_utilities.split


class Test(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        if type(self.value) != type(other.value):
            raise TypeError('simulated failure.')
        return self.value == other.value


def test_split_empty1():
    assert list(split([], lambda x: False)) == []

    def test():
        list(split([], lambda x: False)) == []
    assert not memory_leak(test)


def test_split_normal1():
    assert list(split([1, 2, 3], lambda x: x == 2)) == [[1], [3]]

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 2))
    assert not memory_leak(test)


def test_split_normal2():
    assert list(split([1, 2, 3], lambda x: x == 3)) == [[1, 2]]

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 3))
    assert not memory_leak(test)


def test_split_keep1():
    assert list(split([1, 2, 3], lambda x: x == 2,
                      keep=True)) == [[1], [2], [3]]

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 2,
                   keep=True))
    assert not memory_leak(test)


def test_split_keep2():
    assert list(split([1, 2, 3], lambda x: x == 3,
                      keep=True)) == [[1, 2], [3]]

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 3,
                   keep=True))
    assert not memory_leak(test)


def test_split_maxsplit1():
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=1)) == [[1], [3, 4, 5]]

    def test():
        list(split([Test(1), Test(2), Test(3), Test(4), Test(5)],
                   lambda x: x.value % 2 == 0, maxsplit=1))
    assert not memory_leak(test)


def test_split_maxsplit2():
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=2)) == [[1], [3], [5]]

    def test():
        list(split([Test(1), Test(2), Test(3), Test(4), Test(5)],
                   lambda x: x.value % 2 == 0, maxsplit=2))
    assert not memory_leak(test)


def test_split_eq1():
    assert list(split([1, 2, 3, 2, 5], 2,
                      eq=True)) == [[1], [3], [5]]

    def test():
        list(split([Test(1), Test(2), Test(3), Test(2), Test(5)], Test(2),
                   eq=True))
    assert not memory_leak(test)


def test_split_failure1():
    # not iterable
    with pytest.raises(TypeError):
        split(1, lambda x: False)

    def test():
        with pytest_raises(TypeError):
            split(Test(1), lambda x: False)
    assert not memory_leak(test)


def test_split_failure2():
    # func fails
    with pytest.raises(TypeError):
        list(split([1, 2, 3], lambda x: x + 'a'))

    def test():
        with pytest_raises(TypeError):
            list(split([Test(1), Test(2), Test(3)],
                       lambda x: Test(x.value + 'a')))
    assert not memory_leak(test)


def test_split_failure3():
    # cmp fails
    with pytest.raises(TypeError):
        list(split([Test(1), Test(2), Test(3)], Test('a'), eq=True))

    def test():
        with pytest_raises(TypeError):
            list(split([Test(1), Test(2), Test(3)], Test('a'), eq=True))
    assert not memory_leak(test)
