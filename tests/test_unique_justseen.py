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


unique_justseen = iteration_utilities.unique_justseen


class T(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        return self.value == other.value


class T2(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        raise TypeError()

    def __ne__(self, other):
        raise TypeError()


class T3(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        raise TypeError()

    def __ne__(self, other):
        return self.value != other.value


def test_unique_justseen_empty1():
    assert list(unique_justseen([])) == []

    def test():
        list(unique_justseen([])) == []
    assert not memory_leak(test)


def test_unique_justseen_normal1():
    assert list(unique_justseen([1, 1, 2, 3, 3])) == [1, 2, 3]

    def test():
        list(unique_justseen([T(1), T(1), T(2), T(3), T(3)]))
    assert not memory_leak(test)


def test_unique_justseen_normal2():
    assert list(unique_justseen('aAabBb')) == ['a', 'A', 'a', 'b', 'B', 'b']

    def test():
        list(unique_justseen([T(1), T(-1), T(1),
                              T(2), T(-2), T(2)]))
    assert not memory_leak(test)


def test_unique_justseen_normal3():
    assert list(unique_justseen('aAabBb', key=str.lower)) == ['a', 'b']

    def test():
        list(unique_justseen([T(1), T(-1), T(1),
                              T(2), T(-2), T(2)],
                             key=lambda x: abs(x.value)))
    assert not memory_leak(test)


def test_unique_justseen_failure1():
    # not iterable
    with pytest.raises(TypeError):
        unique_justseen(10)

    def test():
        with pytest_raises(TypeError):
            unique_justseen(T(1))
    assert not memory_leak(test)


def test_unique_justseen_failure2():
    with pytest.raises(TypeError):  # function call fails
        list(unique_justseen([1, 2, 3], key=lambda x: x + 'a'))

    def test():
        with pytest_raises(TypeError):  # function call fails
            list(unique_justseen([T(1), T(2), T(3)],
                                 key=lambda x: x + 'a'))
    assert not memory_leak(test)


def test_unique_justseen_failure3():
    # objects do not support eq or ne
    with pytest.raises(TypeError):
        list(unique_justseen([T2(1), T2(2)]))

    def test():
        with pytest_raises(TypeError):
            list(unique_justseen([T2(1), T2(2)]))
    assert not memory_leak(test)


def test_unique_justseen_failure4():
    res = list(unique_justseen([T3(1), T3(1)]))
    assert len(res) == 1
    assert isinstance(res[0], T3)
    assert res[0].value == 1

    def test():
        list(unique_justseen([T3(1), T3(1)]))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_unique_justseen_pickle1():
    ujs = unique_justseen([1, 2, 3])
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [1, 2, 3]

    def test():
        ujs = unique_justseen([T(1), T(2), T(3)])
        x = pickle.dumps(ujs)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_unique_justseen_pickle2():
    ujs = unique_justseen([1, 2, 3])
    assert next(ujs) == 1
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [2, 3]

    def test():
        ujs = unique_justseen([T(1), T(2), T(3)])
        next(ujs)
        x = pickle.dumps(ujs)
        list(pickle.loads(x))
    assert not memory_leak(test)


@pytest.mark.xfail(not iteration_utilities.PY34, reason='see method comments')
def test_unique_justseen_pickle3():
    # Pickling a method descriptor is not possible for Python 3.3 and before
    # Also operator.methodcaller loses it's methodname when pickled for Python
    #   3.4 and lower...
    ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == ['a']

    def test():
        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        x = pickle.dumps(ujs)
        list(pickle.loads(x))
    assert not memory_leak(test)


@pytest.mark.xfail(not iteration_utilities.PY34, reason='see method comments')
def test_unique_justseen_pickle4():
    # Pickling a method descriptor is not possible for Python 3.3 and before
    # Also operator.methodcaller loses it's methodname when pickled for Python
    #   3.4 and lower...
    ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
    assert next(ujs) == 'a'
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == []

    def test():
        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        next(ujs)
        x = pickle.dumps(ujs)
        list(pickle.loads(x))
    assert not memory_leak(test)
