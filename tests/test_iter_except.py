# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


iter_except = iteration_utilities.iter_except


@memory_leak_decorator()
def test_iterexcept_normal1():
    assert list(iter_except(({T('a'): T(10)}).popitem, KeyError
                            )) == [(T('a'), T(10))]


@memory_leak_decorator()
def test_iterexcept_first():
    d = {}

    def insert():
        d[T('a')] = T(10)

    assert list(iter_except(d.popitem, KeyError, insert
                            )) == [None, (T('a'), T(10))]


@memory_leak_decorator(collect=True)
def test_iterexcept_failure1():
    # wrong exception
    with pytest.raises(KeyError):
        list(iter_except(({T('a'): T(10)}).popitem, ValueError))


@memory_leak_decorator(offset=1)
def test_iterexcept_pickle1():
    # TODO: Implement some tests for pickle!
    pass
