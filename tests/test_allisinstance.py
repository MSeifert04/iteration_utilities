# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT, failingTIterator, FailingIsinstanceClass


all_isinstance = iteration_utilities.all_isinstance


@memory_leak_decorator()
def test_allisinstance_empty1():
    assert all_isinstance([], T)


@memory_leak_decorator()
def test_allisinstance_normal1():
    assert all_isinstance(toT([1, 2, 3]), T)


@memory_leak_decorator()
def test_allisinstance_normal2():
    assert not all_isinstance(toT([1, 2, 3]) + [10], T)


@memory_leak_decorator()
def test_allisinstance_normal3():
    # using a generator (raises a StopIteration)
    assert all_isinstance((i for i in toT([1, 2, 3])), T)


@memory_leak_decorator(collect=True)
def test_allisinstance_failure1():
    # iterable is not iterable
    with pytest.raises(TypeError):
        all_isinstance(T(1), T)


@memory_leak_decorator(collect=True)
def test_allisinstance_failure2():
    # not enough arguments
    with pytest.raises(TypeError):
        all_isinstance([T(1)])


@memory_leak_decorator(collect=True)
def test_allisinstance_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        all_isinstance(failingTIterator(), T)
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_allisinstance_failure4():
    # Test failing isinstance operation
    with pytest.raises(TypeError) as exc:
        all_isinstance(toT([1, 2, 3]), FailingIsinstanceClass)
    assert 'isinstance failed' in str(exc)
