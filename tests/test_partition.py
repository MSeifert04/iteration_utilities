# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
from helper_cls import T, toT
from helper_leak import memory_leak_decorator


partition = iteration_utilities.partition


@memory_leak_decorator()
def test_partition_empty1():
    assert partition([]) == (toT([]), toT([]))


@memory_leak_decorator()
def test_partition_normal1():
    assert partition([T(0), T(1), T(2)]) == (toT([0]), toT([1, 2]))


@memory_leak_decorator()
def test_partition_normal2():
    assert partition([T(3), T(1), T(0)]) == (toT([0]), toT([3, 1]))


@memory_leak_decorator()
def test_partition_normal3():
    assert partition([T(0), T(0), T(0)]) == (toT([0, 0, 0]), [])


@memory_leak_decorator()
def test_partition_normal4():
    assert partition([T(1), T(1), T(1)]) == ([], toT([1, 1, 1]))


@memory_leak_decorator()
def test_partition_normal5():
    # using a generator
    assert partition((i for i in [T(0), T(1)])) == ([T(0)], [T(1)])


@memory_leak_decorator()
def test_partition_normal6():
    # pred=None is identical to no pred
    assert partition([T(0), T(1), T(2)], None) == (toT([0]), toT([1, 2]))


@memory_leak_decorator()
def test_partition_pred1():
    assert partition([T(0), T(1), T(2)],
                     lambda x: x.value > 1) == (toT([0, 1]), toT([2]))


@memory_leak_decorator()
def test_partition_pred2():
    assert partition([T(0), T(1), T(2)],
                     lambda x: x.value < 1) == (toT([1, 2]), toT([0]))


@memory_leak_decorator(collect=True)
def test_partition_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        partition(_hf.FailIter())


@memory_leak_decorator(collect=True)
def test_partition_failure2():
    with pytest.raises(TypeError):
        partition([T(1), T('a')], lambda x: x.value + 3)


@memory_leak_decorator(collect=True)
def test_partition_failure3():
    with pytest.raises(TypeError):
        partition([T(1), T('a')], lambda x: x.value - 1)


@memory_leak_decorator(collect=True)
def test_partition_failure4():
    with pytest.raises(TypeError):
        partition([T(1), T('a')], lambda x: x.value + 'a')


@memory_leak_decorator(collect=True)
def test_partition_failure5():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        partition(_hf.FailNext(), bool)


@memory_leak_decorator(collect=True)
def test_partition_failure6():
    # too few arguments
    with pytest.raises(TypeError):
        partition()


@memory_leak_decorator(collect=True)
def test_partition_failure7():
    # object has no boolean interpretation
    class NoBool(T):
        def __bool__(self):
            raise ValueError('bad class')
        __nonzero__ = __bool__

    with pytest.raises(ValueError, match='bad class'):
        partition([NoBool(10)])


@memory_leak_decorator(collect=True, offset=1)
def test_partition_failure8():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        partition(_hf.CacheNext(1))
