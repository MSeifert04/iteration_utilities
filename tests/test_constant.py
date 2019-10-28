# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

from iteration_utilities import constant

from helper_cls import T


def test_constant_repr1():
    x = constant(2)
    r = repr(x)
    assert 'constant' in r
    assert '2' in r


def test_constant_attributes1():
    x = constant(T(2))
    assert x.item == T(2)


def test_constant_normal1():
    one = constant(T(1))
    assert one() == T(1)


def test_constant_normal2():
    one = constant(T(1))
    assert one(10, a=2) == T(1)


def test_constant_failure1():
    # Too few arguments
    with pytest.raises(TypeError):
        constant()


def test_constant_pickle1(protocol):
    x = pickle.dumps(constant(T(10)), protocol=protocol)
    assert pickle.loads(x)() == T(10)
