# Licensed under Apache License Version 2.0 - see LICENSE

import operator
import pickle

import pytest

import iteration_utilities
from iteration_utilities import duplicates

import helper_funcs as _hf
from helper_cls import T, toT


def test_duplicates_empty1():
    assert list(duplicates([])) == []


def test_duplicates_normal1():
    assert list(duplicates([T(1), T(2), T(1)])) == [T(1)]


def test_duplicates_key1():
    assert list(duplicates([T(1), T(2), T(1)], abs)) == [T(1)]


def test_duplicates_key2():
    assert list(duplicates([T(1), T(1), T(-1)], abs)) == toT([1, -1])


def test_duplicates_unhashable1():
    assert list(duplicates([{T(1): T(1)}, {T(2): T(2)}, {T(1): T(1)}]
                           )) == [{T(1): T(1)}]


def test_duplicates_unhashable2():
    assert list(duplicates([[T(1)], [T(2)], [T(1)]])) == [[T(1)]]


def test_duplicates_unhashable3():
    assert list(duplicates([[T(1), T(1)], [T(1), T(2)],
                            [T(1), T(3)]], operator.itemgetter(0)
                           )) == [[T(1), T(2)], [T(1), T(3)]]


def test_duplicates_getter1():
    t = duplicates([T(1), T([0, 0]), T(3), T(1)])
    assert not t.seen
    assert t.key is None
    assert next(t) == T(1)
    assert T(1) in t.seen
    assert T(3) in t.seen
    assert T([0, 0]) in t.seen
    assert t.key is None


def test_duplicates_getter2():
    t = duplicates([T(1), T([0, 0]), T(3), T(1)],
                   key=iteration_utilities.return_identity)
    assert t.key is iteration_utilities.return_identity


def test_duplicates_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        duplicates(_hf.FailIter())


def test_duplicates_failure2():
    with pytest.raises(TypeError):
        list(duplicates([T(1), T(2), T(3), T('a')], abs))


def test_duplicates_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(duplicates(_hf.FailNext()))


def test_duplicates_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        duplicates()


def test_duplicates_failure5():
    # Failure when comparing the object to the objects in the list
    with pytest.raises(_hf.FailEqNoHash.EXC_TYP, match=_hf.FailEqNoHash.EXC_MSG):
        list(duplicates([[T(1)], _hf.FailEqNoHash()]))


def test_duplicates_failure6():
    # Failure (no TypeError) when trying to hash the value
    with pytest.raises(_hf.FailHash.EXC_TYP, match=_hf.FailHash.EXC_MSG):
        list(duplicates([T(1), _hf.FailHash()]))


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_duplicates_failure7():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        list(duplicates(_hf.CacheNext(1)))


def test_duplicates_failure_setstate1():
    # __setstate__ only accepts Seen instances
    dp = duplicates(toT([1, 1]))
    with pytest.raises(TypeError):
        dp.__setstate__((set(toT(range(1, 3))),))


def test_duplicates_failure_setstate2():
    _hf.iterator_setstate_list_fail(duplicates(toT([1, 1])))


def test_duplicates_failure_setstate3():
    _hf.iterator_setstate_empty_fail(duplicates(toT([1, 1])))


def test_duplicates_copy1():
    _hf.iterator_copy(duplicates(toT([1, 1])))


def test_duplicates_pickle1(protocol):
    dpl = duplicates([T(1), T(2), T(1), T(2)])
    assert next(dpl) == T(1)
    x = pickle.dumps(dpl, protocol=protocol)
    assert list(pickle.loads(x)) == [T(2)]
