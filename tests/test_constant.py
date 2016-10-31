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


const = iteration_utilities.constant


@memory_leak_decorator()
def test_constant_normal1():
    one = const(T(1))
    assert one() == T(1)


@memory_leak_decorator()
def test_constant_normal2():
    one = const(T(1))
    assert one(10, a=2) == T(1)


@memory_leak_decorator(offset=1)
def test_constant_pickle1():
    x = pickle.dumps(const(T(10)))
    assert pickle.loads(x)() == T(10)
