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


@memory_leak_decorator()
def test_dotproduct_normal3():
    # generators
    assert dotproduct((i for i in [T(1), T(2), T(3)]),
                      (i for i in [T(1), T(2), T(3)])) == T(14)


@memory_leak_decorator(collect=True)
def test_dotproduct_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        dotproduct(_hf.FailIter(), [T(1)])
    assert _hf.FailIter.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_dotproduct_failure2():
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        dotproduct([T(1)], _hf.FailIter())
    assert _hf.FailIter.EXC_MSG in str(exc)


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
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        dotproduct(_hf.FailNext(), _hf.FailNext())
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_dotproduct_failure8():
    # Too few arguments
    with pytest.raises(TypeError):
        dotproduct()
