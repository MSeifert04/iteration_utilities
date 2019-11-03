# Licensed under Apache License Version 2.0 - see LICENSE

import operator
import pickle
import sys

import pytest

from iteration_utilities import successive

import helper_funcs as _hf
from helper_cls import T, toT


def test_successive_empty1():
    assert list(successive([])) == []


def test_successive_empty2():
    assert list(successive([T(1)])) == []


def test_successive_empty3():
    assert list(successive([], times=10)) == []


def test_successive_empty4():
    assert list(successive([T(1), T(2), T(3)], times=10)) == []


def test_successive_normal1():
    assert (list(successive([T(1), T(2), T(3), T(4)])) ==
            [(T(1), T(2)), (T(2), T(3)), (T(3), T(4))])


def test_successive_normal2():
    assert (list(successive([T(1), T(2), T(3), T(4)], times=3)) ==
            [(T(1), T(2), T(3)), (T(2), T(3), T(4))])


def test_successive_normal3():
    assert (list(successive([T(1), T(2), T(3), T(4)], times=4)) ==
            [(T(1), T(2), T(3), T(4))])


def test_successive_normal4():
    assert (dict(successive([T(1), T(2), T(3), T(4)])) ==
            {T(1): T(2), T(2): T(3), T(3): T(4)})


def test_successive_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        successive(_hf.FailIter())


def test_successive_failure2():
    with pytest.raises(ValueError):  # times must be > 0
        successive([T(1), T(2), T(3)], 0)


def test_successive_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(successive(_hf.FailNext(), 1))


def test_successive_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        successive()


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_successive_failure5():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        list(successive(_hf.CacheNext(1), 3))


def test_successive_copy1():
    _hf.iterator_copy(successive(toT([1, 2, 3, 4])))


def test_successive_failure_setstate1():
    # first argument must be a tuple
    suc = successive([T(1), T(2), T(3), T(4)], 2)
    with pytest.raises(TypeError):
        suc.__setstate__(([T(1), T(2)], ))


def test_successive_failure_setstate2():
    # length of first argument not equal to times
    suc = successive([T(1), T(2), T(3), T(4)], 2)
    with pytest.raises(ValueError):
        suc.__setstate__(((T(1), ), ))


def test_successive_failure_setstate3():
    # length of first argument not equal to times
    suc = successive([T(1), T(2), T(3), T(4)], 2)
    with pytest.raises(ValueError):
        suc.__setstate__(((T(1), T(2), T(3)), ))


def test_successive_failure_setstate4():
    _hf.iterator_setstate_list_fail(
            successive([T(1), T(2), T(3), T(4)], 2))


def test_successive_failure_setstate5():
    _hf.iterator_setstate_empty_fail(
            successive([T(1), T(2), T(3), T(4)], 2))


def test_successive_pickle1(protocol):
    suc = successive([T(1), T(2), T(3), T(4)])
    assert next(suc) == (T(1), T(2))
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [(T(2), T(3)), (T(3), T(4))]


def test_successive_pickle2(protocol):
    suc = successive([T(1), T(2), T(3), T(4)])
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [(T(1), T(2)), (T(2), T(3)),
                                     (T(3), T(4))]


def test_successive_lengthhint1():
    it = successive([0]*6, 4)
    _hf.check_lengthhint_iteration(it, 3)


def test_successive_lengthhint2():
    assert operator.length_hint(successive([0]*6, 11)) == 0


def test_successive_failure_lengthhint1():
    f_it = _hf.FailLengthHint(toT([1, 2, 3]))
    it = successive(f_it)
    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        operator.length_hint(it)

    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        list(it)


def test_successive_failure_lengthhint2():
    # This only checks for overflow if the length_hint is above PY_SSIZE_T_MAX.
    # In theory that would be possible because with times the length would be
    # shorter but "length_hint" throws the exception so we propagate it.
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize + 1)
    it = successive(of_it)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)
