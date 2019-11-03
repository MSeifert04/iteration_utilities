# Licensed under Apache License Version 2.0 - see LICENSE

import itertools
import operator
import pickle
import sys

import pytest

from iteration_utilities import roundrobin

import helper_funcs as _hf
from helper_cls import T, toT


def test_roundrobin_empty1():
    assert list(roundrobin()) == []


def test_roundrobin_empty2():
    assert list(roundrobin([])) == []


def test_roundrobin_empty3():
    assert list(roundrobin([], (), {})) == []


def test_roundrobin_normal1():
    assert list(roundrobin([T(1)], [T(1), T(2)], [T(1), T(2), T(3)]
                           )) == toT([1, 1, 1, 2, 2, 3])


def test_roundrobin_normal2():
    assert list(roundrobin([T(1), T(2), T(3)], [T(1)], [T(1), T(2)]
                           )) == toT([1, 1, 1, 2, 2, 3])


def test_roundrobin_normal3():
    assert list(roundrobin([T(1), T(2)], [T(1), T(2), T(3)], [T(1)]
                           )) == toT([1, 1, 1, 2, 2, 3])


def test_roundrobin_normal4():
    # generator
    assert list(roundrobin((i for i in [T(1), T(2), T(3)]),
                           (i for i in [T(1)]),
                           (i for i in [T(1), T(2)]))
                ) == toT([1, 1, 1, 2, 2, 3])


def test_roundrobin_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        roundrobin(_hf.FailIter())


def test_roundrobin_failure2():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        roundrobin([T(1)], _hf.FailIter())


def test_roundrobin_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(roundrobin(_hf.FailNext()))


def test_roundrobin_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        list(roundrobin([T(1), T(2)], _hf.FailNext()))


def test_roundrobin_failure5():
    # Test that a failing iterator doesn't raise a SystemError
    rr = roundrobin(_hf.FailNext(offset=1, repeats=10),
                    [T(1), T(2), T(3), T(4)])
    assert next(rr) == T(1)
    assert next(rr) == T(1)
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(rr)


def test_roundrobin_copy1():
    _hf.iterator_copy(roundrobin([T(1), T(2), T(3), T(4)]))


def test_roundrobin_failure_setstate1():
    # setstate active < 0
    rr = roundrobin([T(1), T(2), T(3), T(4)])
    with pytest.raises(ValueError):
        rr.__setstate__((1, -1))


def test_roundrobin_failure_setstate2():
    # setstate numactive < 0
    rr = roundrobin([T(1), T(2), T(3), T(4)])
    with pytest.raises(ValueError):
        rr.__setstate__((-1, 0))


def test_roundrobin_failure_setstate3():
    # setstate numactive <= active
    rr = roundrobin([T(1), T(2), T(3), T(4)])
    with pytest.raises(ValueError):
        rr.__setstate__((1, 1))


def test_roundrobin_failure_setstate4():
    # setstate numactive <= active (numactive = 0)
    rr = roundrobin()
    with pytest.raises(ValueError):
        rr.__setstate__((0, 1))


def test_roundrobin_failure_setstate5():
    # setstate numactive > len(iteratortuple)
    rr = roundrobin([T(1), T(2), T(3), T(4)])
    with pytest.raises(ValueError):
        rr.__setstate__((2, 1))


def test_roundrobin_failure_setstate6():
    # setstate numactive > len(iteratortuple) (after exhausting one iterable)
    rr = roundrobin([T(1)], [T(1), T(2), T(3), T(4)])
    assert [i for i in itertools.islice(rr, 3)] == toT([1, 1, 2])
    with pytest.raises(ValueError):
        rr.__setstate__((2, 1))


def test_roundrobin_failure_setstate7():
    _hf.iterator_setstate_list_fail(
            roundrobin([T(1)], [T(1), T(2), T(3), T(4)]))


def test_roundrobin_failure_setstate8():
    _hf.iterator_setstate_empty_fail(
            roundrobin([T(1)], [T(1), T(2), T(3), T(4)]))


def test_roundrobin_pickle1(protocol):
    rr = roundrobin([T(1), T(2), T(3)], [T(1), T(2), T(3)])
    assert next(rr) == T(1)
    x = pickle.dumps(rr, protocol=protocol)
    assert list(pickle.loads(x)) == toT([1, 2, 2, 3, 3])


def test_roundrobin_pickle2(protocol):
    rr2 = roundrobin([T(1)], [T(1), T(2), T(3)])
    assert next(rr2) == T(1)
    assert next(rr2) == T(1)
    assert next(rr2) == T(2)
    x = pickle.dumps(rr2, protocol=protocol)
    assert list(pickle.loads(x)) == [T(3)]


def test_roundrobin_lengthhint1():
    it = roundrobin([0], [1, 2, 3], [1])
    _hf.check_lengthhint_iteration(it, 5)


def test_roundrobin_failure_lengthhint1():
    f_it = _hf.FailLengthHint(toT([1, 2, 3]))
    it = roundrobin(f_it)
    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        operator.length_hint(it)

    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        list(it)


def test_roundrobin_failure_lengthhint2():
    # This only checks for overflow if the length_hint is above PY_SSIZE_T_MAX
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize + 1)
    it = roundrobin(of_it)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)


def test_roundrobin_failure_lengthhint3():
    # Check if by adding the different lengths it could lead to overflow.
    # We use two iterables both with sys.maxsize length.
    it1 = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize)
    it2 = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize)
    it = roundrobin(it1, it2)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)
