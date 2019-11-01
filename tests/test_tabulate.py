# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

import iteration_utilities
from iteration_utilities import tabulate, getitem

import helper_funcs as _hf
from helper_cls import T


class T2(T):
    def __add__(self, other):
        if isinstance(other, T):
            return self.__class__(self.value + other.value)
        return self.__class__(self.value + other)

    def __mul__(self, other):
        if isinstance(other, T):
            return self.__class__(self.value * other.value)
        return self.__class__(self.value * other)


def test_tabulate_normal1():
    assert list(getitem(tabulate(lambda x: x, T2(0)),
                        stop=5)) == [T2(0), T2(1), T2(2), T2(3), T2(4)]


def test_tabulate_normal2():
    assert list(getitem(tabulate(T), stop=5)) == [T(0), T(1), T(2), T(3), T(4)]


def test_tabulate_attributes1():
    it = tabulate(T)
    assert it.func is T
    assert it.current == 0

    next(it)

    assert it.current == 1


def test_tabulate_failure1():

    class T:
        def __init__(self, val):
            self.val = val

        def __truediv__(self, other):
            return self.__class__(self.val / other.val)

    # Function call fails
    with pytest.raises(ZeroDivisionError):
        next(tabulate(lambda x: T(1)/x, T(0)))


def test_tabulate_failure2():
    # incrementing with one fails
    with pytest.raises(TypeError):
        next(tabulate(iteration_utilities.return_identity, T(0.5)))


def test_tabulate_failure3():
    tab = tabulate(iteration_utilities.return_identity, T(0))
    # Fail once while incrementing, this will set cnt to NULL
    with pytest.raises(TypeError):
        next(tab)
    with pytest.raises(StopIteration):
        next(tab)


def test_tabulate_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        tabulate()


def test_tabulate_copy1():
    _hf.iterator_copy(tabulate(T))


def test_tabulate_pickle1(protocol):
    rr = tabulate(T)
    assert next(rr) == T(0)
    x = pickle.dumps(rr, protocol=protocol)
    assert next(pickle.loads(x)) == T(1)


def test_tabulate_pickle2(protocol):
    rr = tabulate(T, 2)
    assert next(rr) == T(2)
    x = pickle.dumps(rr, protocol=protocol)
    assert next(pickle.loads(x)) == T(3)


def test_tabulate_pickle3(protocol):
    rr = tabulate(T)
    x = pickle.dumps(rr, protocol=protocol)
    assert next(pickle.loads(x)) == T(0)
