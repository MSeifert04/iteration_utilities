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


intersperse = iteration_utilities.intersperse


class T(object):
    def __init__(self, value):
        self.value = value


def test_intersperse_empty1():
    assert list(intersperse([], 0)) == []

    def test():
        list(intersperse([], T(0)))
    assert not memory_leak(test)


def test_intersperse_empty2():
    assert list(intersperse([1], 0)) == [1]

    def test():
        list(intersperse([T(1)], T(0)))
    assert not memory_leak(test)


def test_intersperse_normal1():
    assert list(intersperse([1, 2], 0)) == [1, 0, 2]

    def test():
        list(intersperse([T(1), T(2)], T(0)))
    assert not memory_leak(test)


def test_intersperse_failure1():
    with pytest.raises(TypeError):
        intersperse(100, 0)

    def test():
        with pytest_raises(TypeError):
            intersperse(T(100), T(0))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_intersperse_pickle1():
    its = intersperse([1, 2, 3], 0)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [1, 0, 2, 0, 3]

    """
    def test():
        its = intersperse([T(1), T(2), T(3)], T(0))
        x = pickle.dumps(its)
        list(pickle.loads(x))
    assert not memory_leak(test)
    """


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_intersperse_pickle2():
    # start value must be set!
    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 2, 0, 3]

    def test():
        its = intersperse([T(1), T(2), T(3)], T(0))
        next(its)
        x = pickle.dumps(its)
        list(pickle.loads(x))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_intersperse_pickle3():
    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [2, 0, 3]

    def test():
        its = intersperse([T(1), T(2), T(3)], T(0))
        next(its)
        next(its)
        x = pickle.dumps(its)
        list(pickle.loads(x))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_intersperse_pickle4():
    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    assert next(its) == 2
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 3]

    def test():
        its = intersperse([T(1), T(2), T(3)], T(0))
        next(its)
        next(its)
        next(its)
        x = pickle.dumps(its)
        list(pickle.loads(x))
    assert not memory_leak(test)
