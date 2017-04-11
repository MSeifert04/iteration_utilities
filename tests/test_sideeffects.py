# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle
import sys

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
from helper_cls import T, toT
from helper_leak import memory_leak_decorator


sideeffects = iteration_utilities.sideeffects
return_None = iteration_utilities.return_None


def raise_error_when_below10(val):
    if isinstance(val, tuple):
        if val[0].value < 10:
            raise ValueError()
    else:
        if val.value < 10:
            raise ValueError()


@memory_leak_decorator()
def test_sideeffects_empty1():
    assert list(sideeffects([], return_None)) == []


@memory_leak_decorator()
def test_sideeffects_empty2():
    assert list(sideeffects([], return_None, 0)) == []


@memory_leak_decorator()
def test_sideeffects_empty3():
    assert list(sideeffects([], return_None, 1)) == []


@memory_leak_decorator()
def test_sideeffects_empty4():
    assert list(sideeffects([], return_None, 10)) == []


@memory_leak_decorator()
def test_sideeffects_normal1():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append)) == [T(1), T(2)]
    assert l == [T(1), T(2)]


@memory_leak_decorator()
def test_sideeffects_normal2():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 0)) == [T(1), T(2)]
    assert l == [T(1), T(2)]


@memory_leak_decorator()
def test_sideeffects_normal3():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 1)) == [T(1), T(2)]
    assert l == [(T(1), ), (T(2), )]


@memory_leak_decorator()
def test_sideeffects_normal4():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 2)) == [T(1), T(2)]
    assert l == [(T(1), T(2))]


@memory_leak_decorator()
def test_sideeffects_normal5():
    l = []
    assert list(sideeffects([T(1), T(2), T(3)], l.append,
                            times=2)) == [T(1), T(2), T(3)]
    assert l == [(T(1), T(2)), (T(3), )]


@memory_leak_decorator()
def test_sideeffects_normal6():
    # generator
    l = []
    assert list(sideeffects((i for i in [T(1), T(2)]),
                            l.append, 2)) == [T(1), T(2)]
    assert l == [(T(1), T(2))]


@memory_leak_decorator()
def test_sideeffects_normal7():
    # useless side-effect
    assert list(sideeffects([T(1), T(2)], return_None)) == [T(1), T(2)]


@memory_leak_decorator()
def test_sideeffects_normal8():
    # useless side-effect
    assert list(sideeffects(toT(range(10)), return_None, 3)) == toT(range(10))


@memory_leak_decorator()
def test_sideeffects_attribute1():
    it = sideeffects(toT(range(10)), return_None)
    assert it.times == 0
    assert it.func is return_None
    assert it.count == 0


@memory_leak_decorator(collect=True)
def test_sideeffects_failure1():
    l = []
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        sideeffects(_hf.FailIter(), l.append)
    assert _hf.FailIter.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_sideeffects_failure2():
    l = []
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        sideeffects(_hf.FailIter(), l.append, 1)
    assert _hf.FailIter.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_sideeffects_failure3():
    with pytest.raises(ValueError):
        list(sideeffects([T(1), T(2), T(3)], raise_error_when_below10))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure4():
    with pytest.raises(ValueError):
        list(sideeffects([T(11), T(12), T(3)], raise_error_when_below10))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure5():
    with pytest.raises(ValueError):
        list(sideeffects([T(11), T(12), T(3)], raise_error_when_below10, 2))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure6():
    with pytest.raises(ValueError):
        list(sideeffects([T(3), T(12), T(11)], raise_error_when_below10, 2))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure7():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        list(sideeffects(_hf.FailNext(), lambda x: x))
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_sideeffects_failure8():
    # Too few arguments
    with pytest.raises(TypeError):
        sideeffects()


@memory_leak_decorator(collect=True)
def test_sideeffects_copy1():
    _hf.iterator_copy(sideeffects(toT([1, 2, 3, 4]), return_None))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate1():
    # If times==0 then the second argument must be None
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    with pytest.raises(TypeError):
        se.__setstate__((0, ()))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate2():
    # The first argument must be smaller than the length of the second
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(ValueError):
        se.__setstate__((1, (T(1), )))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate3():
    # The first argument must not be smaller than zero
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(ValueError):
        se.__setstate__((-1, (T(1), )))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate4():
    # The length of the second argument must be equal to the "times".
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(ValueError):
        se.__setstate__((1, (T(1), T(2))))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate5():
    # If the second argument is None then the times must be zero
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    with pytest.raises(TypeError):
        se.__setstate__((0, None))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate6():
    # If the second argument is None then the first argument must be zero
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 0)
    with pytest.raises(TypeError):
        se.__setstate__((1, None))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate7():
    # The second argument must be a tuple or None
    se = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    with pytest.raises(TypeError):
        se.__setstate__((1, [T(1), T(2)]))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate8():
    _hf.iterator_setstate_list_fail(
            sideeffects([T(1), T(2), T(3), T(4)], return_None, 2))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure_setstate9():
    _hf.iterator_setstate_empty_fail(
            sideeffects([T(1), T(2), T(3), T(4)], return_None, 2))


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle1():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    assert next(suc) == T(1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle2():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(1), T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle3():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(1), T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle4():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    assert next(suc) == T(1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle5():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    assert next(suc) == T(1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle6():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    assert next(suc) == T(1)
    assert next(suc) == T(2)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(3), T(4)]


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_sideeffects_lengthhint1():
    it = sideeffects([1, 2, 3, 4, 5, 6], return_None)
    assert operator.length_hint(it) == 6
    next(it)
    assert operator.length_hint(it) == 5
    next(it)
    assert operator.length_hint(it) == 4
    next(it)
    assert operator.length_hint(it) == 3
    next(it)
    assert operator.length_hint(it) == 2
    next(it)
    assert operator.length_hint(it) == 1
    next(it)
    assert operator.length_hint(it) == 0


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_sideeffects_failure_lengthhint1():
    f_it = _hf.FailLengthHint(toT([1, 2, 3]))
    it = sideeffects(f_it, return_None)
    with pytest.raises(_hf.FailLengthHint.EXC_TYP) as exc:
        operator.length_hint(it)
    assert _hf.FailLengthHint.EXC_MSG in str(exc)

    with pytest.raises(_hf.FailLengthHint.EXC_TYP) as exc:
        list(it)
    assert _hf.FailLengthHint.EXC_MSG in str(exc)


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_sideeffects_failure_lengthhint2():
    # This only checks for overflow if the length_hint is above PY_SSIZE_T_MAX
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize + 1)
    it = sideeffects(of_it, return_None)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)
