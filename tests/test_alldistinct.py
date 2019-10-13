# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
from helper_cls import T


all_distinct = iteration_utilities.all_distinct


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
    with pytest.raises(_hf.FailIter.EXC_TYP, match= _hf.FailIter.EXC_MSG):
        all_distinct(_hf.FailIter())


def test_alldistinct_failure2():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        all_distinct(_hf.FailNext())


def test_alldistinct_failure3():
    # Failure when comparing the object to the objects in the list
    class NoHashNoEq():
        def __hash__(self):
            raise TypeError('cannot be hashed')

        def __eq__(self, other):
            raise ValueError('bad class')

    with pytest.raises(ValueError, match='bad class'):
        all_distinct([[T(1)], NoHashNoEq()])


def test_alldistinct_failure4():
    # Failure (no TypeError) when trying to hash the value
    class NoHash():
        def __hash__(self):
            raise ValueError('bad class')

    with pytest.raises(ValueError, match='bad class'):
        all_distinct([T(1), NoHash()])


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_alldistinct_failure5():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        all_distinct(_hf.CacheNext(1))
