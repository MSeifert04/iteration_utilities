# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT, failingTIterator


intersperse = iteration_utilities.intersperse


@memory_leak_decorator()
def test_intersperse_empty1():
    assert list(intersperse([], T(0))) == []


@memory_leak_decorator()
def test_intersperse_empty2():
    assert list(intersperse([T(1)], T(0))) == [T(1)]


@memory_leak_decorator()
def test_intersperse_normal1():
    assert list(intersperse([T(1), T(2)], T(0))) == toT([1, 0, 2])


@memory_leak_decorator(collect=True)
def test_intersperse_failure1():
    with pytest.raises(TypeError):
        intersperse(T(100), T(0))


@memory_leak_decorator(collect=True)
def test_intersperse_failure2():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(intersperse(failingTIterator(), T(0)))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle1():
    its = intersperse(toT([1, 2, 3]), T(0))
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([1, 0, 2, 0, 3])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle2():
    its = intersperse(toT([1, 2, 3]), T(0))
    assert next(its) == T(1)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([0, 2, 0, 3])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle3():
    its = intersperse([T(1), T(2), T(3)], T(0))
    assert next(its) == T(1)
    assert next(its) == T(0)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([2, 0, 3])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle4():
    its = intersperse([T(1), T(2), T(3)], T(0))
    assert next(its) == T(1)
    assert next(its) == T(0)
    assert next(its) == T(2)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([0, 3])


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_intersperse_lengthhint1():
    it = intersperse([1, 2, 3], 2)
    assert operator.length_hint(it) == 5
    next(it)
    assert operator.length_hint(it) == 4
    next(it)
    assert operator.length_hint(it) == 3
    next(it)
    assert operator.length_hint(it) == 2
    next(it)
    assert operator.length_hint(it) == 1
    next(it)
    assert operator.length_hint(it) == 0
