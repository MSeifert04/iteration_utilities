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


roundrobin = iteration_utilities.roundrobin


class T(object):
    def __init__(self, value):
        self.value = value



def test_roundrobin_empty1():
    assert list(roundrobin()) == []

    def test():
        list(roundrobin())
    assert not memory_leak(test)


def test_roundrobin_empty2():
    assert list(roundrobin([])) == []

    def test():
        list(roundrobin([]))
    assert not memory_leak(test)


def test_roundrobin_empty3():
    assert list(roundrobin([], (), {})) == []

    def test():
        list(roundrobin([], (), {}))
    assert not memory_leak(test)


def test_roundrobin_normal1():
    assert list(roundrobin([1], [1, 2], [1, 2, 3])) == [1, 1, 1, 2, 2, 3]

    def test():
        list(roundrobin([T(1)],
                        [T(1), T(2)],
                        [T(1), T(2), T(3)]))
    assert not memory_leak(test)


def test_roundrobin_normal2():
    assert list(roundrobin([1, 2, 3], [1], [1, 2])) == [1, 1, 1, 2, 2, 3]

    def test():
        list(roundrobin([T(1), T(2), T(3)],
                        [T(1)],
                        [T(1), T(2)]))
    assert not memory_leak(test)


def test_roundrobin_normal3():
    assert list(roundrobin([1, 2], [1, 2, 3], [1])) == [1, 1, 1, 2, 2, 3]

    def test():
        list(roundrobin([T(1), T(2)],
                        [T(1), T(2), T(3)],
                        [T(1)]))
    assert not memory_leak(test)


def test_roundrobin_failure1():
    with pytest.raises(TypeError):
        list(roundrobin(10))

    def test():
        with pytest_raises(TypeError):
            list(roundrobin(T(1)))
    assert not memory_leak(test)


def test_roundrobin_failure2():
    with pytest.raises(TypeError):
        list(roundrobin([10], 100))

    def test():
        with pytest_raises(TypeError):
            list(roundrobin([T(1)], T(1)))
    assert not memory_leak(test)
