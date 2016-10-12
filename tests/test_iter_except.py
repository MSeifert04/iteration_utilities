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


iter_except = iteration_utilities.iter_except


class T(object):
    def __init__(self, value):
        self.value = value

    def __hash__(self):
        return hash(self.value)

    def __eq__(self, other):
        return self.value == other.value


def test_iterexcept_normal1():
    assert list(iter_except(({'a': 10}).popitem, KeyError)) == [('a', 10)]

    def test():
        list(iter_except(({T('a'): T(10)}).popitem, KeyError))
    assert not memory_leak(test)


def test_iterexcept_first():
    d = {}

    def insert():
        d['a'] = 10

    assert list(iter_except(d.popitem, KeyError, insert)) == [None, ('a', 10)]

    def test():
        d = {}

        def insert():
            d[T('a')] = T(10)

        list(iter_except(d.popitem, KeyError, insert))

    assert not memory_leak(test)


def test_iterexcept_failure1():
    # wrong exception
    with pytest.raises(KeyError):
        list(iter_except(({'a': 10}).popitem, ValueError))

    def test():
        with pytest_raises(KeyError):
            list(iter_except(({T('a'): T(10)}).popitem, ValueError))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_intersperse_pickle1():
    # TODO: Implement some tests for pickle!
    pass
