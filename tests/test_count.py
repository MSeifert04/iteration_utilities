# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, failingTIterator


count_items = iteration_utilities.count_items


@memory_leak_decorator()
def test_count_empty1():
    assert count_items([]) == 0


@memory_leak_decorator()
def test_count_normal1():
    assert count_items([T(0), T(0)]) == 2


@memory_leak_decorator()
def test_count_normal2():
    assert count_items([T(0), T(0), T(1)], bool) == 1


@memory_leak_decorator()
def test_count_normal3():
    # None as pred is equal to not giving any predicate
    assert count_items([T(0), T(0), T(1), T(1)], None) == 4


@memory_leak_decorator()
def test_count_normal4():
    assert count_items([], iteration_utilities.return_identity) == 0


@memory_leak_decorator()
def test_count_normal5():
    assert count_items([T(1), T(2), T(3)], lambda x: x > T(2)) == 1


@memory_leak_decorator()
def test_count_normal6():
    assert count_items([T(1), T(2), T(3)], lambda x: x < T(3)) == 2


@memory_leak_decorator()
def test_count_normal7():
    assert count_items([T(3), T(1), T(2), T(3), T(3)], T(3), True) == 3


@memory_leak_decorator(collect=True)
def test_count_failure1():
    with pytest.raises(TypeError):
        count_items(T(1))


@memory_leak_decorator(collect=True)
def test_count_failure2():
    with pytest.raises(TypeError):
        count_items([T(1)], T(1))


@memory_leak_decorator(collect=True)
def test_count_failure3():
    # Regression test when accessing the next item of the iterable resulted
    # in an Exception. For example when the iterable was a filter and the
    # filter function threw an exception.
    with pytest.raises(TypeError) as exc:
        count_items(failingTIterator())
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_count_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        count_items()


@memory_leak_decorator(collect=True)
def test_count_failure5():
    # eq True but no pred
    with pytest.raises(TypeError):
        count_items([T(0)], eq=True)


@memory_leak_decorator(collect=True)
def test_count_failure6():
    # eq True but pred None (like not given)
    with pytest.raises(TypeError):
        count_items([T(0)], pred=None, eq=True)


@memory_leak_decorator(collect=True)
def test_count_failure7():
    # function returns item without boolean interpretation

    class NoBool(object):
        def __bool__(self):
            raise ValueError('bad class')
        __nonzero__ = __bool__

    with pytest.raises(ValueError) as exc:
        count_items([T(0)], lambda x: NoBool())
    assert 'bad class' in str(exc)
