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


nth = iteration_utilities.nth


class T(object):
    def __init__(self, value):
        self.value = value

    def __bool__(self):
        return bool(self.value)

    def __nonzero__(self):
        return bool(self.value)

    def __pow__(self, other):
        return self.__class__(self.value ** other.value)


def test_nth_normal1():
    assert nth([1, 2, 3], 1) == 2

    def test():
        nth([T(1), T(2), T(3)], 1)
    assert not memory_leak(test)


def test_nth_normal2():
    assert nth(range(10), 2) == 2
    # TODO: Missing memory leak test


def test_nth_pred1():
    # With pred
    assert nth([0, 1, 2], 1, pred=bool) == 2

    def test():
        nth([T(0), T(1), T(2)], 1, pred=bool)
    assert not memory_leak(test)


def test_nth_pred2():
    assert nth([0, 1, 2], 1, pred=None) == 2

    def test():
        nth([T(0), T(1), T(2)], 1, pred=None)
    assert not memory_leak(test)


def test_nth_pred3():
    assert nth([0]*100 + [1], 0, pred=bool) == 1

    def test():
        nth([T(0)]*100 + [T(1)], 0, pred=bool)
    assert not memory_leak(test)


def test_nth_pred4():
    assert nth([[1], [1, 2], [1, 2]], 1, pred=lambda x: len(x) > 1) == [1, 2]

    def test():
        nth([[T(0)], [T(1), T(2)]]*2, 1, pred=lambda x: len(x) > 1)
    assert not memory_leak(test)


def test_nth_predtruthyretpred1():
    # pred with truthy/retpred
    assert nth([0, 1, 2, 3, 0], 1, pred=bool, truthy=False) == 0

    def test():
        nth([T(0), T(2), T(3), T(0)], 1,
            pred=bool, truthy=False)
    assert not memory_leak(test)


def test_nth_predtruthyretpred2():
    assert not nth([0, 1, 2, 3, 0], 1, pred=bool, truthy=False, retpred=True)

    def test():
        nth([T(0), T(1), T(2), T(3), T(0)], 1,
            pred=bool, truthy=False, retpred=True)
    assert not memory_leak(test)


def test_nth_predtruthyretpred3():
    assert nth([0, 1, 2, 3, 0], 1, pred=lambda x: x**2, truthy=False) == 0

    def test():
        nth([T(0), T(2), T(3), T(0)], 1,
            pred=lambda x: x**T(2), truthy=False)
    assert not memory_leak(test)


def test_nth_predtruthyretpred4():
    assert nth([0, 1, 2, 3, 0], 1,
               pred=lambda x: x**2, truthy=False, retpred=True) == 0

    def test():
        nth([T(0), T(2), T(3), T(0)], 0,
            pred=lambda x: x**T(2), truthy=False, retpred=True)
    assert not memory_leak(test)


def test_nth_predtruthyretpred5():
    assert nth([0, 1, 2, 3], 2, pred=bool) == 3

    def test():
        nth([T(0), T(2), T(3)], 1, pred=bool)
    assert not memory_leak(test)


def test_nth_predtruthyretpred6():
    assert nth([0, 1, 2, 3], 2, pred=bool, retpred=True)

    def test():
        nth([T(0), T(2), T(3)], 1, pred=bool, retpred=True)
    assert not memory_leak(test)


def test_nth_predtruthyretpred7():
    assert nth([0, 2, 3, 4], 2, pred=lambda x: x**2) == 4

    def test():
        nth([T(0), T(2), T(3)], 1, pred=lambda x: x**T(2))
    assert not memory_leak(test)


def test_nth_predtruthyretpred8():
    assert nth([0, 2, 3, 4], 2, pred=lambda x: x**2, retpred=True) == 16

    def test():
        nth([T(0), T(2), T(3)], 1,
            pred=lambda x: x**T(2), retpred=True)
    assert not memory_leak(test)


def test_nth_default1():
    # With default
    assert nth([], 2, default=None) is None

    def test():
        nth([], 2, default=None) is None
    assert not memory_leak(test)


def test_nth_default2():
    assert nth([0, 0, 0], 1, default=None, pred=bool) is None

    def test():
        nth([T(0), T(0), T(0)], 1, default=None, pred=bool) is None
    assert not memory_leak(test)


def test_nth_failures1():
    # failures
    with pytest.raises(TypeError):
        nth(100, 10)

    def test():
        with pytest_raises(TypeError):
            nth(T(100), 10)
    assert not memory_leak(test)


def test_nth_failures2():
    with pytest.raises(IndexError):
        nth([], 10)

    def test():
        with pytest_raises(IndexError):
            nth([], 10)
    assert not memory_leak(test)


def test_nth_failures3():
    with pytest.raises(IndexError):
        nth([0], 1, pred=bool)

    def test():
        with pytest_raises(IndexError):
            nth([T(0)], T(1))
    assert not memory_leak(test)


def test_nth_failures4():
    with pytest.raises(TypeError):
        nth(['a', 'b'], 1, pred=abs)

    def test():
        with pytest_raises(TypeError):
            nth([T('a'), T('b')], 1, pred=lambda x: abs(x.value))
    assert not memory_leak(test)
