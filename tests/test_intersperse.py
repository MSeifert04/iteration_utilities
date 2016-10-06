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


intersperse = iteration_utilities.intersperse


class Test(object):
    def __init__(self, value):
        self.value = value


def test_intersperse_empty1():
    assert list(intersperse([], 0)) == []

    def test():
        list(intersperse([], Test(0)))
    assert not memory_leak(test)


def test_intersperse_empty2():
    assert list(intersperse([1], 0)) == [1]

    def test():
        list(intersperse([Test(1)], Test(0)))
    assert not memory_leak(test)


def test_intersperse_normal1():
    assert list(intersperse([1, 2], 0)) == [1, 0, 2]

    def test():
        list(intersperse([Test(1), Test(2)], Test(0)))
    assert not memory_leak(test)


def test_intersperse_failure1():
    with pytest.raises(TypeError):
        intersperse(100, 0)

    def test():
        with pytest_raises(TypeError):
            intersperse(Test(100), Test(0))
    assert not memory_leak(test)
