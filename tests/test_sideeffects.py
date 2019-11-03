# Licensed under Apache License Version 2.0 - see LICENSE

import operator
import pickle
import sys

import pytest

from iteration_utilities import sideeffects, return_None

import helper_funcs as _hf
from helper_cls import T, toT


def raise_error_when_below10(val):
    if isinstance(val, tuple):
        if val[0].value < 10:
            raise ValueError()
    else:
        if val.value < 10:
            raise ValueError()


def test_sideeffects_empty1():
    assert list(sideeffects([], return_None)) == []


def test_sideeffects_empty2():
    assert list(sideeffects([], return_None, 0)) == []


def test_sideeffects_empty3():
    assert list(sideeffects([], return_None, 1)) == []


def test_sideeffects_empty4():
    assert list(sideeffects([], return_None, 10)) == []


def test_sideeffects_normal1():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append)) == [T(1), T(2)]
    assert l == [T(1), T(2)]


def test_sideeffects_normal2():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 0)) == [T(1), T(2)]
    assert l == [T(1), T(2)]


def test_sideeffects_normal3():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 1)) == [T(1), T(2)]
    assert l == [(T(1), ), (T(2), )]


def test_sideeffects_normal4():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 2)) == [T(1), T(2)]
    assert l == [(T(1), T(2))]


def test_sideeffects_normal5():
    l = []
    assert list(sideeffects([T(1), T(2), T(3)], l.append,
                            times=2)) == [T(1), T(2), T(3)]
    assert l == [(T(1), T(2)), (T(3), )]


def test_sideeffects_normal6():
    # generator
    l = []
    assert list(sideeffects((i for i in [T(1), T(2)]),
                            l.append, 2)) == [T(1), T(2)]
    assert l == [(T(1), T(2))]


def test_sideeffects_normal7():
    # useless side-effect
    assert list(sideeffects([T(1), T(2)], return_None)) == [T(1), T(2)]


def test_sideeffects_normal8():
    # useless side-effect
    assert list(sideeffects(toT(range(10)), return_None, 3)) == toT(range(10))


def test_sideeffects_attribute1():
    it = sideeffects(toT(range(10)), return_None)
    assert it.times == 0
    assert it.func is return_None
    assert it.count == 0


def test_sideeffects_failure1():
    l = []
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        sideeffects(_hf.FailIter(), l.append)


def test_sideeffects_failure2():
    l = []
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        sideeffects(_hf.FailIter(), l.append, 1)


def test_sideeffects_failure3():
    with pytest.raises(ValueError):
        list(sideeffects([T(1), T(2), T(3)], raise_error_when_below10))


def test_sideeffects_failure4():
    with pytest.raises(ValueError):
        list(sideeffects([T(11), T(12), T(3)], raise_error_when_below10))


def test_sideeffects_failure5():
    with pytest.raises(ValueError):
        list(sideeffects([T(11), T(12), T(3)], raise_error_when_below10, 2))


def test_sideeffects_failure6():
    with pytest.raises(ValueError):
        list(sideeffects([T(3), T(12), T(11)], raise_error_when_below10, 2))


def test_sideeffects_failure7():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        list(sideeffects(_hf.FailNext(), lambda x: x))


def test_sideeffects_failure8():
    # Too few arguments
    with pytest.raises(TypeError):
        sideeffects()


def test_sideeffects_copy1():
    _hf.iterator_copy(sideeffects(toT([1, 2, 3, 4]), return_None))


def test_sideeffects_failure_setstate1():
    # If times==0 then the second argument must be None
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    with pytest.raises(TypeError):
        se.__setstate__((0, ()))


def test_sideeffects_failure_setstate2():
    # The first argument must be smaller than the length of the second
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(ValueError):
        se.__setstate__((1, (T(1), )))


def test_sideeffects_failure_setstate3():
    # The first argument must not be smaller than zero
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(ValueError):
        se.__setstate__((-1, (T(1), )))


def test_sideeffects_failure_setstate4():
    # The length of the second argument must be equal to the "times".
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(ValueError):
        se.__setstate__((1, (T(1), T(2))))


def test_sideeffects_failure_setstate5():
    # If the second argument is None then the times must be zero
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(TypeError):
        se.__setstate__((0, None))


def test_sideeffects_failure_setstate6():
    # If the second argument is None then the first argument must be zero
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 0)
    with pytest.raises(TypeError):
        se.__setstate__((1, None))


def test_sideeffects_failure_setstate7():
    # The second argument must be a tuple or None
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    with pytest.raises(TypeError):
        se.__setstate__((1, [T(1), T(2)]))


def test_sideeffects_failure_setstate8():
    _hf.iterator_setstate_list_fail(
            sideeffects([T(1), T(2), T(3), T(4)], return_None, 2))


def test_sideeffects_failure_setstate9():
    _hf.iterator_setstate_empty_fail(
            sideeffects([T(1), T(2), T(3), T(4)], return_None, 2))


def test_sideeffects_pickle1(protocol):
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    assert next(suc) == T(1)
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


def test_sideeffects_pickle2(protocol):
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [T(1), T(2), T(3), T(4)]


def test_sideeffects_pickle3(protocol):
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [T(1), T(2), T(3), T(4)]


def test_sideeffects_pickle4(protocol):
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    assert next(suc) == T(1)
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


def test_sideeffects_pickle5(protocol):
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    assert next(suc) == T(1)
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


def test_sideeffects_pickle6(protocol):
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    assert next(suc) == T(1)
    assert next(suc) == T(2)
    x = pickle.dumps(suc, protocol=protocol)
    assert list(pickle.loads(x)) == [T(3), T(4)]


def test_sideeffects_lengthhint1():
    it = sideeffects([1, 2, 3, 4, 5, 6], return_None)
    _hf.check_lengthhint_iteration(it, 6)


def test_sideeffects_failure_lengthhint1():
    f_it = _hf.FailLengthHint(toT([1, 2, 3]))
    it = sideeffects(f_it, return_None)
    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        operator.length_hint(it)

    with pytest.raises(_hf.FailLengthHint.EXC_TYP, match=_hf.FailLengthHint.EXC_MSG):
        list(it)


def test_sideeffects_failure_lengthhint2():
    # This only checks for overflow if the length_hint is above PY_SSIZE_T_MAX
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize + 1)
    it = sideeffects(of_it, return_None)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)
