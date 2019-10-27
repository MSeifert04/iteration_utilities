# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import all_distinct

import helper_funcs as _hf
from helper_cls import T


def test_alldistinct_empty1():
    assert all_distinct([])


def test_alldistinct_normal1():
    assert all_distinct([T(1), T(2), T(3)])


def test_alldistinct_normal2():
    assert not all_distinct([T(1), T(1), T(1)])


def test_alldistinct_normal3():
    # generator
    assert all_distinct((i for i in [T(1), T(2), T(3)]))


def test_alldistinct_unhashable1():
    assert all_distinct([{T('a'): T(1)}, {T('a'): T(2)}])


def test_alldistinct_unhashable2():
    assert not all_distinct([{T('a'): T(1)}, {T('a'): T(1)}])


def test_alldistinct_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        all_distinct(_hf.FailIter())


def test_alldistinct_failure2():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        all_distinct(_hf.FailNext())


def test_alldistinct_failure3():
    # Failure when comparing the object to the objects in the list
    with pytest.raises(_hf.FailEqNoHash.EXC_TYP, match=_hf.FailEqNoHash.EXC_MSG):
        all_distinct([[T(1)], _hf.FailEqNoHash()])


def test_alldistinct_failure4():
    # Failure (no TypeError) when trying to hash the value
    with pytest.raises(_hf.FailHash.EXC_TYP, match=_hf.FailHash.EXC_MSG):
        all_distinct([T(1), _hf.FailHash()])


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_alldistinct_failure5():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        all_distinct(_hf.CacheNext(1))
