# Licensed under Apache License Version 2.0 - see LICENSE

import operator
import pickle
import sys

import pytest

from iteration_utilities import intersperse

import helper_funcs as _hf
from helper_cls import T, toT


def test_intersperse_empty1():
    assert list(intersperse([], T(0))) == []


def test_intersperse_empty2():
    assert list(intersperse([T(1)], T(0))) == [T(1)]


def test_intersperse_normal1():
    assert list(intersperse([T(1), T(2)], T(0))) == toT([1, 0, 2])


def test_intersperse_attributes1():
    it = intersperse([T(1), T(2)], T(0))
    assert it.fillvalue == T(0)


def test_intersperse_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        intersperse(_hf.FailIter(), T(0))


def test_intersperse_failure2():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(intersperse(_hf.FailNext(), T(0)))


def test_intersperse_failure3():
    # Too few arguments
    with pytest.raises(TypeError):
        intersperse()


def test_intersperse_copy1():
    _hf.iterator_copy(intersperse(toT([1, 2, 3]), T(0)))


def test_intersperse_failure_setstate1():
    # When start==0 then no second item should be given to setstate
    its = intersperse(toT([1, 1]), None)
    with pytest.raises(ValueError):
        its.__setstate__((0, T(1)))


def test_intersperse_failure_setstate2():
    _hf.iterator_setstate_list_fail(intersperse(toT([1, 1]), None))


def test_intersperse_failure_setstate3():
    _hf.iterator_setstate_empty_fail(intersperse(toT([1, 1]), None))


def test_intersperse_pickle1(protocol):
    its = intersperse(toT([1, 2, 3]), T(0))
    x = pickle.dumps(its, protocol=protocol)
    assert list(pickle.loads(x)) == toT([1, 0, 2, 0, 3])


def test_intersperse_pickle2(protocol):
    its = intersperse(toT([1, 2, 3]), T(0))
    assert next(its) == T(1)
    x = pickle.dumps(its, protocol=protocol)
    assert list(pickle.loads(x)) == toT([0, 2, 0, 3])


def test_intersperse_pickle3(protocol):
    its = intersperse([T(1), T(2), T(3)], T(0))
    assert next(its) == T(1)
    assert next(its) == T(0)
    x = pickle.dumps(its, protocol=protocol)
    assert list(pickle.loads(x)) == toT([2, 0, 3])


def test_intersperse_pickle4(protocol):
    its = intersperse([T(1), T(2), T(3)], T(0))
    assert next(its) == T(1)
    assert next(its) == T(0)
    assert next(its) == T(2)
    x = pickle.dumps(its, protocol=protocol)
    assert list(pickle.loads(x)) == toT([0, 3])


def test_intersperse_lengthhint1():
    it = intersperse([1, 2, 3], 2)
    _hf.check_lengthhint_iteration(it, 5)


def test_intersperse_lengthhint_failure1():
    f_it = _hf.FailLengthHint(toT([1, 2, 3]))
    it = intersperse(f_it, 2)
    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        operator.length_hint(it)

    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        list(it)


def test_intersperse_lengthhint_failure2():
    # This is the easy way to overflow the length_hint: If the iterable itself
    # has a length_hint > sys.maxsize
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize + 1)
    it = intersperse(of_it, 2)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)


def test_intersperse_lengthhint_failure3():
    # The length_hint method multiplies the length_hint of the iterable with
    # 2 (and adds/subtracts 1) so it's actually possible to have overflow even
    # if the length of the iterable doesn't trigger the overflow!
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize)
    it = intersperse(of_it, 2)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)
