# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
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


@memory_leak_decorator()
def test_all_equal_normal3():
    # generator
    assert all_equal(i for i in [T(1), T(1), T(1)])


@memory_leak_decorator(collect=True)
def test_all_equal_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        all_equal(_hf.FailIter())
    assert _hf.FailIter.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_all_equal_failure2():
    # comparison fail
    with pytest.raises(TypeError):
        all_equal([T(1), T('a')])


@memory_leak_decorator(collect=True)
def test_all_equal_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        all_equal(_hf.FailNext())
    assert _hf.FailNext.EXC_MSG in str(exc)
