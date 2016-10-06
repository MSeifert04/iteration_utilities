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


one = iteration_utilities.one


class T(object):
    def __init__(self, value):
        self.value = value


def test_one_normal1():
    assert one([0]) == 0

    def test():
        one([T(0)])
    assert not memory_leak(test)


def test_one_normal2():
    assert one('a') == 'a'

    def test():
        one('a')
    assert not memory_leak(test)


def test_one_normal3():
    assert one({'o': 10}) == 'o'

    def test():
        one({T(0): T(10)})
    assert not memory_leak(test)


def test_one_failure1():
    with pytest.raises(TypeError):
        one(10)

    def test():
        with pytest_raises(TypeError):
            one(T(0))
    assert not memory_leak(test)


def test_one_failure2():
    # empty iterable
    with pytest.raises(ValueError):
        one([])

    def test():
        with pytest_raises(ValueError):
            one([])
    assert not memory_leak(test)


def test_one_failure3():
    # more than 1 element
    with pytest.raises(ValueError):
        one([1, 2])

    def test():
        with pytest_raises(ValueError):
            one([T(1), T(2)])
    assert not memory_leak(test)
