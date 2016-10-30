# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_cls import T


all_distinct = iteration_utilities.all_distinct


# TODO: Missing empty test


def test_alldistinct_normal1():
    assert all_distinct([1, 2, 3])

    def test():
        all_distinct([T(1), T(2), T(3)])
    assert not memory_leak(test)


def test_alldistinct_normal2():
    assert not all_distinct([1, 1, 1])

    def test():
        all_distinct([T(1), T(1), T(1)])
    assert not memory_leak(test)


def test_alldistinct_unhashable1():
    assert all_distinct([{'a': 1}, {'a': 2}])

    def test():
        all_distinct([{T('a'): T(1)}, {T('a'): T(2)}])
    assert not memory_leak(test)


def test_alldistinct_unhashable2():
    assert not all_distinct([{'a': 1}, {'a': 1}])

    def test():
        all_distinct([{T('a'): T(1)}, {T('a'): T(1)}])
    assert not memory_leak(test)


def test_alldistinct_failure1():
    # iterable is not iterable
    with pytest.raises(TypeError):
        all_distinct(1)

    def test():
        with pytest.raises(TypeError):
            all_distinct(T(1))
    assert not memory_leak(test)
