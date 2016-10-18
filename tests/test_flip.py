# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


flip = iteration_utilities.flip


class T(object):
    def __init__(self, value):
        self.value = value


def test_flip_normal1():
    assert not flip(isinstance)(float, 10)

    def test():
        flip(isinstance)(float, 10)
    assert not memory_leak(test)


def test_flip_normal2():
    assert flip(isinstance)(int, 10)

    def test():
        flip(isinstance)(int, 10)
    assert not memory_leak(test)


def test_flip_args0():
    def func():
        return ()
    assert flip(func)() == ()

    def test():
        flip(func)()
    assert not memory_leak(test)


def test_flip_args1():
    def func(a):
        return (a, )
    assert flip(func)(10) == (10, )

    def test():
        flip(func)(T(10))
    assert not memory_leak(test)


def test_flip_args2():
    def func(a, b):
        return a, b
    assert flip(func)(10, 20) == (20, 10)

    def test():
        flip(func)(T(1), T(2))
    assert not memory_leak(test)


def test_flip_args3():
    def func(a, b, c):
        return a, b, c
    assert flip(func)(10, 20, 30) == (30, 20, 10)

    def test():
        flip(func)(T(1), T(2), T(3))
    assert not memory_leak(test)


def test_flip_failure1():
    with pytest.raises(TypeError):
        flip(isinstance)(10, float)

    def test():
        with pytest_raises(TypeError):
            flip(isinstance)(10, float)
    assert not memory_leak(test)


def test_flip_pickle1():
    x = pickle.dumps(flip(isinstance))
    assert pickle.loads(x)(float, 10.)
    assert pickle.loads(x)(int, 10)
    assert not pickle.loads(x)(float, 10)

    def test():
        x = pickle.dumps(flip(isinstance))
        pickle.loads(x)(float, 10.)
        pickle.loads(x)(int, 10)
        not pickle.loads(x)(float, 10)
    memory_leak(test)
    assert not memory_leak(test)
