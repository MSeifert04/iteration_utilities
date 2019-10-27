# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import all_isinstance

import helper_funcs as _hf
from helper_cls import T, toT


def test_allisinstance_empty1():
    assert all_isinstance([], T)


def test_allisinstance_normal1():
    assert all_isinstance(toT([1, 2, 3]), T)


def test_allisinstance_normal2():
    assert not all_isinstance(toT([1, 2, 3]) + [10], T)


def test_allisinstance_normal3():
    # using a generator (raises a StopIteration)
    assert all_isinstance((i for i in toT([1, 2, 3])), T)


def test_allisinstance_one_type():
    assert all_isinstance([1, 2, 3], int)


def test_allisinstance_multiple_types():
    assert all_isinstance([1, 2, 3, 4.5], (int, float))


def test_allisinstance_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        all_isinstance(_hf.FailIter(), T)


def test_allisinstance_failure2():
    # not enough arguments
    with pytest.raises(TypeError):
        all_isinstance([T(1)])


def test_allisinstance_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        all_isinstance(_hf.FailNext(), T)


def test_allisinstance_failure4():
    # Test failing isinstance operation
    with pytest.raises(_hf.FailingIsinstanceClass.EXC_TYP,
                       match=_hf.FailingIsinstanceClass.EXC_MSG):
        all_isinstance(toT([1, 2, 3]), _hf.FailingIsinstanceClass)


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_allisinstance_failure5():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        all_isinstance(_hf.CacheNext(1), int)
