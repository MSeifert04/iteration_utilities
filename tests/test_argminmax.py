# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


argmin = iteration_utilities.argmin
argmax = iteration_utilities.argmax


class T(object):
    def __init__(self, value):
        self.value = value

    def __gt__(self, other):
        return self.value > other.value

    def __lt__(self, other):
        return self.value < other.value

    def __abs__(self):
        return self.__class__(abs(self.value))


def test_argmax_normal1():
    # Just one test for argmax because the internals are identical to argmin
    assert argmax(0, 1, 2) == 2

    def test():
        argmax(T(0), T(1), T(2))
    assert not memory_leak(test)


def test_argmin_positional1():
    assert argmin(0, 1, 2) == 0

    def test():
        argmin(T(0), T(1), T(2))
    assert not memory_leak(test)


def test_argmin_positional2():
    assert argmin(3, 0, 1) == 1

    def test():
        argmin(T(3), T(0), T(1))
    assert not memory_leak(test)


def test_argmin_sequence1():
    assert argmin([3, 0, 1]) == 1

    def test():
        argmin([T(3), T(0), T(1)])
    assert not memory_leak(test)


def test_argmin_generator1():
    assert argmin((i for i in [5, 4, 3, 2])) == 3

    def test():
        argmin((T(i) for i in [5, 4, 3, 2]))
    assert not memory_leak(test)


def test_argmin_key1():
    assert argmin([3, -2, 1], key=abs) == 2

    def test():
        argmin([T(3), T(0), T(1)], key=abs)
    assert not memory_leak(test)


def test_argmin_default1():
    assert argmin([], default=2) == 2

    def test():
        argmin([], default=2)
    assert not memory_leak(test)


def test_argmin_failure1():
    # default not possible if given multiple positional arguments
    with pytest.raises(TypeError):
        argmin(3, 0, 1, default=1)

    def test():
        with pytest_raises(TypeError):
            argmin(T(3), T(0), T(1), default=1)
    assert not memory_leak(test)


def test_argmin_failure2():
    # not integer default value
    with pytest.raises(TypeError):
        argmin([3, 0, 1], default=1.5, key=lambda x: x + 1)

    def test():
        with pytest_raises(TypeError):
            argmin([T(3), T(0), T(1)], default=1.5, key=lambda x: x + 1)
    assert not memory_leak(test)


def test_argmin_failure3():
    # unwanted kwarg
    with pytest.raises(TypeError):
        argmin([3, 0, 1], default=1, key=lambda x: x + 1, blub=10)

    def test():
        with pytest_raises(TypeError):
            argmin([T(3), T(0), T(1)],
                   default=1, key=lambda x: x + 1, blub=T(10))
    assert not memory_leak(test)


def test_argmin_failure4():
    # no args
    with pytest.raises(TypeError):
        argmin(key=lambda x: x + 1)

    def test():
        with pytest_raises(TypeError):
            argmin(key=lambda x: x + 1)
    assert not memory_leak(test)


def test_argmin_failure5():
    # empty sequence
    with pytest.raises(ValueError):
        argmin([], key=lambda x: x + 1)

    def test():
        with pytest_raises(ValueError):
            argmin([], key=lambda x: x + 1)
    assert not memory_leak(test)


def test_argmin_failure6():
    # cmp failed
    with pytest.raises(TypeError):
        argmin([1, 2, 'a'], key=lambda x: x)

    def test():
        with pytest_raises(TypeError):
            argmin([T(1), T(2), T('a')], key=lambda x: x)
    assert not memory_leak(test)


def test_argmin_failure7():
    # key failed
    with pytest.raises(TypeError):
        argmin([1, 2, 'a'], key=lambda x: x + 1)

    def test():
        with pytest_raises(TypeError):
            argmin([T(1), T(2), T('a')], key=lambda x: x + 1)
    assert not memory_leak(test)
