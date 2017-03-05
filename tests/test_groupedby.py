# Built-ins
from __future__ import absolute_import, division, print_function
import itertools
import operator

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT


groupedby = iteration_utilities.groupedby


if iteration_utilities.EQ_PY2:
    range = xrange
    filter = itertools.ifilter


@memory_leak_decorator()
def test_groupedby_empty1():
    assert groupedby([], key=lambda x: x) == {}


@memory_leak_decorator()
def test_groupedby_normal1():
    assert groupedby([T('a'), T('ab'), T('abc')], key=lambda x: x.value[0]
                     ) == {'a': toT(['a', 'ab', 'abc'])}


@memory_leak_decorator()
def test_groupedby_normal2():
    assert groupedby([T('a'), T('ba'), T('ab'), T('abc'), T('b')],
                     key=lambda x: x.value[0]
                     ) == {'a': toT(['a', 'ab', 'abc']),
                           'b': toT(['ba', 'b'])}


@memory_leak_decorator()
def test_groupedby_keep1():
    assert groupedby([T('a'), T('ba'), T('ab'), T('abc'), T('b')],
                     key=lambda x: x.value[0],
                     keep=len) == {'a': [1, 2, 3], 'b': [2, 1]}


@memory_leak_decorator()
def test_groupedby_reduce1():
    assert groupedby([(T('a'), T(1)), (T('a'), T(2)), (T('b'), T(5))],
                     key=operator.itemgetter(0),
                     keep=operator.itemgetter(1),
                     reduce=operator.add) == {T('a'): T(3), T('b'): T(5)}


@memory_leak_decorator()
def test_groupedby_reduce2():
    assert groupedby([(T('a'), T(1)), (T('a'), T(2)), (T('b'), T(5))],
                     key=operator.itemgetter(0),
                     reduce=lambda x, y: x + y[1],
                     reducestart=T(0)) == {T('a'): T(3), T('b'): T(5)}


@memory_leak_decorator()
def test_groupedby_reduce3():
    assert groupedby(map(T, range(500)), key=lambda x: T(x.value % 5),
                     reduce=operator.add,
                     reducestart=T(0))


@memory_leak_decorator(collect=True)
def test_groupedby_failure1():
    # not iterable
    with pytest.raises(TypeError):
        groupedby(T(1), key=len)


@memory_leak_decorator(collect=True)
def test_groupedby_failure2():
    # key func fails
    with pytest.raises(TypeError):
        groupedby([T(1), T(2), T(3)], key=lambda x: T(x.value + 'a'))


@memory_leak_decorator(collect=True)
def test_groupedby_failure3():
    # keep func fails
    with pytest.raises(TypeError):
        groupedby([T(1), T(2), T(3)], key=lambda x: x,
                  keep=lambda x: T(x.value + 'a'))


@memory_leak_decorator(collect=True)
def test_groupedby_failure4():
    # unhashable
    with pytest.raises(TypeError):
        groupedby([{T('a'): T(10)}], key=lambda x: x)


@memory_leak_decorator(collect=True)
def test_groupedby_failure5():
    # no reduce but reducestart
    with pytest.raises(TypeError):
        groupedby(toT(range(10)), lambda x: x, reducestart=T(0))


@memory_leak_decorator(collect=True)
def test_groupedby_failure6():
    # reduce function fails with reducestart
    with pytest.raises(TypeError):
        groupedby(map(T, range(10)), lambda x: x.value % 2 == 0,
                  reduce=operator.add, reducestart=T('a'))


@memory_leak_decorator(collect=True)
def test_groupedby_failure7():
    # reduce function fails
    with pytest.raises(TypeError):
        groupedby(map(T, [1, 2, 3, 4, 'a']), lambda x: True,
                  reduce=operator.add)


@memory_leak_decorator(collect=True)
def test_groupedby_failure8():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError):
        groupedby(filter(operator.eq, zip([T(1)], [T(1)])),
                  bool)
