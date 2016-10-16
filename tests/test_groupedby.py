# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


groupedby = iteration_utilities.groupedby


class T(object):
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        return self.value == other.value

    def __hash__(self):
        return hash(self.value)


def test_groupedby_empty1():
    assert groupedby([], key=lambda x: x) == {}

    def test():
        groupedby([], key=lambda x: x)
    assert not memory_leak(test)


def test_groupedby_normal1():
    assert groupedby(['a', 'ab', 'abc'],
                     key=operator.itemgetter(0)) == {'a': ['a', 'ab', 'abc']}

    def test():
        groupedby([T('a'), T('ab'), T('abc')],
                  key=lambda x: T(x.value[0]))
    assert not memory_leak(test)


def test_groupedby_normal2():
    assert groupedby(['a', 'ba', 'ab', 'abc', 'b'],
                     key=operator.itemgetter(0)) == {'a': ['a', 'ab', 'abc'],
                                                     'b': ['ba', 'b']}

    def test():
        groupedby([T('a'), T('ba'), T('ab'), T('abc'), T('b')],
                  key=lambda x: T(x.value[0]))
    assert not memory_leak(test)


def test_groupedby_normal3():
    assert groupedby(['a', 'ba', 'ab', 'abc', 'b'],
                    key=operator.itemgetter(0),
                    keepkey=len) == {'a': [1, 2, 3], 'b': [2, 1]}

    def test():
        groupedby([T('a'), T('ba'), T('ab'), T('abc'), T('b')],
                  key=lambda x: T(x.value[0]),
                  keepkey=lambda x: T(len(x.value)))
    assert not memory_leak(test)


def test_groupedby_failure1():
    # not iterable
    with pytest.raises(TypeError):
        groupedby(1, key=len)

    def test():
        with pytest_raises(TypeError):
            groupedby(T(1), key=lambda x: T(len(x.value)))
    assert not memory_leak(test)


def test_groupedby_failure2():
    # key func fails
    with pytest.raises(TypeError):
        groupedby([1, 2, 3], key=lambda x: x + 'a')

    def test():
        with pytest_raises(TypeError):
            groupedby([T(1), T(2), T(3)], key=lambda x: T(x.value + 'a'))
    assert not memory_leak(test)


def test_groupedby_failure3():
    # keepkey func fails
    with pytest.raises(TypeError):
        groupedby([1, 2, 3], key=lambda x: x, keepkey=lambda x: x + 'a')

    def test():
        with pytest_raises(TypeError):
            groupedby([T(1), T(2), T(3)],
                      key=lambda x: x, keepkey=lambda x: T(x.value + 'a'))
    assert not memory_leak(test)


def test_groupedby_failure4():
    # unhashable
    with pytest.raises(TypeError):
        groupedby([{'a': 10}], key=lambda x: x)

    def test():
        with pytest_raises(TypeError):
            groupedby([{T('a'): T(10)}], key=lambda x: x)
    assert not memory_leak(test)
