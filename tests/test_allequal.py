# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import all_equal

import helper_funcs as _hf
from helper_cls import T


def test_all_equal_empty1():
    assert all_equal([])


def test_all_equal_normal1():
    assert all_equal([T(1), T(1), T(1)])


def test_all_equal_normal2():
    assert not all_equal([T(1), T(1), T(2)])


def test_all_equal_normal3():
    # generator
    assert all_equal(i for i in [T(1), T(1), T(1)])


def test_all_equal_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        all_equal(_hf.FailIter())


def test_all_equal_failure2():
    # comparison fail
    with pytest.raises(TypeError):
        all_equal([T(1), T('a')])


def test_all_equal_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        all_equal(_hf.FailNext())


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_all_equal_failure4():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        all_equal(_hf.CacheNext(1))
