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


dotproduct = iteration_utilities.dotproduct


@memory_leak_decorator()
def test_dotproduct_empty1():
    assert dotproduct([], []) == 0


@memory_leak_decorator()
def test_dotproduct_normal1():
    assert dotproduct([T(1), T(2), T(3)], [T(1), T(2), T(3)]) == T(14)


@memory_leak_decorator()
def test_dotproduct_normal2():
    assert dotproduct([T(100), T(200), T(300)],
                      [T(100), T(200), T(300)]) == T(140000)


@memory_leak_decorator(collect=True)
def test_dotproduct_failure1():
    # first iterable is not iterable
    with pytest.raises(TypeError):
        dotproduct(T(1), [T(1)])


@memory_leak_decorator(collect=True)
def test_dotproduct_failure2():
    # second iterable is not iterable
    with pytest.raises(TypeError):
        dotproduct([T(1)], T(1))


@memory_leak_decorator(collect=True)
def test_dotproduct_failure3():
    # multiplication fails
    with pytest.raises(TypeError):
        dotproduct([T(1)], [1])


@memory_leak_decorator(collect=True)
def test_dotproduct_failure4():
    # multiplication fails (later)
    with pytest.raises(TypeError):
        dotproduct([T(1), T(1)], [T(1), 1])


@memory_leak_decorator(collect=True)
def test_dotproduct_failure5():
    # addition fails
    with pytest.raises(TypeError):
        dotproduct([T(1), 1], [T(1), 1])


@memory_leak_decorator(collect=True)
def test_dotproduct_failure6():
    # addition fails (inverted)
    with pytest.raises(TypeError):
        dotproduct([1, T(1), 1], [1, T(1), 1])


@memory_leak_decorator(collect=True)
def test_dotproduct_failure7():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError):
        dotproduct(filter(operator.eq, zip([T(1)], [T(1)])))
