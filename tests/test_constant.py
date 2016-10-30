# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak


const = iteration_utilities.constant


class T(object):
    def __init__(self, value):
        self.value = value


def test_constant_normal1():
    one = const(1)
    assert one() == 1

    def test():
        const(T(1))()
    assert not memory_leak(test)


def test_constant_normal2():
    one = const(1)
    assert one(10, a=2) == 1

    def test():
        const(T(1))(10, a=2)
    assert not memory_leak(test)


def test_constant_pickle1():
    x = pickle.dumps(const(10))
    assert pickle.loads(x)() == 10

    def test():
        x = pickle.dumps(const(T(10)))
        pickle.loads(x)()
    memory_leak(test)
    assert not memory_leak(test)
