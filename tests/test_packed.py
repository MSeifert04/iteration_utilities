# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


packed = iteration_utilities.packed


@memory_leak_decorator()
def test_packed_repr1():
    x = packed(int)
    r = repr(x)
    assert 'packed' in r
    assert 'int' in r


@memory_leak_decorator()
def test_packed_normal1():
    eq = packed(operator.eq)
    assert eq((T(1), T(1)))
    assert not eq((T(1), T(2)))


@memory_leak_decorator()
def test_packed_normal2():
    eq = packed(operator.eq)
    assert eq([T(1), T(1)])
    assert not eq([T(1), T(2)])


@memory_leak_decorator(collect=True)
def test_packed_failure1():
    # too many arguments when creating an instance
    with pytest.raises(TypeError):
        packed(1, 2)


@memory_leak_decorator(collect=True)
def test_packed_failure2():
    # too few arguments when creating an instance
    with pytest.raises(TypeError):
        packed()


@memory_leak_decorator(collect=True)
def test_packed_failure3():
    # too few arguments when calling the instance
    with pytest.raises(TypeError):
        packed(operator.eq)()


@memory_leak_decorator(collect=True)
def test_packed_failure4():
    # too many arguments when calling the instance
    with pytest.raises(TypeError):
        packed(operator.eq)(1, 2)


@memory_leak_decorator(collect=True)
def test_packed_failure5():
    # arguments for calling the instance are not convertible to tuple
    with pytest.raises(TypeError):
        packed(operator.eq)(1)


@memory_leak_decorator(collect=True)
def test_packed_failure6():
    # function raised an Exception
    def failingfunc(a, b):
        raise ValueError('bad func!')

    with pytest.raises(ValueError) as exc:
        packed(failingfunc)((1, 2))
    assert 'bad func!' in str(exc)


@memory_leak_decorator(offset=1)
def test_packed_pickle1():
    eq = packed(operator.eq)
    x = pickle.dumps(eq)
    assert pickle.loads(x)((T(1), T(1)))
    assert not pickle.loads(x)((T(1), T(2)))
