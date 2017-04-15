# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
from helper_cls import T
from helper_leak import memory_leak_decorator


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


@memory_leak_decorator()
def test_one_normal4():
    # generator with one item
    assert one(i for i in [T(0)]) == T(0)


@memory_leak_decorator(collect=True)
def test_one_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        one(_hf.FailIter())
    assert _hf.FailIter.EXC_MSG in str(exc)


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
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        one(_hf.FailNext())
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_one_failure5():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        one(_hf.FailNext(offset=1))
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_one_failure6():
    # generator without items
    with pytest.raises(ValueError):
        one(i for i in [])


@memory_leak_decorator(collect=True, offset=1)
def test_one_failure7():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP) as exc:
        one(_hf.CacheNext(1))
    assert _hf.CacheNext.EXC_MSG in str(exc)
