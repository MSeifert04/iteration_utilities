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


count_items = iteration_utilities.count_items


class T(object):
    def __init__(self, value):
        self.value = value

    def __lt__(self, other):
        return self.value < other.value

    def __gt__(self, other):
        return self.value > other.value

    def __eq__(self, other):
        return self.value == other.value

    def __bool__(self):
        return bool(self.value)

    def __nonzero__(self):
        return bool(self.value)


def test_count_empty1():
    assert count_items([]) == 0

    def test():
        count_items([])
    assert not memory_leak(test)


def test_count_normal1():
    assert count_items([0, 0]) == 2

    def test():
        count_items([T(0), T(0)])
    assert not memory_leak(test)


def test_count_normal2():
    assert count_items([0, 0, 1], bool) == 1

    def test():
        count_items([T(0), T(0), T(1)], bool)
    assert not memory_leak(test)


def test_count_normal3():
    assert count_items([0, 0, 1, 1], None) == 4

    def test():
        count_items([T(0), T(0), T(1), T(1)], None)
    assert not memory_leak(test)


def test_count_normal4():
    assert count_items([], lambda x: x) == 0

    def test():
        count_items([], iteration_utilities.return_identity)
    assert not memory_leak(test)


def test_count_normal5():
    assert count_items([1, 2, 3], lambda x: x > 2) == 1

    def test():
        count_items([T(1), T(2), T(3)], lambda x: x > T(2))
    assert not memory_leak(test)


def test_count_normal6():
    assert count_items([1, 2, 3], lambda x: x < 3) == 2

    def test():
        count_items([T(1), T(2), T(3)], lambda x: x < T(3))
    assert not memory_leak(test)


def test_count_normal7():
    assert count_items([3, 1, 2, 3, 3], 3, True) == 3

    def test():
        count_items([T(3), T(1), T(2), T(3), T(3)], T(3), True)
    assert not memory_leak(test)


def test_count_failure1():
    with pytest.raises(TypeError):
        count_items(1)

    def test():
        with pytest_raises(TypeError):
            count_items(T(1))
    assert not memory_leak(test)


def test_count_failure2():
    with pytest.raises(TypeError):
        count_items([1], 1)

    def test():
        with pytest_raises(TypeError):
            count_items([T(1)], T(1))
    assert not memory_leak(test)
