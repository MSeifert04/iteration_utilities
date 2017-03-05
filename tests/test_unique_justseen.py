# Built-ins
from __future__ import absolute_import, division, print_function
import itertools
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT


if iteration_utilities.EQ_PY2:
    filter = itertools.ifilter


unique_justseen = iteration_utilities.unique_justseen


class T2(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        raise TypeError()

    def __ne__(self, other):
        raise TypeError()


@memory_leak_decorator()
def test_unique_justseen_empty1():
    assert list(unique_justseen([])) == [] == []


@memory_leak_decorator()
def test_unique_justseen_normal1():
    assert list(unique_justseen(toT([1, 1, 2, 2, 3, 3]))) == toT([1, 2, 3])


@memory_leak_decorator()
def test_unique_justseen_normal2():
    assert list(unique_justseen('aAabBb')) == ['a', 'A', 'a', 'b', 'B', 'b']


@memory_leak_decorator()
def test_unique_justseen_normal3():
    assert list(unique_justseen('aAabBb', key=str.lower)) == ['a', 'b']


@memory_leak_decorator(collect=True)
def test_unique_justseen_failure1():
    # not iterable
    with pytest.raises(TypeError):
        unique_justseen(T(1))


@memory_leak_decorator(collect=True)
def test_unique_justseen_failure2():
    with pytest.raises(TypeError):  # function call fails
        list(unique_justseen([T(1), T(2), T(3)], key=lambda x: x + 'a'))


@memory_leak_decorator(collect=True)
def test_unique_justseen_failure3():
    # objects do not support eq or ne
    with pytest.raises(TypeError):
        list(unique_justseen([T2(1), T2(2)]))


@memory_leak_decorator(collect=True)
def test_unique_justseen_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(unique_justseen(filter(operator.eq, zip([T(1)], [T(1)]))))
    assert 'op_eq expected 2 arguments, got 1' in str(exc)


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_unique_justseen_pickle1():
    ujs = unique_justseen([T(1), T(2), T(3)])
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == toT([1, 2, 3])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_unique_justseen_pickle2():
    ujs = unique_justseen([T(1), T(2), T(3)])
    assert next(ujs) == T(1)
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == toT([2, 3])


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='see method comments')
@memory_leak_decorator(offset=1)
def test_unique_justseen_pickle3():
    # Pickling a method descriptor is not possible for Python 3.3 and before
    # Also operator.methodcaller loses it's methodname when pickled for Python
    #   3.4 and lower...
    ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == ['a']


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='see method comments')
@memory_leak_decorator(offset=1)
def test_unique_justseen_pickle4():
    # Pickling a method descriptor is not possible for Python 3.3 and before
    # Also operator.methodcaller loses it's methodname when pickled for Python
    #   3.4 and lower...
    ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
    assert next(ujs) == 'a'
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == []
