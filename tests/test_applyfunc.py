# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_cls import T


applyfunc = iteration_utilities.applyfunc
getitem = iteration_utilities.getitem


def test_applyfunc_normal1():
    assert list(getitem(applyfunc(lambda x: x**2, 2), stop=3)) == [4, 16, 256]

    def test():
        list(getitem(applyfunc(lambda x: x**T(2), T(2)), stop=3))
    assert not memory_leak(test)


def test_applyfunc_normal2():
    assert list(getitem(applyfunc(lambda x: x, 2), stop=3)) == [2, 2, 2]

    def test():
        list(getitem(applyfunc(lambda x: x, T(2)), stop=3))
    assert not memory_leak(test)


def test_applyfunc_failure1():
    with pytest.raises(TypeError):
        list(getitem(applyfunc(lambda x: x**2, 'a'), stop=3))

    def test():
        with pytest.raises(TypeError):
            list(getitem(applyfunc(lambda x: x**T(2), T('a')), stop=3))
    assert not memory_leak(test)


def test_applyfunc_pickle1():
    apf = applyfunc(iteration_utilities.square, 2)
    assert next(apf) == 4
    x = pickle.dumps(apf)
    assert next(pickle.loads(x)) == 16

    def test():
        apf = applyfunc(iteration_utilities.square, T(2))
        next(apf)
        x = pickle.dumps(apf)
        next(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)
