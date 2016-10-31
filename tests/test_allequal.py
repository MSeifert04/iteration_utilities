# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


all_equal = iteration_utilities.all_equal


@memory_leak_decorator()
def test_all_equal_empty1():
    assert all_equal([])


@memory_leak_decorator()
def test_all_equal_normal1():
    assert all_equal([T(1), T(1), T(1)])


@memory_leak_decorator()
def test_all_equal_normal2():
    assert not all_equal([T(1), T(1), T(2)])


@memory_leak_decorator(collect=True)
def test_all_equal_failure1():
    # not iterable
    with pytest.raises(TypeError):
        all_equal(T(1))


@memory_leak_decorator(collect=True)
def test_all_equal_failure2():
    # comparison fail
    with pytest.raises(TypeError):
        all_equal([T(1), T('a')])
