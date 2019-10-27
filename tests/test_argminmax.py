# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import argmin, argmax

import helper_funcs as _hf
from helper_cls import T


def test_argmax_normal1():
    # Just one test for argmax because the internals are identical to argmin
    assert argmax(T(0), T(1), T(2)) == 2


def test_argmin_positional1():
    assert argmin(T(0), T(1), T(2)) == 0


def test_argmin_positional2():
    assert argmin(T(3), T(0), T(1)) == 1


def test_argmin_positional3():
    # key=None is identical to no key
    assert argmin(T(3), T(1), T(2), key=None) == 1


def test_argmin_sequence1():
    assert argmin([T(3), T(0), T(1)]) == 1


def test_argmin_generator1():
    assert argmin((T(i) for i in [5, 4, 3, 2])) == 3


def test_argmin_key1():
    assert argmin([T(3), T(-2), T(1)], key=abs) == 2


def test_argmin_default1():
    assert argmin([], default=2) == 2


def test_argmin_failure1():
    # default not possible if given multiple positional arguments
    with pytest.raises(TypeError):
        argmin(T(3), T(0), T(1), default=1)


def test_argmin_failure2():
    # not integer default value
    with pytest.raises(TypeError):
        argmin([T(3), T(0), T(1)], default='1.5', key=lambda x: x + 1)


def test_argmin_failure3():
    # unwanted kwarg
    with pytest.raises(TypeError):
        argmin([T(3), T(0), T(1)], default=1, key=lambda x: x + 1, blub=10)


def test_argmin_failure4():
    # no args
    with pytest.raises(TypeError):
        argmin(key=lambda x: x + 1)


def test_argmin_failure5():
    # empty sequence
    with pytest.raises(ValueError):
        argmin([], key=lambda x: x + 1)


def test_argmin_failure6():
    # cmp failed
    with pytest.raises(TypeError):
        argmin([T(1), T(2), T('a')], key=lambda x: x)


def test_argmin_failure7():
    # key failed
    with pytest.raises(TypeError):
        argmin([T(1), T(2), T('a')], key=lambda x: x + 1)


def test_argmin_failure8():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        argmin(_hf.FailNext())


def test_argmin_failure9():
    # Test that a failing iterator doesn't raise a SystemError
    # with default
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        argmin(_hf.FailNext(), default=1)


def test_argmin_failure10():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        argmin(_hf.FailIter())


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_argmin_failure11():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        argmin(_hf.CacheNext(1))
