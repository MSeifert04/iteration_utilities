# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


applyfunc = iteration_utilities.applyfunc
getitem = iteration_utilities.getitem


@memory_leak_decorator()
def test_applyfunc_normal1():
    assert list(getitem(applyfunc(lambda x: x**T(2), T(2)),
                        stop=3)) == [T(4), T(16), T(256)]


@memory_leak_decorator()
def test_applyfunc_normal2():
    assert list(getitem(applyfunc(lambda x: x, T(2)),
                        stop=3)) == [T(2), T(2), T(2)]


@memory_leak_decorator(collect=True)
def test_applyfunc_failure1():
    with pytest.raises(TypeError):
        list(getitem(applyfunc(lambda x: x**T(2), T('a')), stop=3))


@memory_leak_decorator(collect=True)
def test_applyfunc_failure2():
    # Too few arguments
    with pytest.raises(TypeError):
        applyfunc(bool)


@memory_leak_decorator(offset=1)
def test_applyfunc_pickle1():
    apf = applyfunc(iteration_utilities.square, T(2))
    assert next(apf) == T(4)
    x = pickle.dumps(apf)
    assert next(pickle.loads(x)) == T(16)
