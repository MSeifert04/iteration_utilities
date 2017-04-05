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


any_isinstance = iteration_utilities.any_isinstance


@memory_leak_decorator()
def test_anyisinstance_empty1():
    assert not any_isinstance([], T)


@memory_leak_decorator()
def test_anyisinstance_normal1():
    assert not any_isinstance(toT([1, 2, 3]), int)


@memory_leak_decorator()
def test_anyisinstance_normal2():
    assert any_isinstance(toT([1, 2, 3]), T)


@memory_leak_decorator()
def test_anyisinstance_normal3():
    assert any_isinstance(toT([1, 2, 3]) + [10], int)


@memory_leak_decorator()
def test_anyisinstance_normal4():
    # using a generator (raises a StopIteration)
    assert not any_isinstance((i for i in toT([1, 2, 3])), int)


@memory_leak_decorator(collect=True)
def test_anyisinstance_failure1():
    # iterable is not iterable
    with pytest.raises(TypeError):
        any_isinstance(T(1), T)


@memory_leak_decorator(collect=True)
def test_anyisinstance_failure2():
    # not enough arguments
    with pytest.raises(TypeError):
        any_isinstance([T(1)])


@memory_leak_decorator(collect=True)
def test_anyisinstance_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        any_isinstance(failingTIterator(), T)
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_anyisinstance_failure4():
    # Test failing isinstance operation
    with pytest.raises(TypeError) as exc:
        any_isinstance(toT([1, 2, 3]), FailingIsinstanceClass)
    assert 'isinstance failed' in str(exc)
