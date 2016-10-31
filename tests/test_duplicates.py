# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT


duplicates = iteration_utilities.duplicates


@memory_leak_decorator()
def test_duplicates_empty1():
    assert list(duplicates([])) == []


@memory_leak_decorator()
def test_duplicates_normal1():
    assert list(duplicates([T(1), T(2), T(1)])) == [T(1)]


@memory_leak_decorator()
def test_duplicates_key1():
    assert list(duplicates([T(1), T(2), T(1)], abs)) == [T(1)]


@memory_leak_decorator()
def test_duplicates_key2():
    assert list(duplicates([T(1), T(1), T(-1)], abs)) == toT([1, -1])


@memory_leak_decorator()
def test_duplicates_unhashable1():
    assert list(duplicates([{T(1): T(1)}, {T(2): T(2)}, {T(1): T(1)}]
                           )) == [{T(1): T(1)}]


@memory_leak_decorator()
def test_duplicates_unhashable2():
    assert list(duplicates([[T(1)], [T(2)], [T(1)]])) == [[T(1)]]


@memory_leak_decorator()
def test_duplicates_unhashable3():
    assert list(duplicates([[T(1), T(1)], [T(1), T(2)],
                            [T(1), T(3)]], operator.itemgetter(0)
                           )) == [[T(1), T(2)], [T(1), T(3)]]


@memory_leak_decorator()
def test_duplicates_getter1():
    t = duplicates([T(1), T([0, 0]), T(3), T(1)])
    assert not t.seen
    assert t.key is None
    assert next(t) == T(1)
    assert T(1) in t.seen
    assert T(3) in t.seen
    assert T([0, 0]) in t.seen
    assert t.key is None


@memory_leak_decorator(collect=True)
def test_duplicates_failure1():
    with pytest.raises(TypeError):
        list(duplicates(T(10)))


@memory_leak_decorator(collect=True)
def test_duplicates_failure2():
    with pytest.raises(TypeError):
        list(duplicates([T(1), T(2), T(3), T('a')], abs))


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_duplicates_pickle1():
    dpl = duplicates([T(1), T(2), T(1), T(2)])
    assert next(dpl) == T(1)
    x = pickle.dumps(dpl)
    assert list(pickle.loads(x)) == [T(2)]
