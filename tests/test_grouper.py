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


grouper = iteration_utilities.grouper


class T(object):
    def __init__(self, value):
        self.value = value


def test_grouper_empty1():
    # Empty iterable
    assert list(grouper([], 2)) == []

    def test():
        list(grouper([], 2))
    assert not memory_leak(test)


def test_grouper_normal1():
    # no fillvalue + truncate
    assert list(grouper([1], 3)) == [(1, )]

    def test():
        list(grouper([T(1)], 3))
    assert not memory_leak(test)


def test_grouper_normal2():
    assert list(grouper([1, 2], 3)) == [(1, 2)]

    def test():
        list(grouper([T(1), T(2)], 3))
    assert not memory_leak(test)


def test_grouper_normal3():
    assert list(grouper([1, 2, 3], 3)) == [(1, 2, 3)]

    def test():
        list(grouper([T(1), T(2), T(3)], 3))
    assert not memory_leak(test)


def test_grouper_normal4():
    assert list(grouper([1, 2, 3, 4], 3)) == [(1, 2, 3), (4, )]

    def test():
        list(grouper([T(1), T(2), T(3), T(4)], 3))
    assert not memory_leak(test)


def test_grouper_normal5():
    assert list(grouper([1, 2, 3, 4, 5], 3)) == [(1, 2, 3), (4, 5)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4), T(5)], 3))
    assert not memory_leak(test)


def test_grouper_normal6():
    assert list(grouper([1, 2, 3, 4, 5, 6], 3)) == [(1, 2, 3), (4, 5, 6)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4), T(5), T(6)],
                     3))
    assert not memory_leak(test)


def test_grouper_fill1():
    # with fillvalue
    assert list(grouper([1], 3,
                        fillvalue=0)) == [(1, 0, 0)]

    def test():
        list(grouper([T(1)], 3,
                     fillvalue=T(0)))
    assert not memory_leak(test)


def test_grouper_fill2():
    assert list(grouper([1, 2], 3,
                        fillvalue=0)) == [(1, 2, 0)]

    def test():
        list(grouper([T(1), T(2)], 3,
                     fillvalue=T(0)))
    assert not memory_leak(test)


def test_grouper_fill3():
    assert list(grouper([1, 2, 3], 3,
                        fillvalue=0)) == [(1, 2, 3)]

    def test():
        list(grouper([T(1), T(2), T(3)], 3,
                     fillvalue=T(0)))
    assert not memory_leak(test)


def test_grouper_fill4():
    assert list(grouper([1, 2, 3, 4], 3,
                        fillvalue=0)) == [(1, 2, 3), (4, 0, 0)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4)], 3,
                     fillvalue=T(0)))
    assert not memory_leak(test)


def test_grouper_fill5():
    assert list(grouper([1, 2, 3, 4, 5], 3,
                        fillvalue=0)) == [(1, 2, 3), (4, 5, 0)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4), T(5)], 3,
                     fillvalue=T(0)))
    assert not memory_leak(test)


def test_grouper_fill6():
    assert list(grouper([1, 2, 3, 4, 5, 6], 3,
                        fillvalue=0)) == [(1, 2, 3), (4, 5, 6)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4), T(5), T(6)], 3,
                     fillvalue=T(0)))
    assert not memory_leak(test)


def test_grouper_truncate1():
    # with truncate
    assert list(grouper([1], 3,
                        truncate=True)) == []

    def test():
        list(grouper([T(1)], 3,
                     truncate=True))
    assert not memory_leak(test)


def test_grouper_truncate2():
    assert list(grouper([1, 2], 3,
                        truncate=True)) == []

    def test():
        list(grouper([T(1), T(2)], 3,
                     truncate=True))
    assert not memory_leak(test)


def test_grouper_truncate3():
    assert list(grouper([1, 2, 3], 3,
                        truncate=True)) == [(1, 2, 3)]

    def test():
        list(grouper([T(1), T(2), T(3)], 3,
                     truncate=True))
    assert not memory_leak(test)


def test_grouper_truncate4():
    assert list(grouper([1, 2, 3, 4], 3,
                        truncate=True)) == [(1, 2, 3)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4)], 3,
                     truncate=True))
    assert not memory_leak(test)


def test_grouper_truncate5():
    assert list(grouper([1, 2, 3, 4, 5], 3,
                        truncate=True)) == [(1, 2, 3)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4), T(5)], 3,
                     truncate=True))
    assert not memory_leak(test)


def test_grouper_truncate6():
    assert list(grouper([1, 2, 3, 4, 5, 6], 3,
                        truncate=True)) == [(1, 2, 3), (4, 5, 6)]

    def test():
        list(grouper([T(1), T(2), T(3), T(4), T(5), T(6)], 3,
                     truncate=True))
    assert not memory_leak(test)


def test_grouper_failure1():
    # fillvalue + truncate is forbidden
    with pytest.raises(TypeError):
        grouper([1, 2, 3], 2, fillvalue=None, truncate=True)

    def test():
        with pytest_raises(TypeError):
            grouper([T(1), T(2), T(3)], 2,
                    fillvalue=T(0), truncate=True)
    assert not memory_leak(test)


def test_grouper_failure2():
    # n must be > 0
    with pytest.raises(ValueError):
        grouper([1, 2, 3], 0)

    def test():
        with pytest_raises(ValueError):
            grouper([T(1), T(2), T(3)], 0)
    assert not memory_leak(test)


def test_grouper_failure3():
    # iterable must be iterable
    with pytest.raises(TypeError):
        grouper(1, 2)

    def test():
        with pytest_raises(TypeError):
            grouper(T(1), 2)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_grouper_pickle1():
    grp = grouper(range(10), 3)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9,)]

    def test():
        grp = grouper(list(map(T, range(10))), 3)
        next(grp)
        x = pickle.dumps(grp)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_grouper_pickle2():
    grp = grouper(range(10), 3, fillvalue=0)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9, 0, 0)]

    def test():
        grp = grouper(list(map(T, range(10))), 3, fillvalue=T(0))
        next(grp)
        x = pickle.dumps(grp)
        list(pickle.loads(x))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_grouper_pickle3():
    grp = grouper(range(10), 3, truncate=True)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8)]

    def test():
        grp = grouper(list(map(T, range(10))), 3, truncate=True)
        next(grp)
        x = pickle.dumps(grp)
        list(pickle.loads(x))
    assert not memory_leak(test)
