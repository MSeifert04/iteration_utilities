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
from helper_cls import T, failingTIterator


successive = iteration_utilities.successive


@memory_leak_decorator()
def test_successive_empty1():
    assert list(successive([])) == []


@memory_leak_decorator()
def test_successive_empty2():
    assert list(successive([T(1)])) == []


@memory_leak_decorator()
def test_successive_empty3():
    assert list(successive([], times=10)) == []


@memory_leak_decorator()
def test_successive_empty4():
    assert list(successive([T(1), T(2), T(3)], times=10)) == []


@memory_leak_decorator()
def test_successive_normal1():
    assert (list(successive([T(1), T(2), T(3), T(4)])) ==
            [(T(1), T(2)), (T(2), T(3)), (T(3), T(4))])


@memory_leak_decorator()
def test_successive_normal2():
    assert (list(successive([T(1), T(2), T(3), T(4)], times=3)) ==
            [(T(1), T(2), T(3)), (T(2), T(3), T(4))])


@memory_leak_decorator()
def test_successive_normal3():
    assert (list(successive([T(1), T(2), T(3), T(4)], times=4)) ==
            [(T(1), T(2), T(3), T(4))])


@memory_leak_decorator()
def test_successive_normal4():
    assert (dict(successive([T(1), T(2), T(3), T(4)])) ==
            {T(1): T(2), T(2): T(3), T(3): T(4)})


@memory_leak_decorator(collect=True)
def test_successive_failure1():
    with pytest.raises(TypeError):
        successive(T(1))


@memory_leak_decorator(collect=True)
def test_successive_failure2():
    with pytest.raises(ValueError):  # times must be > 0
        successive([T(1), T(2), T(3)], 0)


@memory_leak_decorator(collect=True)
def test_successive_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(successive(failingTIterator(), 1))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_successive_pickle1():
    suc = successive([T(1), T(2), T(3), T(4)])
    assert next(suc) == (T(1), T(2))
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [(T(2), T(3)), (T(3), T(4))]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_successive_pickle2():
    suc = successive([T(1), T(2), T(3), T(4)])
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [(T(1), T(2)), (T(2), T(3)),
                                     (T(3), T(4))]


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_successive_lengthhint1():
    it = successive([0]*6, 4)
    assert operator.length_hint(it) == 3
    next(it)
    assert operator.length_hint(it) == 2
    next(it)
    assert operator.length_hint(it) == 1
    next(it)
    assert operator.length_hint(it) == 0


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_successive_lengthhint2():
    assert operator.length_hint(successive([0]*6, 11)) == 0
