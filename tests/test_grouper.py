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


grouper = iteration_utilities.grouper


@memory_leak_decorator()
def test_grouper_empty1():
    # Empty iterable
    assert list(grouper([], 2)) == []


@memory_leak_decorator()
def test_grouper_normal1():
    # no fillvalue + truncate
    assert list(grouper([T(1)], 3)) == [(T(1), )]


@memory_leak_decorator()
def test_grouper_normal2():
    assert list(grouper([T(1), T(2)], 3)) == [(T(1), T(2))]


@memory_leak_decorator()
def test_grouper_normal3():
    assert list(grouper([T(1), T(2), T(3)], 3)) == [(T(1), T(2), T(3))]


@memory_leak_decorator()
def test_grouper_normal4():
    assert list(grouper([T(1), T(2), T(3), T(4)], 3)) == [(T(1), T(2), T(3)),
                                                          (T(4), )]


@memory_leak_decorator()
def test_grouper_normal5():
    assert list(grouper(toT([1, 2, 3, 4, 5]), 3)) == [(T(1), T(2), T(3)),
                                                      (T(4), T(5))]


@memory_leak_decorator()
def test_grouper_normal6():
    assert list(grouper(toT([1, 2, 3, 4, 5, 6]), 3)) == [(T(1), T(2), T(3)),
                                                         (T(4), T(5), T(6))]


@memory_leak_decorator()
def test_grouper_normal7():
    # generator
    assert list(grouper((i for i in toT([1, 2, 3, 4, 5, 6])), 3)
                ) == [(T(1), T(2), T(3)), (T(4), T(5), T(6))]


@memory_leak_decorator()
def test_grouper_fill1():
    # with fillvalue
    assert list(grouper(toT([1]), 3,
                        fillvalue=T(0))) == [(T(1), T(0), T(0))]


@memory_leak_decorator()
def test_grouper_fill2():
    assert list(grouper(toT([1, 2]), 3,
                        fillvalue=T(0))) == [(T(1), T(2), T(0))]


@memory_leak_decorator()
def test_grouper_fill3():
    assert list(grouper(toT([1, 2, 3]), 3,
                        fillvalue=T(0))) == [(T(1), T(2), T(3))]


@memory_leak_decorator()
def test_grouper_fill4():
    assert list(grouper(toT([1, 2, 3, 4]), 3,
                        fillvalue=T(0))) == [(T(1), T(2), T(3)),
                                             (T(4), T(0), T(0))]


@memory_leak_decorator()
def test_grouper_fill5():
    assert list(grouper(toT([1, 2, 3, 4, 5]), 3,
                        fillvalue=T(0))) == [(T(1), T(2), T(3)),
                                             (T(4), T(5), T(0))]


@memory_leak_decorator()
def test_grouper_fill6():
    assert list(grouper(toT([1, 2, 3, 4, 5, 6]), 3,
                        fillvalue=T(0))) == [(T(1), T(2), T(3)),
                                             (T(4), T(5), T(6))]


@memory_leak_decorator()
def test_grouper_truncate1():
    # with truncate
    assert list(grouper(toT([1]), 3, truncate=True)) == []


@memory_leak_decorator()
def test_grouper_truncate2():
    assert list(grouper(toT([1, 2]), 3, truncate=True)) == []


@memory_leak_decorator()
def test_grouper_truncate3():
    assert list(grouper(toT([1, 2, 3]), 3,
                        truncate=True)) == [(T(1), T(2), T(3))]


@memory_leak_decorator()
def test_grouper_truncate4():
    assert list(grouper(toT([1, 2, 3, 4]), 3,
                        truncate=True)) == [(T(1), T(2), T(3))]


@memory_leak_decorator()
def test_grouper_truncate5():
    assert list(grouper(toT([1, 2, 3, 4, 5]), 3,
                        truncate=True)) == [(T(1), T(2), T(3))]


@memory_leak_decorator()
def test_grouper_truncate6():
    assert list(grouper(toT([1, 2, 3, 4, 5, 6]), 3,
                        truncate=True)) == [(T(1), T(2), T(3)),
                                            (T(4), T(5), T(6))]


@memory_leak_decorator(collect=True)
def test_grouper_failure1():
    # fillvalue + truncate is forbidden
    with pytest.raises(TypeError):
        grouper(toT([1, 2, 3]), 2, fillvalue=T(0), truncate=True)


@memory_leak_decorator(collect=True)
def test_grouper_failure2():
    # n must be > 0
    with pytest.raises(ValueError):
        grouper(toT([1, 2, 3]), 0)


@memory_leak_decorator(collect=True)
def test_grouper_failure3():
    # iterable must be iterable
    with pytest.raises(TypeError):
        grouper(T(1), 2)


@memory_leak_decorator(collect=True)
def test_grouper_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(grouper(failingTIterator(), 2))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_grouper_failure5():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(grouper(failingTIterator(offset=1), 2))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_grouper_failure6():
    # Too few arguments
    with pytest.raises(TypeError):
        grouper()
    with pytest.raises(TypeError):
        grouper(toT([1, 2, 3, 4]))


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_grouper_pickle1():
    grp = grouper(toT(range(10)), 3)
    assert next(grp) == (T(0), T(1), T(2))
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(T(3), T(4), T(5)), (T(6), T(7), T(8)),
                                     (T(9), )]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_grouper_pickle2():
    grp = grouper(toT(range(10)), 3, fillvalue=T(0))
    assert next(grp) == (T(0), T(1), T(2))
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(T(3), T(4), T(5)), (T(6), T(7), T(8)),
                                     (T(9), T(0), T(0))]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_grouper_pickle3():
    grp = grouper(toT(range(10)), 3, truncate=True)
    assert next(grp) == (T(0), T(1), T(2))
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(T(3), T(4), T(5)), (T(6), T(7), T(8))]


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_grouper_lengthhint1():
    assert operator.length_hint(grouper([1, 2, 3, 4, 5],
                                        2, truncate=True)) == 2
    assert operator.length_hint(grouper([1, 2, 3, 4, 5, 6],
                                        2, truncate=True)) == 3
    assert operator.length_hint(grouper([1, 2, 3, 4, 5], 2)) == 3
    assert operator.length_hint(grouper([1, 2, 3, 4, 5, 6],
                                        2)) == 3
    assert operator.length_hint(grouper([1, 2, 3, 4, 5],
                                        2, fillvalue=None)) == 3
    assert operator.length_hint(grouper([1, 2, 3, 4, 5, 6],
                                        2, fillvalue=None)) == 3
