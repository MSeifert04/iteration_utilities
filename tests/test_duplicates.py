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


duplicates = iteration_utilities.duplicates


class T(object):
    def __init__(self, value):
        self.value = value

    def __hash__(self):
        return hash(self.value)

    def __eq__(self, other):
        return self.value == other.value


def test_duplicates_empty1():
    assert list(duplicates([])) == []

    def test():
        list(duplicates([]))
    assert not memory_leak(test)


def test_duplicates_normal1():
    assert list(duplicates([1, 2, 1])) == [1]

    def test():
        list(duplicates([T(1), T(2), T(3)]))
    assert not memory_leak(test)


def test_duplicates_key1():
    assert list(duplicates([1, 2, 1], abs)) == [1]

    def test():
        list(duplicates([T(1), T(2), T(1)], lambda x: abs(x.value)))
    assert not memory_leak(test)


def test_duplicates_key2():
    assert list(duplicates([1, 1, -1], abs)) == [1, -1]

    def test():
        list(duplicates([T(1), T(1), T(-1)], lambda x: abs(x.value)))
    assert not memory_leak(test)


def test_duplicates_unhashable1():
    assert list(duplicates([{1: 1}, {2: 2}, {1: 1}])) == [{1: 1}]

    def test():
        list(duplicates([{T(1): T(1)}, {T(2): T(2)}, {T(1): T(1)}]))
    assert not memory_leak(test)


def test_duplicates_unhashable2():
    assert list(duplicates([[1], [2], [1]])) == [[1]]

    def test():
        list(duplicates([[T(1)], [T(2)], [T(1)]]))
    assert not memory_leak(test)


def test_duplicates_unhashable3():
    assert list(duplicates([[1, 1], [1, 2], [1, 3]],
                           operator.itemgetter(0))) == [[1, 2], [1, 3]]

    def test():
        list(duplicates([[T(1), T(1)], [T(1), T(2)],
                        [T(1), T(3)]], operator.itemgetter(0)))
    assert not memory_leak(test)


def test_duplicates_failure1():
    with pytest.raises(TypeError):
        list(duplicates(10))

    def test():
        with pytest_raises(TypeError):
            list(duplicates(T(10)))
    assert not memory_leak(test)


def test_duplicates_failure2():
    with pytest.raises(TypeError):
        list(duplicates([1, 2, 3, 'a'], abs))

    def test():
        with pytest_raises(TypeError):
            list(duplicates([T(1), T(2), T(3), T('a')],
                            lambda x: abs(x.value)))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_duplicates_pickle1():
    dpl = duplicates([1, 2, 1, 2])
    assert next(dpl) == 1
    x = pickle.dumps(dpl)
    assert list(pickle.loads(x)) == [2]

    def test():
        dpl = duplicates([T(1), T(2), T(1), T(2)])
        next(dpl)
        x = pickle.dumps(dpl)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)
