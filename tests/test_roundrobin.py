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


roundrobin = iteration_utilities.roundrobin


@memory_leak_decorator()
def test_roundrobin_empty1():
    assert list(roundrobin()) == []


@memory_leak_decorator()
def test_roundrobin_empty2():
    assert list(roundrobin([])) == []


@memory_leak_decorator()
def test_roundrobin_empty3():
    assert list(roundrobin([], (), {})) == []


@memory_leak_decorator()
def test_roundrobin_normal1():
    assert list(roundrobin([T(1)], [T(1), T(2)], [T(1), T(2), T(3)]
                           )) == toT([1, 1, 1, 2, 2, 3])


@memory_leak_decorator()
def test_roundrobin_normal2():
    assert list(roundrobin([T(1), T(2), T(3)], [T(1)], [T(1), T(2)]
                           )) == toT([1, 1, 1, 2, 2, 3])


@memory_leak_decorator()
def test_roundrobin_normal3():
    assert list(roundrobin([T(1), T(2)], [T(1), T(2), T(3)], [T(1)]
                           )) == toT([1, 1, 1, 2, 2, 3])


@memory_leak_decorator(collect=True)
def test_roundrobin_failure1():
    with pytest.raises(TypeError):
        list(roundrobin(T(1)))


@memory_leak_decorator(collect=True)
def test_roundrobin_failure2():
    with pytest.raises(TypeError):
        list(roundrobin([T(1)], T(1)))


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_roundrobin_pickle1():
    rr = roundrobin([T(1), T(2), T(3)], [T(1), T(2), T(3)])
    assert next(rr) == T(1)
    x = pickle.dumps(rr)
    assert list(pickle.loads(x)) == toT([1, 2, 2, 3, 3])


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_roundrobin_pickle2():
    rr2 = roundrobin([T(1)], [T(1), T(2), T(3)])
    assert next(rr2) == T(1)
    assert next(rr2) == T(1)
    assert next(rr2) == T(2)
    x = pickle.dumps(rr2)
    assert list(pickle.loads(x)) == [T(3)]
