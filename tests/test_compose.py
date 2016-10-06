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
        return self.__class__(self.value + other)


def test_compose_normal1():
    double_increment = compose(lambda x: x*2, lambda x: x+1)
    assert double_increment(10) == 21
    assert double_increment(2.) == 5

    def test():
        compose(lambda x: x*2, lambda x: x+1)(T(10))
        compose(lambda x: x*2, lambda x: x+1)(T(2))
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
