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


compose = iteration_utilities.compose


class T(object):
    def __init__(self, value):
        self.value = value

    def __add__(self, other):
        return self.__class__(self.value + other)

    def __mul__(self, other):
        return self.__class__(self.value * other)

    def __rtruediv__(self, other):
        return self.__class__(other / self.value)

    def __pow__(self, other):
        return self.__class__(self.value ** other)


def test_compose_normal1():
    double_increment = compose(lambda x: x*2, lambda x: x+1)
    assert double_increment(10) == 21
    assert double_increment(2.) == 5

    def test():
        compose(lambda x: x*2, lambda x: x+1)(T(10))
        compose(lambda x: x*2, lambda x: x+1)(T(2))
    assert not memory_leak(test)


def test_compose_reverse1():
    double_increment = compose(lambda x: x*2, lambda x: x+1, reverse=True)
    assert double_increment(10) == 22
    assert double_increment(2.) == 6

    def test():
        compose(lambda x: x*2, lambda x: x+1, reverse=True)(T(10))
        compose(lambda x: x*2, lambda x: x+1, reverse=True)(T(2))
    assert not memory_leak(test)


def test_compose_all1():
    double_increment = compose(lambda x: x*2, lambda x: x+1, all=True)
    assert double_increment(10) == (20, 11)
    assert double_increment(2.) == (4, 3)

    def test():
        compose(lambda x: x*2, lambda x: x+1, all=True)(T(10))
        compose(lambda x: x*2, lambda x: x+1, all=True)(T(2))
    assert not memory_leak(test)


def test_compose_failure1():
    with pytest.raises(TypeError):  # at least one func must be present
        compose()

    def test():
        with pytest_raises(TypeError):  # at least one func must be present
            compose()
    assert not memory_leak(test)


def test_compose_failure2():
    with pytest.raises(TypeError):  # kwarg not accepted
        compose(lambda x: x+1, invalidkwarg=lambda x: x*2)

    def test():
        with pytest_raises(TypeError):  # kwarg not accepted
            compose(lambda x: x+1, invalidkwarg=lambda x: x*2)
    assert not memory_leak(test)


def test_compose_failure3():
    with pytest.raises(TypeError):  # func fails
        compose(lambda x: x+1)('a')

    def test():
        with pytest_raises(TypeError):  # func fails
            compose(lambda x: x+1)(T('a'))
    assert not memory_leak(test)


def test_compose_failure4():
    with pytest.raises(TypeError):  # second func fails
        compose(lambda x: x*2, lambda x: x+1)('a')

    def test():
        with pytest_raises(TypeError):  # second func fails
            compose(lambda x: x*2, lambda x: x+1)(T('a'))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_compose_pickle1():
    cmp = compose(iteration_utilities.square, iteration_utilities.reciprocal)
    x = pickle.dumps(cmp)
    assert pickle.loads(x)(10) == 1/100
    assert pickle.loads(x)(2) == 1/4

    def test():
        cmp = compose(iteration_utilities.square,
                      iteration_utilities.reciprocal)
        x = pickle.dumps(cmp)
        pickle.loads(x)(T(10))
        pickle.loads(x)(T(2))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_compose_pickle2():
    cmp = compose(iteration_utilities.square, iteration_utilities.double,
                  reverse=True)
    x = pickle.dumps(cmp)
    assert pickle.loads(x)(10) == 400
    assert pickle.loads(x)(3) == 36

    def test():
        cmp = compose(iteration_utilities.square, iteration_utilities.double,
                      reverse=True)
        x = pickle.dumps(cmp)
        pickle.loads(x)(T(10))
        pickle.loads(x)(T(2))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_compose_pickle3():
    cmp = compose(iteration_utilities.square, iteration_utilities.double,
                  all=True)
    x = pickle.dumps(cmp)
    assert pickle.loads(x)(10) == (100, 20)
    assert pickle.loads(x)(3) == (9, 6)

    def test():
        cmp = compose(iteration_utilities.square, iteration_utilities.double,
                      all=True)
        x = pickle.dumps(cmp)
        pickle.loads(x)(T(10))
        pickle.loads(x)(T(2))
    assert not memory_leak(test)
