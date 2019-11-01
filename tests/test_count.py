# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

import iteration_utilities
from iteration_utilities import count_items

import helper_funcs as _hf
from helper_cls import T


def test_count_empty1():
    assert count_items([]) == 0


def test_count_normal1():
    assert count_items([T(0), T(0)]) == 2


def test_count_normal2():
    assert count_items([T(0), T(0), T(1)], bool) == 1


def test_count_normal3():
    # None as pred is equal to not giving any predicate
    assert count_items([T(0), T(0), T(1), T(1)], None) == 4


def test_count_normal4():
    assert count_items([], iteration_utilities.return_identity) == 0


def test_count_normal5():
    assert count_items([T(1), T(2), T(3)], lambda x: x > T(2)) == 1


def test_count_normal6():
    assert count_items([T(1), T(2), T(3)], lambda x: x < T(3)) == 2


def test_count_normal7():
    assert count_items([T(3), T(1), T(2), T(3), T(3)], T(3), True) == 3


def test_count_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        count_items(_hf.FailIter())


def test_count_failure2():
    with pytest.raises(TypeError):
        count_items([T(1)], T(1))


def test_count_failure3():
    # Regression test when accessing the next item of the iterable resulted
    # in an Exception. For example when the iterable was a filter and the
    # filter function threw an exception.
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        count_items(_hf.FailNext())


def test_count_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        count_items()


def test_count_failure5():
    # eq True but no pred
    with pytest.raises(TypeError):
        count_items([T(0)], eq=True)


def test_count_failure6():
    # eq True but pred None (like not given)
    with pytest.raises(TypeError):
        count_items([T(0)], pred=None, eq=True)


def test_count_failure7():
    # function returns item without boolean interpretation
    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        count_items([T(0)], lambda x: _hf.FailBool())


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_count_failure8():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        count_items(_hf.CacheNext(1))
