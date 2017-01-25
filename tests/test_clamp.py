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


clamp = iteration_utilities.clamp


@memory_leak_decorator()
def test_applyfunc_empty1():
    assert list(clamp([], T(10), T(100))) == []


@memory_leak_decorator()
def test_applyfunc_normal1():
    assert list(clamp(toT(range(10)), T(2), T(7))) == toT([2, 3, 4, 5, 6, 7])


@memory_leak_decorator()
def test_applyfunc_normal2():
    # only low
    assert list(clamp(toT(range(10)), T(2))) == toT([2, 3, 4, 5, 6, 7, 8, 9])


@memory_leak_decorator()
def test_applyfunc_normal3():
    # only high
    assert list(clamp(toT(range(10)),
                      high=T(7))) == toT([0, 1, 2, 3, 4, 5, 6, 7])


@memory_leak_decorator()
def test_applyfunc_normal4():
    # both, inclusive
    assert list(clamp(toT(range(10)), low=T(2), high=T(7),
                      inclusive=True)) == toT([3, 4, 5, 6])


@memory_leak_decorator()
def test_applyfunc_normal5():
    # no low/high
    assert list(clamp(toT(range(10)))) == toT([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])


@memory_leak_decorator(collect=True)
def test_applyfunc_failure1():
    with pytest.raises(TypeError):
        list(clamp(toT(range(10)), T('a'), T(3)))


@memory_leak_decorator(collect=True)
def test_applyfunc_failure2():
    with pytest.raises(TypeError):
        list(clamp(map(T, range(10)), T(3), T('a')))


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_applyfunc_pickle1():
    clmp = clamp(toT(range(10)), T(2), T(7))
    assert next(clmp) == T(2)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([3, 4, 5, 6, 7])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_applyfunc_pickle2():
    # inclusive
    clmp = clamp(map(T, range(10)), T(2), T(7), inclusive=True)
    assert next(clmp) == T(3)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([4, 5, 6])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_applyfunc_pickle3():
    # only low
    clmp = clamp(map(T, range(10)), T(2))
    assert next(clmp) == T(2)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([3, 4, 5, 6, 7, 8, 9])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_applyfunc_pickle4():
    # only high
    clmp = clamp(map(T, range(10)), high=T(7))
    assert next(clmp) == T(0)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([1, 2, 3, 4, 5, 6, 7])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_applyfunc_pickle5():
    # only high, with inclusive
    clmp = clamp(map(T, range(10)), high=T(7), inclusive=True)
    assert next(clmp) == T(0)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([1, 2, 3, 4, 5, 6])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_applyfunc_pickle6():
    # only low, with inclusive
    clmp = clamp(map(T, range(10)), T(2), inclusive=True)
    assert next(clmp) == T(3)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([4, 5, 6, 7, 8, 9])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_applyfunc_pickle7():
    # no low no high
    clmp = clamp(map(T, range(10)))
    assert next(clmp) == T(0)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([1, 2, 3, 4, 5, 6, 7, 8, 9])
