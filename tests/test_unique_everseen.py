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


unique_everseen = iteration_utilities.unique_everseen
Seen = iteration_utilities.Seen


def test_uniqueeverseen_empty1():
    assert list(unique_everseen([])) == []

    def test():
        list(unique_everseen([]))
    assert not memory_leak(test)


def test_uniqueeverseen_normal1():
    assert list(unique_everseen([1, 2, 1])) == [1, 2]

    def test():
        list(unique_everseen([T(1), T(2), T(3)]))
    assert not memory_leak(test)


def test_uniqueeverseen_key1():
    assert list(unique_everseen([1, 2, 1], abs)) == [1, 2]

    def test():
        list(unique_everseen([T(1), T(2), T(1)],
                             lambda x: abs(x.value)))
    assert not memory_leak(test)


def test_uniqueeverseen_key2():
    assert list(unique_everseen([1, 1, -1], abs)) == [1]

    def test():
        list(unique_everseen([T(1), T(1), T(-1)],
                             lambda x: abs(x.value)))
    assert not memory_leak(test)


def test_uniqueeverseen_unhashable1():
    assert list(unique_everseen([{1: 1}, {2: 2}, {1: 1}])) == [{1: 1}, {2: 2}]

    def test():
        list(unique_everseen([{T(1): T(1)}, {T(2): T(2)},
                              {T(1): T(1)}]))
    assert not memory_leak(test)


def test_uniqueeverseen_unhashable2():
    assert list(unique_everseen([[1], [2], [1]])) == [[1], [2]]

    def test():
        list(unique_everseen([[T(1)], [T(2)], [T(1)]]))
    assert not memory_leak(test)


def test_uniqueeverseen_unhashable3():
    assert list(unique_everseen([[1, 1], [1, 2], [1, 3]],
                                operator.itemgetter(0))) == [[1, 1]]

    def test():
        list(unique_everseen([[T(1), T(1)], [T(1), T(2)],
                              [T(1), T(3)]], operator.itemgetter(0)))
    assert not memory_leak(test)


def test_uniqueeverseen_getter1():
    t = unique_everseen([1, [0, 0], 3])
    assert not t.seen
    assert t.key is None
    assert next(t) == 1
    assert t.seen == Seen({1})
    assert t.key is None
    assert next(t) == [0, 0]
    assert 1 in t.seen
    assert [0, 0] in t.seen
    assert t.key is None
    assert next(t) == 3
    assert t.seen == Seen({1, 3}, [[0, 0]])
    assert t.key is None

    def test():
        t = unique_everseen([T(1), T([0, 0]), T(3)])
        l1 = t.seen, t.key
        next(t)
        l1 = t.seen, t.key
        next(t)
        l1 = t.seen, t.key
        next(t)
    assert not memory_leak(test)


def test_uniqueeverseen_failure1():
    with pytest.raises(TypeError):
        list(unique_everseen(10))

    def test():
        with pytest.raises(TypeError):
            list(unique_everseen(T(10)))
    assert not memory_leak(test)


def test_uniqueeverseen_failure2():
    with pytest.raises(TypeError):
        list(unique_everseen([1, 2, 3, 'a'], abs))

    def test():
        with pytest.raises(TypeError):
            list(unique_everseen([T(1), T(2), T(3), T('a')],
                                 lambda x: abs(x.value)))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_uniqueeverseen_pickle1():
    uqe = unique_everseen([1, 2, 1, 2])
    assert next(uqe) == 1
    x = pickle.dumps(uqe)
    assert list(pickle.loads(x)) == [2]

    def test():
        uqe = unique_everseen([T(1), T(2), T(1), T(2)])
        next(uqe)
        x = pickle.dumps(uqe)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)
