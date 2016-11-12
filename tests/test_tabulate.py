# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT


tabulate = iteration_utilities.tabulate
getitem = iteration_utilities.getitem


class T2(T):
    def __add__(self, other):
        if isinstance(other, T):
            return self.__class__(self.value + other.value)
        return self.__class__(self.value + other)

    def __mul__(self, other):
        if isinstance(other, T):
            return self.__class__(self.value * other.value)
        return self.__class__(self.value * other)


@memory_leak_decorator()
def test_tabulate_normal1():
    assert list(getitem(tabulate(lambda x: x, T2(0)),
                        stop=5)) == [T2(0), T2(1), T2(2), T2(3), T2(4)]


@memory_leak_decorator()
def test_tabulate_normal2():
    assert list(getitem(tabulate(T), stop=5)) == [T(0), T(1), T(2), T(3), T(4)]


@memory_leak_decorator(collect=True)
def test_tabulate_failure1():
    with pytest.raises(TypeError):
        next(tabulate(iteration_utilities.square, T(0.5)))


@memory_leak_decorator(collect=True)
def test_tabulate_failure2():
    with pytest.raises(TypeError):
        next(tabulate(iteration_utilities.return_identity, T(0.5)))


@memory_leak_decorator(collect=True)
def test_tabulate_failure3():
    tab = tabulate(iteration_utilities.return_identity, T(0))
    # Fail once while incrementing, this will set cnt to NULL
    with pytest.raises(TypeError):
        next(tab)
    with pytest.raises(StopIteration):
        next(tab)


@memory_leak_decorator(offset=1)
def test_tabulate_pickle1():
    rr = tabulate(T)
    assert next(rr) == T(0)
    x = pickle.dumps(rr)
    assert next(pickle.loads(x)) == T(1)


@memory_leak_decorator(offset=1)
def test_tabulate_pickle2():
    rr = tabulate(T, 2)
    assert next(rr) == T(2)
    x = pickle.dumps(rr)
    assert next(pickle.loads(x)) == T(3)


@memory_leak_decorator(offset=1)
def test_tabulate_pickle3():
    rr = tabulate(T)
    x = pickle.dumps(rr)
    assert next(pickle.loads(x)) == T(0)
