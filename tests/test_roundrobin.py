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


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_roundrobin_pickle1():
    rr = roundrobin([1, 2, 3], [1, 2, 3])
    assert next(rr) == 1
    x = pickle.dumps(rr)
    assert list(pickle.loads(x)) == [1, 2, 2, 3, 3]

    def test():
        rr = roundrobin([T(1), T(2), T(3)], [T(1), T(2), T(3)])
        next(rr)
        x = pickle.dumps(rr)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_roundrobin_pickle2():
    rr2 = roundrobin([1], [1, 2, 3])
    assert next(rr2) == 1
    assert next(rr2) == 1
    assert next(rr2) == 2
    x = pickle.dumps(rr2)
    assert list(pickle.loads(x)) == [3]

    def test():
        rr2 = roundrobin([T(1)], [T(1), T(2), T(3)])
        next(rr2)
        next(rr2)
        next(rr2)
        x = pickle.dumps(rr2)
        list(pickle.loads(x))
    assert not memory_leak(test)
