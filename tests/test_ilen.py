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


ilen = iteration_utilities.ilen


class T(object):
    def __init__(self, value):
        self.value = value


def test_ilen_empty1():
    assert ilen([]) == 0

    def test():
        ilen([])
    assert not memory_leak(test)


def test_ilen_normal1():
    assert ilen(range(10)) == 10

    def test():
        ilen(range(10))
    assert not memory_leak(test)


def test_ilen_normal2():
    assert ilen([1, 2, 3, 4, 5]) == 5

    def test():
        ilen([T(1), T(2), T(3), T(4), T(5)])
    assert not memory_leak(test)


def test_ilen_normal3():
    generator = (i for i in [1, 2, 3, 4, 5])
    assert ilen(generator) == 5
    assert ilen(generator) == 0

    def test():
        generator = (i for i in [T(1), T(2), T(3), T(4), T(5)])
        ilen(generator) == 5
        ilen(generator) == 0
    assert not memory_leak(test)


def test_ilen_failure1():
    with pytest.raises(TypeError):
        ilen(100)

    def test():
        with pytest_raises(TypeError):
            ilen(100)
    assert not memory_leak(test)
