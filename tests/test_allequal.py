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


all_equal = iteration_utilities.all_equal


class T(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        if type(self.value) != type(other.value):
            raise TypeError('simulated failure.')
        return self.value == other.value


def test_all_equal_empty1():
    assert all_equal([])

    def test():
        all_equal([])
    assert not memory_leak(test)


def test_all_equal_normal1():
    assert all_equal([1, 1, 1])

    def test():
        all_equal([T(1), T(1), T(1)])
    assert not memory_leak(test)


def test_all_equal_normal2():
    assert not all_equal([1, 1, 2])

    def test():
        all_equal([T(1), T(1), T(2)])
    assert not memory_leak(test)


def test_all_equal_failure1():
    # not iterable
    with pytest.raises(TypeError):
        all_equal(1)

    def test():
        with pytest_raises(TypeError):
            all_equal(T(1))
    assert not memory_leak(test)


def test_all_equal_failure2():
    # comparison fail
    with pytest.raises(TypeError):
        all_equal([T(1), T('a')])

    def test():
        with pytest_raises(TypeError):
            all_equal([T(1), T('a')])
    assert not memory_leak(test)
