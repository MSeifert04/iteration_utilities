# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator


complement = iteration_utilities.complement


@memory_leak_decorator()
def test_complement_normal1():
    assert not complement(lambda x: x is True)(True)


@memory_leak_decorator()
def test_complement_normal2():
    assert complement(lambda x: x is True)(False)


@memory_leak_decorator()
def test_complement_normal3():
    assert complement(lambda x: x is False)(True)


@memory_leak_decorator()
def test_complement_normal4():
    assert not complement(lambda x: x is False)(False)


@memory_leak_decorator()
def test_complement_normal5():
    assert not complement(iteration_utilities.is_None)(None)


@memory_leak_decorator()
def test_complement_normal6():
    assert complement(iteration_utilities.is_None)(False)


@memory_leak_decorator()
def test_complement_normal7():
    assert complement(iteration_utilities.is_None)(True)


@memory_leak_decorator(offset=1)
def test_complement_pickle1():
    x = pickle.dumps(complement(iteration_utilities.is_None))
    assert pickle.loads(x)(False)
    assert pickle.loads(x)(True)
    assert not pickle.loads(x)(None)
