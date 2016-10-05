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


successive = iteration_utilities.successive


class T(object):
    def __init__(self, value):
        self.value = value


def test_successive_empty1():
    assert list(successive([])) == []

    def test():
        list(successive([]))
    assert not memory_leak(test)


def test_successive_empty2():
    assert list(successive([1])) == []

    def test():
        list(successive([T(1)]))
    assert not memory_leak(test)


def test_successive_empty3():
    assert list(successive([], times=10)) == []

    def test():
        list(successive([], times=10))
    assert not memory_leak(test)


def test_successive_empty4():
    assert list(successive([1, 2, 3, 4, 5], times=10)) == []

    def test():
        list(successive([T(1), T(2), T(3)], times=10))
    assert not memory_leak(test)


def test_successive_normal1():
    assert list(successive(range(4))) == [(0, 1), (1, 2), (2, 3)]

    def test():
        list(successive([T(1), T(2), T(3), T(4)]))
    assert not memory_leak(test)


def test_successive_normal2():
    assert list(successive(range(4), times=3)) == [(0, 1, 2), (1, 2, 3)]

    def test():
        list(successive([T(1), T(2), T(3), T(4)], times=3))
    assert not memory_leak(test)


def test_successive_normal3():
    assert list(successive(range(4), times=4)) == [(0, 1, 2, 3)]

    def test():
        list(successive([T(1), T(2), T(3), T(4)], times=4))
    assert not memory_leak(test)


def test_successive_failure1():
    with pytest.raises(TypeError):
        successive(10)

    def test():
        with pytest_raises(TypeError):
            successive(T(1))
    assert not memory_leak(test)


def test_successive_failure2():
    with pytest.raises(ValueError):  # times must be > 0
        successive([1, 2, 3], 0)

    def test():
        with pytest_raises(ValueError):  # times must be > 0
            successive([T(1), T(2), T(3)], 0)
    assert not memory_leak(test)


def test_successive_pickle1():
    suc = successive([1, 2, 3, 4])
    assert next(suc) == (1, 2)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [(2, 3), (3, 4)]

    def test():
        suc = successive([T(1), T(2), T(3), T(4)])
        next(suc)
        x = pickle.dumps(suc)
        list(pickle.loads(x))
    assert not memory_leak(test)
