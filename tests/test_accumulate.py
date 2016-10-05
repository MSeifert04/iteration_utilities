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


class Test(object):
    def __init__(self, value):
        self.value = value

    def __add__(self, other):
        return self.__class__(self.value + other.value)

    def __mul__(self, other):
        return self.__class__(self.value * other.value)


accumulate = iteration_utilities.accumulate


def test_accumulate_empty1():
    assert list(accumulate([])) == []

    def test():
        list(accumulate([]))
    assert not memory_leak(test)


def test_accumulate_normal1():
    assert list(accumulate([1, 2, 3])) == [1, 3, 6]

    def test():
        list(accumulate([Test(1), Test(2), Test(3)]))
    assert not memory_leak(test)


def test_accumulate_normal2():
    assert list(accumulate(None, [])) == []

    def test():
        list(accumulate(None, []))
    assert not memory_leak(test)


def test_accumulate_normal3():
    assert list(accumulate(None, [1, 2, 3, 4])) == [1, 3, 6, 10]

    def test():
        list(accumulate(None, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test)


def test_accumulate_binop1():
    assert list(accumulate(operator.add, [1, 2, 3, 4])) == [1, 3, 6, 10]

    def test():
        list(accumulate(operator.add, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test)


def test_accumulate_binop2():
    assert list(accumulate(operator.mul, [1, 2, 3, 4])) == [1, 2, 6, 24]

    def test():
        list(accumulate(operator.mul, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test)


def test_accumulate_initial1():
    assert list(accumulate(None, [1, 2, 3], 10)) == [11, 13, 16]

    def test():
        list(accumulate(operator.add, [Test(1), Test(2), Test(3)], Test(10)))
    assert not memory_leak(test)


def test_accumulate_failure1():
    with pytest.raises(TypeError):
        list(accumulate(None, [1, 2, 3], 'a'))

    def test():
        with pytest_raises(TypeError):
            list(accumulate(operator.add,
                            [Test(1), Test(2), Test(3)],
                            Test('a')))
    assert not memory_leak(test)


def test_accumulate_failure2():
    with pytest.raises(TypeError):
        list(accumulate(operator.add, [1, 2, 3], 'a'))

    def test():
        with pytest_raises(TypeError):
            list(accumulate(None,
                            [Test(1), Test(2), Test(3)],
                            Test('a')))
    assert not memory_leak(test)


def test_accumulate_failure3():
    with pytest.raises(TypeError):
        list(accumulate(['a', 2, 3]))

    def test():
        with pytest_raises(TypeError):
            list(accumulate([Test('a'), Test(2), Test(3)]))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_accumulate_pickle1():
    acc = accumulate([1, 2, 3, 4])
    assert next(acc) == 1
    x = pickle.dumps(acc)
    assert list(pickle.loads(x)) == [3, 6, 10]

    def test():
        acc = accumulate([Test(1), Test(2), Test(3), Test(4)])
        next(acc)
        x = pickle.dumps(acc)
        list(pickle.loads(x))
    assert not memory_leak(test)
