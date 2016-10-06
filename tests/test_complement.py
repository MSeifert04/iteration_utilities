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


complement = iteration_utilities.complement


def test_complement_normal1():
    assert not complement(lambda x: x is True)(True)

    def test():
        complement(lambda x: x is True)(True)
    assert not memory_leak(test)


def test_complement_normal2():
    assert complement(lambda x: x is True)(False)

    def test():
        complement(lambda x: x is True)(False)
    assert not memory_leak(test)


def test_complement_normal3():
    assert complement(lambda x: x is False)(True)

    def test():
        complement(lambda x: x is False)(True)
    assert not memory_leak(test)


def test_complement_normal4():
    assert not complement(lambda x: x is False)(False)

    def test():
        complement(lambda x: x is False)(False)
    assert not memory_leak(test)


def test_complement_normal5():
    assert not complement(iteration_utilities.is_None)(None)

    def test():
        complement(iteration_utilities.is_None)(None)
    assert not memory_leak(test)


def test_complement_normal6():
    assert complement(iteration_utilities.is_None)(False)

    def test():
        complement(iteration_utilities.is_None)(False)
    assert not memory_leak(test)


def test_complement_normal7():
    assert complement(iteration_utilities.is_None)(True)

    def test():
        complement(iteration_utilities.is_None)(True)
    assert not memory_leak(test)
