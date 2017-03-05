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
from helper_cls import T


if iteration_utilities.EQ_PY2:
    filter = itertools.ifilter


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
    with pytest.raises(TypeError):
        count_items(filter(operator.eq, zip([T(1)], [T(1)])))
