# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

from iteration_utilities import unique_justseen

import helper_funcs as _hf
from helper_cls import T, toT


class T2:
    def __init__(self, value):
        self.value = value

    def __eq__(self, other):
        raise TypeError()

    def __ne__(self, other):
        raise TypeError()


def test_unique_justseen_empty1():
    assert list(unique_justseen([])) == [] == []


def test_unique_justseen_normal1():
    assert list(unique_justseen(toT([1, 1, 2, 2, 3, 3]))) == toT([1, 2, 3])


def test_unique_justseen_normal2():
    assert list(unique_justseen('aAabBb')) == ['a', 'A', 'a', 'b', 'B', 'b']


def test_unique_justseen_normal3():
    assert list(unique_justseen('aAabBb', key=str.lower)) == ['a', 'b']


def test_unique_justseen_normal4():
    # key=None is identical to no key
    assert list(unique_justseen(toT([1, 1, 2, 2, 3, 3]),
                                None)) == toT([1, 2, 3])


def test_unique_justseen_attributes1():
    it = unique_justseen(toT([1, 1, 2, 2, 3, 3]), None)
    assert it.key is None
    with pytest.raises(AttributeError):
        it.lastseen

    next(it)

    assert it.lastseen == T(1)


def test_unique_justseen_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        unique_justseen(_hf.FailIter())


def test_unique_justseen_failure2():
    with pytest.raises(TypeError):  # function call fails
        list(unique_justseen([T(1), T(2), T(3)], key=lambda x: x + 'a'))


def test_unique_justseen_failure3():
    # objects do not support eq or ne
    with pytest.raises(TypeError):
        list(unique_justseen([T2(1), T2(2)]))


def test_unique_justseen_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(unique_justseen(_hf.FailNext()))


def test_unique_justseen_failure5():
    # Too few arguments
    with pytest.raises(TypeError):
        unique_justseen()


def test_unique_justseen_copy1():
    _hf.iterator_copy(unique_justseen([T(1), T(2), T(3)]))


def test_unique_justseen_failure_setstate1():
    _hf.iterator_setstate_list_fail(unique_justseen(toT([1, 2, 3])))


def test_unique_justseen_failure_setstate2():
    _hf.iterator_setstate_empty_fail(unique_justseen(toT([1, 2, 3])))


def test_unique_justseen_pickle1(protocol):
    ujs = unique_justseen([T(1), T(2), T(3)])
    x = pickle.dumps(ujs, protocol=protocol)
    assert list(pickle.loads(x)) == toT([1, 2, 3])


def test_unique_justseen_pickle2(protocol):
    ujs = unique_justseen([T(1), T(2), T(3)])
    assert next(ujs) == T(1)
    x = pickle.dumps(ujs, protocol=protocol)
    assert list(pickle.loads(x)) == toT([2, 3])


def test_unique_justseen_pickle3(protocol):
    ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
    x = pickle.dumps(ujs, protocol=protocol)
    assert list(pickle.loads(x)) == ['a']


def test_unique_justseen_pickle4(protocol):
    ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
    assert next(ujs) == 'a'
    x = pickle.dumps(ujs, protocol=protocol)
    assert list(pickle.loads(x)) == []
