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


applyfunc = iteration_utilities.applyfunc
take = iteration_utilities.take


class T(object):
    def __init__(self, value):
        self.value = value

    def __pow__(self, other):
        return self.__class__(self.value**other.value)


def test_applyfunc_normal1():
    assert take(applyfunc(lambda x: x**2, 2), 3) == [4, 16, 256]

    def test():
        take(applyfunc(lambda x: x**T(2), T(2)), 3)
    assert not memory_leak(test)


def test_applyfunc_normal2():
    assert take(applyfunc(lambda x: x, 2), 3) == [2, 2, 2]

    def test():
        take(applyfunc(lambda x: x, T(2)), 3)
    assert not memory_leak(test)


def test_applyfunc_failure1():
    with pytest.raises(TypeError):
        take(applyfunc(lambda x: x**2, 'a'), 3)

    def test():
        with pytest_raises(TypeError):
            take(applyfunc(lambda x: x**T(2), T('a')), 3)
    assert not memory_leak(test)


def test_applyfunc_pickle1():
    apf = applyfunc(iteration_utilities.square, 2)
    assert next(apf) == 4
    x = pickle.dumps(apf)
    assert next(pickle.loads(x)) == 16

    def test():
        apf = applyfunc(lambda x: x**T(2), T(2))
        next(apf)
        x = pickle.dumps(apf)
        next(pickle.loads(x))
    assert not memory_leak(test)
