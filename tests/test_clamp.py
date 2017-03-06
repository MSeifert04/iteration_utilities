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


clamp = iteration_utilities.clamp


@memory_leak_decorator()
def test_clamp_empty1():
    assert list(clamp([], T(10), T(100))) == []


@memory_leak_decorator()
def test_clamp_normal1():
    assert list(clamp(toT(range(10)), T(2), T(7))) == toT([2, 3, 4, 5, 6, 7])


@memory_leak_decorator()
def test_clamp_normal2():
    # only low
    assert list(clamp(toT(range(10)), T(2))) == toT([2, 3, 4, 5, 6, 7, 8, 9])


@memory_leak_decorator()
def test_clamp_normal3():
    # only high
    assert list(clamp(toT(range(10)),
                      high=T(7))) == toT([0, 1, 2, 3, 4, 5, 6, 7])


@memory_leak_decorator()
def test_clamp_normal4():
    # both, inclusive
    assert list(clamp(toT(range(10)), low=T(2), high=T(7),
                      inclusive=True)) == toT([3, 4, 5, 6])


@memory_leak_decorator()
def test_clamp_normal5():
    # no low/high
    assert list(clamp(toT(range(10)))) == toT([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])


@memory_leak_decorator()
def test_clamp_normal6():
    # only low without remove
    assert list(clamp(toT(range(10)), T(2), remove=False)) == (
            toT([2, 2, 2, 3, 4, 5, 6, 7, 8, 9]))


@memory_leak_decorator()
def test_clamp_normal7():
    # only high without remove
    assert list(clamp(toT(range(10)), high=T(7),
                      remove=False)) == toT([0, 1, 2, 3, 4, 5, 6, 7, 7, 7])


@memory_leak_decorator()
def test_clamp_normal8():
    # both without remove
    assert list(clamp(toT(range(10)), low=T(2), high=T(7),
                      remove=False)) == toT([2, 2, 2, 3, 4, 5, 6, 7, 7, 7])


@memory_leak_decorator(collect=True)
def test_clamp_failure1():
    with pytest.raises(TypeError):
        list(clamp(toT(range(10)), T('a'), T(3)))


@memory_leak_decorator(collect=True)
def test_clamp_failure2():
    with pytest.raises(TypeError):
        list(clamp(map(T, range(10)), T(3), T('a')))


@memory_leak_decorator(collect=True)
def test_clamp_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(clamp(failingTIterator()))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle1():
    clmp = clamp(toT(range(10)), T(2), T(7))
    assert next(clmp) == T(2)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([3, 4, 5, 6, 7])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle2():
    # inclusive
    clmp = clamp(map(T, range(10)), T(2), T(7), inclusive=True)
    assert next(clmp) == T(3)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([4, 5, 6])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle3():
    # only low
    clmp = clamp(map(T, range(10)), T(2))
    assert next(clmp) == T(2)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([3, 4, 5, 6, 7, 8, 9])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle4():
    # only high
    clmp = clamp(map(T, range(10)), high=T(7))
    assert next(clmp) == T(0)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([1, 2, 3, 4, 5, 6, 7])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle5():
    # only high, with inclusive
    clmp = clamp(map(T, range(10)), high=T(7), inclusive=True)
    assert next(clmp) == T(0)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([1, 2, 3, 4, 5, 6])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle6():
    # only low, with inclusive
    clmp = clamp(map(T, range(10)), T(2), inclusive=True)
    assert next(clmp) == T(3)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([4, 5, 6, 7, 8, 9])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle7():
    # no low no high
    clmp = clamp(map(T, range(10)))
    assert next(clmp) == T(0)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([1, 2, 3, 4, 5, 6, 7, 8, 9])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_clamp_pickle8():
    # only high but without remove
    clmp = clamp(map(T, range(10)), high=T(7), remove=False)
    assert next(clmp) == T(0)
    x = pickle.dumps(clmp)
    assert list(pickle.loads(x)) == toT([1, 2, 3, 4, 5, 6, 7, 7, 7])


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_clamp_lengthhint1():
    # When remove=False we can determine the length-hint.
    it = clamp(toT(range(5)), low=T(2), high=T(5), remove=False)
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


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_clamp_lengthhint2():
    # When low and high are not given we can determine the length-hint
    it = clamp(toT(range(5)))
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


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_clamp_lengthhint3():
    # Only works if "remove=False", otherwise the length-hint simply returns 0.
    it = clamp(toT(range(5)), low=T(2), high=T(5), remove=True)
    assert operator.length_hint(it) == 0
