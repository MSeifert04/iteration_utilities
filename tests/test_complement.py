# Licensed under Apache License Version 2.0 - see LICENSE

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
def test_complement_repr1():
    x = complement(int)
    r = repr(x)
    assert 'complement' in r
    assert 'int' in r


@memory_leak_decorator()
def test_complement_attributes1():
    x = complement(int)
    assert x.func is int


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


@memory_leak_decorator(collect=True)
def test_complement_failure1():
    # Function raises an Exception
    def failingfunction(x):
        raise ValueError('bad function')

    with pytest.raises(ValueError) as exc:
        complement(failingfunction)(1)
    assert 'bad function' in str(exc)


@memory_leak_decorator(collect=True)
def test_complement_failure2():
    # Function raturns an object that cannot be interpreted as boolean
    class NoBool(object):
        def __bool__(self):
            raise ValueError('bad class')
        __nonzero__ = __bool__

    def failingfunction(x):
        return NoBool()

    with pytest.raises(ValueError) as exc:
        complement(failingfunction)(1)
    assert 'bad class' in str(exc)


@memory_leak_decorator(collect=True)
def test_complement_failure3():
    # Too many arguments
    with pytest.raises(TypeError):
        complement(bool, int)


@memory_leak_decorator(collect=True)
def test_complement_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        complement()


@memory_leak_decorator(offset=1)
def test_complement_pickle1():
    x = pickle.dumps(complement(iteration_utilities.is_None))
    assert pickle.loads(x)(False)
    assert pickle.loads(x)(True)
    assert not pickle.loads(x)(None)
