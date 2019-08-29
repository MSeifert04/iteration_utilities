# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_cls import T


const = iteration_utilities.constant


def test_constant_repr1():
    x = const(2)
    r = repr(x)
    assert 'constant' in r
    assert '2' in r


def test_constant_attributes1():
    x = const(T(2))
    assert x.item == T(2)


def test_constant_normal1():
    one = const(T(1))
    assert one() == T(1)


def test_constant_normal2():
    one = const(T(1))
    assert one(10, a=2) == T(1)


def test_constant_failure1():
    # Too few arguments
    with pytest.raises(TypeError):
        const()


def test_constant_pickle1(protocol):
    x = pickle.dumps(const(T(10)), protocol=protocol)
    assert pickle.loads(x)() == T(10)
