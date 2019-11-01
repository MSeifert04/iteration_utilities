# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import all_monotone

import helper_funcs as _hf
from helper_cls import T


def test_all_monotone_empty1():
    assert all_monotone([])


def test_all_monotone_normal1():
    assert all_monotone([T(1), T(1), T(1)])


def test_all_monotone_normal2():
    assert not all_monotone([T(1), T(1), T(1)], strict=True)


def test_all_monotone_normal3():
    assert all_monotone([T(1), T(2), T(3)])


def test_all_monotone_normal4():
    assert all_monotone([T(1), T(2), T(3)], strict=True)


def test_all_monotone_normal5():
    assert all_monotone([T(1), T(1), T(1)], decreasing=True)


def test_all_monotone_normal6():
    assert not all_monotone([T(1), T(1), T(1)], decreasing=True, strict=True)


def test_all_monotone_normal7():
    assert all_monotone([T(3), T(2), T(1)], decreasing=True)


def test_all_monotone_normal8():
    assert all_monotone([T(3), T(2), T(1)], decreasing=True, strict=True)


def test_all_monotone_normal9():
    # generator
    assert all_monotone(i for i in [T(1), T(1), T(1)])


def test_all_monotone_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        all_monotone(_hf.FailIter())


def test_all_monotone_failure2():
    # comparison fail
    with pytest.raises(TypeError):
        all_monotone([T(1), T('a')])


def test_all_monotone_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        all_monotone(_hf.FailNext())


def test_all_monotone_failure4():
    # too few arguments
    with pytest.raises(TypeError):
        all_monotone()


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_all_monotone_failure5():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        all_monotone(_hf.CacheNext(1))
