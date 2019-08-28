# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities


complement = iteration_utilities.complement


def test_complement_repr1():
    x = complement(int)
    r = repr(x)
    assert 'complement' in r
    assert 'int' in r


def test_complement_attributes1():
    x = complement(int)
    assert x.func is int


def test_complement_normal1():
    assert not complement(lambda x: x is True)(True)


def test_complement_normal2():
    assert complement(lambda x: x is True)(False)


def test_complement_normal3():
    assert complement(lambda x: x is False)(True)


def test_complement_normal4():
    assert not complement(lambda x: x is False)(False)


def test_complement_normal5():
    assert not complement(iteration_utilities.is_None)(None)


def test_complement_normal6():
    assert complement(iteration_utilities.is_None)(False)


def test_complement_normal7():
    assert complement(iteration_utilities.is_None)(True)


def test_complement_failure1():
    # Function raises an Exception
    def failingfunction(x):
        raise ValueError('bad function')

    with pytest.raises(ValueError, match='bad function'):
        complement(failingfunction)(1)


def test_complement_failure2():
    # Function raturns an object that cannot be interpreted as boolean
    class NoBool(object):
        def __bool__(self):
            raise ValueError('bad class')
        __nonzero__ = __bool__

    def failingfunction(x):
        return NoBool()

    with pytest.raises(ValueError, match='bad class'):
        complement(failingfunction)(1)


def test_complement_failure3():
    # Too many arguments
    with pytest.raises(TypeError):
        complement(bool, int)


def test_complement_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        complement()


def test_complement_pickle1():
    x = pickle.dumps(complement(iteration_utilities.is_None))
    assert pickle.loads(x)(False)
    assert pickle.loads(x)(True)
    assert not pickle.loads(x)(None)
