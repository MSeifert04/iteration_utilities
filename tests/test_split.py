# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
from functools import partial
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
from helper_cls import T, toT
from helper_leak import memory_leak_decorator


split = iteration_utilities.split


equalsthreeT = partial(operator.eq, T(3))


@memory_leak_decorator()
def test_split_empty1():
    assert list(split([], lambda x: False)) == []


@memory_leak_decorator()
def test_split_normal1():
    assert list(split([T(1), T(2), T(3)],
                      lambda x: x.value == 2)) == [[T(1)], [T(3)]]


@memory_leak_decorator()
def test_split_normal2():
    assert list(split([T(1), T(2), T(3)],
                      lambda x: x.value == 3)) == [toT([1, 2])]


@memory_leak_decorator()
def test_split_normal3():
    # using a generator
    assert list(split((i for i in [T(1), T(2), T(3)]),
                      lambda x: x.value == 2)) == [[T(1)], [T(3)]]


@memory_leak_decorator()
def test_split_keep1():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 2,
                      keep=True)) == [[T(1)], [T(2)], [T(3)]]


@memory_leak_decorator()
def test_split_keep2():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 3,
                      keep=True)) == [[T(1), T(2)], [T(3)]]


@memory_leak_decorator()
def test_split_keep_before1():
    assert list(split([T(1), T(2), T(3), T(4)], lambda x: x.value == 3,
                      keep_before=True)) == [[T(1), T(2), T(3)], [T(4)]]


@memory_leak_decorator()
def test_split_keep_before2():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 3,
                      keep_before=True)) == [[T(1), T(2), T(3)]]


@memory_leak_decorator()
def test_split_keep_after1():
    assert list(split([T(1), T(2), T(3), T(4)], lambda x: x.value == 3,
                      keep_after=True)) == [[T(1), T(2)], [T(3), T(4)]]


@memory_leak_decorator()
def test_split_keep_after2():
    assert list(split([T(1), T(2), T(3)], lambda x: x.value == 3,
                      keep_after=True)) == [[T(1), T(2)], [T(3)]]


@memory_leak_decorator()
def test_split_maxsplit1():
    assert list(split([T(1), T(2), T(3), T(4), T(5)],
                      lambda x: x.value % 2 == 0,
                      maxsplit=1)) == [[T(1)], [T(3), T(4), T(5)]]


@memory_leak_decorator()
def test_split_maxsplit2():
    assert list(split([T(1), T(2), T(3), T(4), T(5)],
                      lambda x: x.value % 2 == 0,
                      maxsplit=2)) == [[T(1)], [T(3)], [T(5)]]


@memory_leak_decorator()
def test_split_eq1():
    assert list(split([T(1), T(2), T(3), T(2), T(5)], T(2),
                      eq=True)) == [[T(1)], [T(3)], [T(5)]]


@memory_leak_decorator()
def test_split_attributes1():
    it = split([], iteration_utilities.return_False)
    assert it.key is iteration_utilities.return_False
    assert it.maxsplit == -1
    assert not it.keep
    assert not it.keep_before
    assert not it.keep_after
    assert not it.eq


@memory_leak_decorator(collect=True)
def test_split_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        split(_hf.FailIter(), lambda x: False)
    assert _hf.FailIter.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_split_failure2():
    # func fails
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], lambda x: T(x.value + 'a')))


@memory_leak_decorator(collect=True)
def test_split_failure3():
    # cmp fails
    with pytest.raises(TypeError):
        list(split([T(1), T(2), T(3)], T('a'), eq=True))


@memory_leak_decorator(collect=True)
def test_split_failure4():
    # more than one keep* parameter
    with pytest.raises(ValueError) as exc:
        split(toT([1, 2, 3, 4]), T(2), eq=True,
              keep=True, keep_before=True)
    assert '`keep`, `keep_before`, `keep_after`' in str(exc)


@memory_leak_decorator(collect=True)
def test_split_failure5():
    # more than one keep* parameter
    with pytest.raises(ValueError) as exc:
        split(toT([1, 2, 3, 4]), T(2), eq=True,
              keep=True, keep_after=True)
    assert '`keep`, `keep_before`, `keep_after`' in str(exc)


@memory_leak_decorator(collect=True)
def test_split_failure6():
    # more than one keep* parameter
    with pytest.raises(ValueError) as exc:
        split(toT([1, 2, 3, 4]), T(2), eq=True,
              keep_before=True, keep_after=True)
    assert '`keep`, `keep_before`, `keep_after`' in str(exc)


@memory_leak_decorator(collect=True)
def test_split_failure11():
    # more than one keep* parameter
    with pytest.raises(ValueError) as exc:
        split(toT([1, 2, 3, 4]), T(2), eq=True,
              keep=True, keep_before=True, keep_after=True)
    assert '`keep`, `keep_before`, `keep_after`' in str(exc)


@memory_leak_decorator(collect=True)
def test_split_failure7():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        next(split(_hf.FailNext(), iteration_utilities.return_False))
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_split_failure8():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        next(split(_hf.FailNext(offset=1),
                   iteration_utilities.return_False))
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_split_failure9():
    # Too few arguments
    with pytest.raises(TypeError):
        split()


@memory_leak_decorator(collect=True)
def test_split_failure10():
    # maxsplit <= -2
    with pytest.raises(ValueError) as exc:
        split(toT([1, 2, 3, 4]), T(2), eq=True, maxsplit=-2)
    assert '`maxsplit`' in str(exc)


@memory_leak_decorator(collect=True)
def test_split_copy1():
    _hf.iterator_copy(split(toT(range(1, 9)), equalsthreeT))


@memory_leak_decorator(collect=True)
def test_split_failure_setstate1():
    _hf.iterator_setstate_list_fail(
            split(toT(range(1, 9)), equalsthreeT))


@memory_leak_decorator(collect=True)
def test_split_failure_setstate2():
    _hf.iterator_setstate_empty_fail(
            split(toT(range(1, 9)), equalsthreeT))


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle1():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT)
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [[T(1), T(2)], [T(4), T(5)], [T(7), T(8)]]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle2():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT([4, 5]), toT([7, 8])]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle3():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT, keep=True)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT(i)
                                     for i in [[3], [4, 5], [3], [7, 8]]]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle4():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, equalsthreeT, maxsplit=1)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT([4, 5, 3, 7, 8])]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_split_pickle5():
    l = [T(1), T(2), T(3), T(4), T(5), T(3), T(7), T(8)]
    spl = split(l, T(3), eq=True)
    assert next(spl) == toT([1, 2])
    x = pickle.dumps(spl)
    assert list(pickle.loads(x)) == [toT([4, 5]), toT([7, 8])]
