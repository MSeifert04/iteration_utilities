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


flip = iteration_utilities.flip


class FlipSubclass(flip):
    pass


@memory_leak_decorator()
def test_flip_repr1():
    x = flip(int)
    r = repr(x)
    assert 'flip' in r
    assert 'int' in r


@memory_leak_decorator()
def test_flip_double_flip1():
    x = flip(int)
    y = flip(x)
    # Simply returned the original function instead of flipping the
    # arguments twice.
    assert y is int


@memory_leak_decorator()
def test_flip_double_flip2():
    # A subclass should prevent the behaviour that it simply returns the
    # original function when flipped.
    assert FlipSubclass(flip(int)) is not int
    assert flip(FlipSubclass(int)) is not int


@memory_leak_decorator()
def test_flip_normal1():
    assert not flip(isinstance)(float, 10)


@memory_leak_decorator()
def test_flip_normal2():
    assert flip(isinstance)(int, 10)


@memory_leak_decorator()
def test_flip_args0():
    def func():
        return ()
    assert flip(func)() == ()


@memory_leak_decorator()
def test_flip_args1():
    def func(a):
        return (a, )
    assert flip(func)(T(10)) == (T(10), )


@memory_leak_decorator()
def test_flip_args2():
    def func(a, b):
        return a, b
    assert flip(func)(T(1), T(2)) == (T(2), T(1))


@memory_leak_decorator()
def test_flip_args3():
    def func(a, b, c):
        return a, b, c
    assert flip(func)(T(1), T(2), T(3)) == (T(3), T(2), T(1))


@memory_leak_decorator(collect=True)
def test_flip_failure1():
    with pytest.raises(TypeError):
        flip(isinstance)(10, float)


@memory_leak_decorator(collect=True)
def test_flip_failure2():
    # Too few arguments
    with pytest.raises(TypeError):
        flip()


@memory_leak_decorator(collect=True)
def test_flip_failure3():
    # Too many arguments
    with pytest.raises(TypeError):
        flip(isinstance, bool)


@memory_leak_decorator(offset=1)
def test_flip_pickle1():
    x = pickle.dumps(flip(isinstance))
    assert pickle.loads(x)(float, 10.)
    assert pickle.loads(x)(int, 10)
    assert not pickle.loads(x)(float, 10)
