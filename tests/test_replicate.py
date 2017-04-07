# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs
from helper_cls import (
    T, toT, failingTIterator, FailLengthHint, OverflowLengthHint)
from helper_leak import memory_leak_decorator


replicate = iteration_utilities.replicate


@memory_leak_decorator()
def test_replicate_empty1():
    assert list(replicate([], 3)) == []


@memory_leak_decorator()
def test_replicate_normal1():
    assert list(replicate([T(1), T(2)], 3)) == toT([1, 1, 1, 2, 2, 2])


@memory_leak_decorator()
def test_replicate_normal2():
    # using a generator
    assert list(replicate((i for i in toT([1, 2])), 2)) == toT([1, 1, 2, 2])


@memory_leak_decorator()
def test_replicate_attributes1():
    # Key+reverse function tests
    it = replicate(toT(range(5)), 3)
    assert it.times == 3
    assert it.timescurrent == 0
    with pytest.raises(AttributeError):
        it.current

    assert next(it) == T(0)

    assert it.times == 3
    assert it.timescurrent == 1
    assert it.current == T(0)


@memory_leak_decorator(collect=True)
def test_replicate_copy1():
    helper_funcs.iterator_copy(replicate([T(1), T(2)], 3))


@memory_leak_decorator(collect=True)
def test_replicate_failure1():
    # not enough arguments
    with pytest.raises(TypeError):
        replicate([T(1), T(2)])


@memory_leak_decorator(collect=True)
def test_replicate_failure2():
    # first argument not iterable
    with pytest.raises(TypeError):
        replicate(T(1), 2)


@memory_leak_decorator(collect=True)
def test_replicate_failure3():
    # second argument <= 1
    with pytest.raises(ValueError):
        replicate([T(1), T(2)], 0)


@memory_leak_decorator(collect=True)
def test_replicate_failure4():
    # second argument <= 1
    with pytest.raises(ValueError):
        replicate([T(1), T(2)], 1)


@memory_leak_decorator(collect=True)
def test_replicate_failure5():
    # iterator throws an exception different from StopIteration
    with pytest.raises(TypeError) as exc:
        list(replicate(failingTIterator(), 2))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_replicate_failure_setstate1():
    # "state" is not a tuple
    mg = replicate(toT(range(5)), 3)
    with pytest.raises(TypeError):
        mg.__setstate__([None, 0])


@memory_leak_decorator(collect=True)
def test_replicate_failure_setstate2():
    # setstate has an invalid second item in "state" < 0
    mg = replicate(toT(range(5)), 3)
    with pytest.raises(ValueError):
        mg.__setstate__((None, -1))


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_replicate_pickle1():
    # normal
    rpl = replicate([T(1), T(2)], 3)
    x = pickle.dumps(rpl)
    assert list(pickle.loads(x)) == toT([1, 1, 1, 2, 2, 2])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_replicate_pickle2():
    # normal
    rpl = replicate([T(1), T(2)], 3)
    assert next(rpl) == T(1)
    x = pickle.dumps(rpl)
    assert list(pickle.loads(x)) == toT([1, 1, 2, 2, 2])


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_replicate_lengthhint1():
    it = replicate([T(1), T(2)], 3)
    assert operator.length_hint(it) == 6
    next(it)
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
@memory_leak_decorator(collect=True)
def test_replicate_failure_lengthhint1():
    it = replicate(FailLengthHint(toT([1, 2, 3])), 3)
    with pytest.raises(ValueError) as exc:
        operator.length_hint(it)
    assert 'length_hint failed' in str(exc)

    with pytest.raises(ValueError) as exc:
        list(it)
    assert 'length_hint failed' in str(exc)


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_replicate_failure_lengthhint2():
    # This only checks for overflow if the length_hint is above PY_SSIZE_T_MAX
    of_it = OverflowLengthHint(toT([1, 2, 3]), OverflowLengthHint.maxsize + 1)
    it = replicate(of_it, 3)
    with pytest.raises(OverflowError):
        operator.length_hint(it)
