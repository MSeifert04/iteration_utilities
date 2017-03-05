# Built-ins
from __future__ import absolute_import, division, print_function
from functools import partial
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT, failingTIterator


split = iteration_utilities.split


equalsthreeT = partial(operator.eq, T(3))


@memory_leak_decorator()
def test_split_empty1():
    assert list(split([], lambda x: False)) == []


@memory_leak_decorator()
def test_split_normal1():
    assert list(split([T(1), T(2), T(3)],
                      lambda x: x.value == 2)) == [[T(1)], [T(3)]]


@memory_leak_decorator()
def test_split_normal2():
    assert list(split([T(1), T(2), T(3)],
                      lambda x: x.value == 3)) == [toT([1, 2])]


@memory_leak_decorator()
def test_split_keep1():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 2,
                      keep=True)) == [[T(1)], [T(2)], [T(3)]]


@memory_leak_decorator()
def test_split_keep2():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 3,
                      keep=True)) == [[T(1), T(2)], [T(3)]]


@memory_leak_decorator()
def test_split_keep_before1():
    assert list(split([T(1), T(2), T(3), T(4)], lambda x: x.value == 3,
                      keep_before=True)) == [[T(1), T(2), T(3)], [T(4)]]


@memory_leak_decorator()
def test_split_keep_before2():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 3,
                      keep_before=True)) == [[T(1), T(2), T(3)]]


@memory_leak_decorator()
def test_split_keep_after1():
    assert list(split([T(1), T(2), T(3), T(4)], lambda x: x.value == 3,
                      keep_after=True)) == [[T(1), T(2)], [T(3), T(4)]]


@memory_leak_decorator()
def test_split_keep_after2():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 3,
                      keep_after=True)) == [[T(1), T(2)], [T(3)]]


@memory_leak_decorator()
def test_split_maxsplit1():
    assert list(split([T(1), T(2), T(3), T(4), T(5)],
                      lambda x: x.value % 2 == 0,
                      maxsplit=1)) == [[T(1)], [T(3), T(4), T(5)]]


@memory_leak_decorator()
def test_split_maxsplit2():
    assert list(split([T(1), T(2), T(3), T(4), T(5)],
                      lambda x: x.value % 2 == 0,
                      maxsplit=2)) == [[T(1)], [T(3)], [T(5)]]


@memory_leak_decorator()
def test_split_eq1():
    assert list(split([T(1), T(2), T(3), T(2), T(5)], T(2),
                      eq=True)) == [[T(1)], [T(3)], [T(5)]]


@memory_leak_decorator(collect=True)
def test_split_failure1():
    # not iterable
    with pytest.raises(TypeError):
        split(T(1), lambda x: False)


@memory_leak_decorator(collect=True)
def test_split_failure2():
    # func fails
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], lambda x: T(x.value + 'a')))


@memory_leak_decorator(collect=True)
def test_split_failure3():
    # cmp fails
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], T('a'), eq=True))


@memory_leak_decorator(collect=True)
def test_split_failure4():
    # more than one of the keeps is specified
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], T('a'), keep=True, keep_before=True))


@memory_leak_decorator(collect=True)
def test_split_failure5():
    # more than one of the keeps is specified
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], T('a'), keep=True, keep_after=True))


@memory_leak_decorator(collect=True)
def test_split_failure6():
    # more than one of the keeps is specified
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], T('a'),
                   keep_before=True, keep_after=True))


@memory_leak_decorator(collect=True)
def test_split_failure7():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(split(failingTIterator(), lambda x: False))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle1():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT)
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [[T(1), T(2)], [T(4), T(5)], [T(7), T(8)]]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle2():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT([4, 5]), toT([7, 8])]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle3():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT, keep=True)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT(i)
                                     for i in [[3], [4, 5], [3], [7, 8]]]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle4():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT, maxsplit=1)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT([4, 5, 3, 7, 8])]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle5():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, T(3), eq=True)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT([4, 5]), toT([7, 8])]
