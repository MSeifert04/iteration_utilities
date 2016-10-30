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


all_monotone = iteration_utilities.all_monotone


def test_all_monotone_empty1():
    assert all_monotone([])

    def test():
        all_monotone([])
    assert not memory_leak(test)


def test_all_monotone_normal1():
    assert all_monotone([1, 1, 1])

    def test():
        all_monotone([T(1), T(1), T(1)])
    assert not memory_leak(test)


def test_all_monotone_normal2():
    assert not all_monotone([1, 1, 1], strict=True)

    def test():
        all_monotone([T(1), T(1), T(1)], strict=True)
    assert not memory_leak(test)


def test_all_monotone_normal3():
    assert all_monotone([1, 2, 3])

    def test():
        all_monotone([T(1), T(2), T(3)])
    assert not memory_leak(test)


def test_all_monotone_normal4():
    assert all_monotone([1, 2, 3], strict=True)

    def test():
        all_monotone([T(1), T(2), T(3)], strict=True)
    assert not memory_leak(test)


def test_all_monotone_normal5():
    assert all_monotone([1, 1, 1], decreasing=True)

    def test():
        all_monotone([T(1), T(1), T(1)], decreasing=True)
    assert not memory_leak(test)


def test_all_monotone_normal6():
    assert not all_monotone([1, 1, 1], decreasing=True, strict=True)

    def test():
        all_monotone([T(1), T(1), T(1)], decreasing=True, strict=True)
    assert not memory_leak(test)


def test_all_monotone_normal7():
    assert all_monotone([3, 2, 1], decreasing=True)

    def test():
        all_monotone([T(3), T(2), T(1)], decreasing=True)
    assert not memory_leak(test)


def test_all_monotone_normal8():
    assert all_monotone([3, 2, 1], decreasing=True, strict=True)

    def test():
        all_monotone([T(3), T(2), T(1)], decreasing=True, strict=True)
    assert not memory_leak(test)


def test_all_monotone_failure1():
    # not iterable
    with pytest.raises(TypeError):
        all_monotone(1)

    def test():
        with pytest.raises(TypeError):
            all_monotone(T(1))
    assert not memory_leak(test)


def test_all_monotone_failure2():
    # comparison fail
    with pytest.raises(TypeError):
        all_monotone([T(1), T('a')])

    def test():
        with pytest.raises(TypeError):
            all_monotone([T(1), T('a')])
    assert not memory_leak(test)
