# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import dotproduct

import helper_funcs as _hf
from helper_cls import T


def test_dotproduct_empty1():
    assert dotproduct([], []) == 0


def test_dotproduct_normal1():
    assert dotproduct([T(1), T(2), T(3)], [T(1), T(2), T(3)]) == T(14)


def test_dotproduct_normal2():
    assert dotproduct([T(100), T(200), T(300)],
                      [T(100), T(200), T(300)]) == T(140000)


def test_dotproduct_normal3():
    # generators
    assert dotproduct((i for i in [T(1), T(2), T(3)]),
                      (i for i in [T(1), T(2), T(3)])) == T(14)


def test_dotproduct_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        dotproduct(_hf.FailIter(), [T(1)])


def test_dotproduct_failure2():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        dotproduct([T(1)], _hf.FailIter())


def test_dotproduct_failure3():
    # multiplication fails
    with pytest.raises(TypeError):
        dotproduct([T(1)], [1])


def test_dotproduct_failure4():
    # multiplication fails (later)
    with pytest.raises(TypeError):
        dotproduct([T(1), T(1)], [T(1), 1])


def test_dotproduct_failure5():
    # addition fails
    with pytest.raises(TypeError):
        dotproduct([T(1), 1], [T(1), 1])


def test_dotproduct_failure6():
    # addition fails (inverted)
    with pytest.raises(TypeError):
        dotproduct([1, T(1), 1], [1, T(1), 1])


def test_dotproduct_failure7():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        dotproduct(_hf.FailNext(), _hf.FailNext())


def test_dotproduct_failure8():
    # Too few arguments
    with pytest.raises(TypeError):
        dotproduct()


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_dotproduct_failure9():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        dotproduct(_hf.CacheNext(1), _hf.CacheNext(1))
