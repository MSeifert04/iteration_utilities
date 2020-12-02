# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

from iteration_utilities import always_iterable

import helper_funcs as _hf


def test_always_iterable_with_iterable():
    assert tuple(always_iterable((1, 2, 3))) == (1, 2, 3)
    assert tuple(always_iterable((1, 2, 3))) == (1, 2, 3)


def test_always_iterable_with_string():
    assert list(always_iterable("abc")) == ["abc"]
    assert list(always_iterable(b"abc")) == [b"abc"]


def test_always_iterable_excluding():
    assert list(always_iterable("abc", excluded_types=None)) == ["a", "b", "c"]
    assert list(always_iterable([1, 2, 3], excluded_types=list)) == [[1, 2, 3]]
    assert list(always_iterable([1, 2, 3], excluded_types=tuple)) == [1, 2, 3]


def test_always_iterable_str_subclass():
    """The default is that only plain `str` or `bytes` are wrapped, but if given
    explicitly also subclasses should work.
    """
    class StringSubClass(str):
        ...

    assert list(always_iterable(StringSubClass("abc"))) == ["a", "b", "c"]
    assert list(always_iterable(StringSubClass("abc"), excluded_types=str)) == ["abc"]


def test_always_iterable_empty_when_none():
    assert list(always_iterable(None)) == [None]
    assert list(always_iterable(None, empty_if_none=True)) == []
    assert list(always_iterable(1, empty_if_none=True)) == [1]


def test_always_iterable_invalid_argument():
    with pytest.raises(TypeError):
        always_iterable()
    with pytest.raises(TypeError):
        always_iterable([1, 2, 3], excluded=None)


def test_always_iterable_not_iterable():
    assert list(always_iterable(1)) == [1]


def test_always_iterable_fails_with_non_typeerror_when_iter():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        always_iterable(_hf.FailIter())


def test_always_iterable_fails_isinstance():
    with pytest.raises(_hf.FailingIsinstanceClass.EXC_TYP, match=_hf.FailingIsinstanceClass.EXC_MSG):
        always_iterable(1, _hf.FailingIsinstanceClass)
