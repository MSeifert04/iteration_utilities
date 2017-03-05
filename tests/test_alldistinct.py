# Built-ins
from __future__ import absolute_import, division, print_function
import itertools
import operator

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


if iteration_utilities.EQ_PY2:
    filter = itertools.ifilter


all_distinct = iteration_utilities.all_distinct


@memory_leak_decorator()
def test_alldistinct_empty1():
    assert all_distinct([])


@memory_leak_decorator()
def test_alldistinct_normal1():
    assert all_distinct([T(1), T(2), T(3)])


@memory_leak_decorator()
def test_alldistinct_normal2():
    assert not all_distinct([T(1), T(1), T(1)])


@memory_leak_decorator()
def test_alldistinct_unhashable1():
    assert all_distinct([{T('a'): T(1)}, {T('a'): T(2)}])


@memory_leak_decorator()
def test_alldistinct_unhashable2():
    assert not all_distinct([{T('a'): T(1)}, {T('a'): T(1)}])


@memory_leak_decorator(collect=True)
def test_alldistinct_failure1():
    # iterable is not iterable
    with pytest.raises(TypeError):
        all_distinct(T(1))


@memory_leak_decorator(collect=True)
def test_alldistinct_failure2():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError):
        all_distinct(filter(operator.eq, zip([T(1)], [T(1)])))
