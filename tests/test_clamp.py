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


clamp = iteration_utilities.clamp


class T(object):
    def __init__(self, value):
        self.value = value

    def __lt__(self, other):
        return self.value < other.value

    def __le__(self, other):
        return self.value <= other.value

    def __gt__(self, other):
        return self.value > other.value

    def __ge__(self, other):
        return self.value >= other.value


def test_applyfunc_empty1():
    assert list(clamp([], 10, 100)) == []

    def test():
        list(clamp([], T(10), T(100)))
    assert not memory_leak(test)


def test_applyfunc_normal1():
    assert list(clamp(range(10), 2, 7)) == [2, 3, 4, 5, 6, 7]

    def test():
        list(clamp(map(T, range(10)), T(2), T(7)))
    assert not memory_leak(test)


def test_applyfunc_normal2():
    # only low
    assert list(clamp(range(10), 2)) == [2, 3, 4, 5, 6, 7, 8, 9]

    def test():
        list(clamp(map(T, range(10)), T(2)))
    assert not memory_leak(test)


def test_applyfunc_normal3():
    # only high
    assert list(clamp(range(10), high=7)) == [0, 1, 2, 3, 4, 5, 6, 7]

    def test():
        list(clamp(map(T, range(10)), high=T(7)))
    assert not memory_leak(test)


def test_applyfunc_normal4():
    # both, inclusive
    assert list(clamp(range(10), low=2, high=7,
                      inclusive=True)) == [3, 4, 5, 6]

    def test():
        list(clamp(map(T, range(10)), low=T(2), high=T(7), inclusive=True))
    assert not memory_leak(test)


def test_applyfunc_normal5():
    # no low/high
    assert list(clamp(range(10))) == [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

    def test():
        list(clamp(map(T, range(10))))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_applyfunc_failure1():
    with pytest.raises(TypeError):
        list(clamp(range(10), 'a', 3))

    def test():
        with pytest.raises(TypeError):
            list(clamp(map(T, range(10)), T('a'), T(3)))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_applyfunc_failure2():
    with pytest.raises(TypeError):
        list(clamp(range(10), 3, 'a'))

    def test():
        with pytest.raises(TypeError):
            list(clamp(map(T, range(10)), T(3), T('a')))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_applyfunc_pickle1():
    clmp = clamp(range(10), 2, 7)
    assert next(clmp) == 2
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == [3, 4, 5, 6, 7]

    def test():
        clmp = clamp(map(T, range(10)), T(2), T(7))
        next(clmp)
        x = pickle.dumps(clmp)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_applyfunc_pickle2():
    # inclusive
    clmp = clamp(range(10), 2, 7, inclusive=True)
    assert next(clmp) == 3
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == [4, 5, 6]

    def test():
        clmp = clamp(map(T, range(10)), T(2), T(7), inclusive=True)
        next(clmp)
        x = pickle.dumps(clmp)
        list(pickle.loads(x))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_applyfunc_pickle3():
    # only low
    clmp = clamp(range(10), 2)
    assert next(clmp) == 2
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == [3, 4, 5, 6, 7, 8, 9]

    def test():
        clmp = clamp(map(T, range(10)), T(2))
        next(clmp)
        x = pickle.dumps(clmp)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_applyfunc_pickle4():
    # only high
    clmp = clamp(range(10), high=7)
    assert next(clmp) == 0
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == [1, 2, 3, 4, 5, 6, 7]

    def test():
        clmp = clamp(map(T, range(10)), high=T(7))
        next(clmp)
        x = pickle.dumps(clmp)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_applyfunc_pickle5():
    # only high, with inclusive
    clmp = clamp(range(10), high=7, inclusive=True)
    assert next(clmp) == 0
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == [1, 2, 3, 4, 5, 6]

    def test():
        clmp = clamp(map(T, range(10)), high=T(7), inclusive=True)
        next(clmp)
        x = pickle.dumps(clmp)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_applyfunc_pickle6():
    # only low, with inclusive
    clmp = clamp(range(10), 2, inclusive=True)
    assert next(clmp) == 3
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == [4, 5, 6, 7, 8, 9]

    def test():
        clmp = clamp(map(T, range(10)), T(2), inclusive=True)
        next(clmp)
        x = pickle.dumps(clmp)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_applyfunc_pickle7():
    # no low no high
    clmp = clamp(range(10))
    assert next(clmp) == 0
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == [1, 2, 3, 4, 5, 6, 7, 8, 9]

    def test():
        clmp = clamp(map(T, range(10)))
        next(clmp)
        x = pickle.dumps(clmp)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)
