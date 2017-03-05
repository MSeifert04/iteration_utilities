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


one = iteration_utilities.one


@memory_leak_decorator()
def test_one_normal1():
    assert one([T(0)]) == T(0)


@memory_leak_decorator()
def test_one_normal2():
    assert one('a') == 'a'


@memory_leak_decorator()
def test_one_normal3():
    assert one({T('o'): T(10)}) == T('o')


@memory_leak_decorator(collect=True)
def test_one_failure1():
    with pytest.raises(TypeError):
        one(T(0))


@memory_leak_decorator(collect=True)
def test_one_failure2():
    # empty iterable
    with pytest.raises(ValueError):
        one([])


@memory_leak_decorator(collect=True)
def test_one_failure3():
    # more than 1 element
    with pytest.raises(ValueError):
        one([T(1), T(2)])


@memory_leak_decorator(collect=True)
def test_one_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(ValueError):
        one(filter(operator.eq, zip([T(1)], [T(1)])))


@memory_leak_decorator(collect=True)
def test_one_failure5():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError):
        one(itertools.chain([T(1)], filter(operator.eq, zip([T(1)], [T(1)]))))
