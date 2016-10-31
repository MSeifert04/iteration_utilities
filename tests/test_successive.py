# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


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


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_successive_pickle1():
    suc = successive([T(1), T(2), T(3), T(4)])
    assert next(suc) == (T(1), T(2))
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [(T(2), T(3)), (T(3), T(4))]


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_successive_pickle2():
    suc = successive([T(1), T(2), T(3), T(4)])
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [(T(1), T(2)), (T(2), T(3)), (T(3), T(4))]
