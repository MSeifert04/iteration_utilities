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


quantify = iteration_utilities.quantify


class T(object):
    def __init__(self, value):
        self.value = value

    def __lt__(self, other):
        return self.value < other.value

    def __gt__(self, other):
        return self.value > other.value

    def __bool__(self):
        return bool(self.value)

    def __nonzero__(self):
        return bool(self.value)


def test_quantify_empty1():
    assert quantify([]) == 0

    def test():
        quantify([])
    assert not memory_leak(test)


def test_quantify_normal1():
    assert quantify([0, 0]) == 0

    def test():
        quantify([T(0), T(0)])
    assert not memory_leak(test)


def test_quantify_normal2():
    assert quantify([0, 0, 1]) == 1

    def test():
        quantify([T(0), T(0), T(1)])
    assert not memory_leak(test)


def test_quantify_normal3():
    assert quantify([0, 0, 1, 1], None) == 2

    def test():
        quantify([T(0), T(0), T(1), T(1)], None)
    assert not memory_leak(test)


def test_quantify_normal4():
    assert quantify([], lambda x: x) == 0

    def test():
        quantify([], iteration_utilities.return_first_positional_argument)
    assert not memory_leak(test)


def test_quantify_normal5():
    assert quantify([1, 2, 3], lambda x: x > 2) == 1

    def test():
        quantify([T(1), T(2), T(3)], lambda x: x > T(2))
    assert not memory_leak(test)


def test_quantify_normal6():
    assert quantify([1, 2, 3], lambda x: x < 3) == 2

    def test():
        quantify([T(1), T(2), T(3)], lambda x: x < T(3))
    assert not memory_leak(test)


def test_quantify_failure1():
    with pytest.raises(TypeError):
        quantify(1)

    def test():
        with pytest_raises(TypeError):
            quantify(T(1))
    assert not memory_leak(test)


def test_quantify_failure2():
    with pytest.raises(TypeError):
        quantify([1], 1)

    def test():
        with pytest_raises(TypeError):
            quantify([T(1)], T(1))
    assert not memory_leak(test)
