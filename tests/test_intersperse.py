# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT


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


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle1():
    its = intersperse(toT([1, 2, 3]), T(0))
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([1, 0, 2, 0, 3])


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle2():
    its = intersperse(toT([1, 2, 3]), T(0))
    assert next(its) == T(1)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([0, 2, 0, 3])


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle3():
    its = intersperse([T(1), T(2), T(3)], T(0))
    assert next(its) == T(1)
    assert next(its) == T(0)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([2, 0, 3])


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_intersperse_pickle4():
    its = intersperse([T(1), T(2), T(3)], T(0))
    assert next(its) == T(1)
    assert next(its) == T(0)
    assert next(its) == T(2)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == toT([0, 3])
