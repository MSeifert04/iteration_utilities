# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


partition = iteration_utilities.partition


class T(object):
    def __init__(self, value):
        self.value = value

    def __bool__(self):
        return bool(self.value)

    def __nonzero__(self):
        return bool(self.value)


def test_partition_empty1():
    assert partition([]) == ([], [])

    def test():
        partition([])
    assert not memory_leak(test)


def test_partition_normal1():
    assert partition([0, 1, 2]) == ([0], [1, 2])

    def test():
        partition([T(0), T(1), T(2)])
    assert not memory_leak(test)


def test_partition_normal2():
    assert partition([3, 1, 0]) == ([0], [3, 1])

    def test():
        partition([T(3), T(1), T(0)])
    assert not memory_leak(test)


def test_partition_normal3():
    assert partition([0, 0, 0]) == ([0, 0, 0], [])

    def test():
        partition([T(0), T(0), T(0)])
    assert not memory_leak(test)


def test_partition_normal4():
    assert partition([1, 1, 1]) == ([], [1, 1, 1])

    def test():
        partition([T(1), T(1), T(1)])
    assert not memory_leak(test)


def test_partition_pred1():
    assert partition([0, 1, 2], lambda x: x > 1) == ([0, 1], [2])

    def test():
        partition([T(0), T(1), T(2)], lambda x: x.value > 1)
    assert not memory_leak(test)


def test_partition_pred2():
    assert partition([0, 1, 2], lambda x: x < 1) == ([1, 2], [0])

    def test():
        partition([T(0), T(1), T(2)], lambda x: x.value < 1)
    assert not memory_leak(test)


def test_partition_failure1():
    # not-iterable
    with pytest.raises(TypeError):
        partition(10)

    def test():
        with pytest_raises(TypeError):
            partition(T(10))
    assert not memory_leak(test)


def test_partition_failure2():
    with pytest.raises(TypeError):
        partition([1, 2, 3, 4, 'a'], lambda x: x + 3)

    def test():
        with pytest_raises(TypeError):
            partition([T(1), T('a')], lambda x: x.value + 3)
    assert not memory_leak(test)


def test_partition_failure3():

    with pytest.raises(TypeError):
        partition([1, 2, 3, 4, 'a'], lambda x: x - 3)

    def test():
        with pytest_raises(TypeError):
            partition([T(1), T('a')], lambda x: x.value - 1)
    assert not memory_leak(test)


def test_partition_failure4():
    with pytest.raises(TypeError):
        partition([1, 2, 3, 4, 'a'], lambda x: x + 'a')

    def test():
        with pytest_raises(TypeError):
            partition([T(1), T('a')], lambda x: x.value + 'a')
    assert not memory_leak(test)
