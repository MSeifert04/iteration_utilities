# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import one

import helper_funcs as _hf
from helper_cls import T


def test_one_normal1():
    assert one([T(0)]) == T(0)


def test_one_normal2():
    assert one('a') == 'a'


def test_one_normal3():
    assert one({T('o'): T(10)}) == T('o')


def test_one_normal4():
    # generator with one item
    assert one(i for i in [T(0)]) == T(0)


def test_one_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        one(_hf.FailIter())


def test_one_failure2():
    # empty iterable
    with pytest.raises(ValueError):
        one([])


def test_one_failure3():
    # more than 1 element
    with pytest.raises(ValueError) as exc:
        one([T(1), T(2)])
    assert "'T(1), T(2)[, ...]'" in str(exc.value)


def test_one_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        one(_hf.FailNext())


def test_one_failure5():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        one(_hf.FailNext(offset=1))


def test_one_failure6():
    # generator without items
    with pytest.raises(ValueError):
        one(i for i in [])


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_one_failure7():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        one(_hf.CacheNext(1))
