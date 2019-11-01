# Licensed under Apache License Version 2.0 - see LICENSE

from functools import partial
import pickle

import pytest

from iteration_utilities import iter_except

from helper_cls import T
import helper_funcs as _hf


def test_iterexcept_normal1():
    dct = {T('a'): T(10)}
    assert list(iter_except(dct.popitem, KeyError)) == [(T('a'), T(10))]


def test_iterexcept_normal2():
    # None as "first" argument is equivalent to not passing in a "first".
    dct = {T('a'): T(10)}
    assert list(iter_except(dct.popitem, KeyError, None)) == [(T('a'), T(10))]


def test_iterexcept_first():
    d = {}

    def insert():
        d[T('a')] = T(10)

    exp_out = [None, (T('a'), T(10))]
    assert list(iter_except(d.popitem, KeyError, insert)) == exp_out


def test_iterexcept_attributes1():
    it = iter_except(list.append, ValueError)
    assert it.func is list.append
    assert it.exception is ValueError
    with pytest.raises(AttributeError):
        it.first


def test_iterexcept_attributes2():
    it = iter_except(list.append, ValueError, list)
    assert it.func is list.append
    assert it.exception is ValueError
    assert it.first is list


def test_iterexcept_failure1():
    # wrong exception
    with pytest.raises(KeyError):
        list(iter_except(({T('a'): T(10)}).popitem, ValueError))


def test_iterexcept_failure2():
    # too few arguments
    with pytest.raises(TypeError):
        iter_except()


def test_iterexcept_copy1():
    dct = {T('a'): T(10)}
    _hf.iterator_copy(iter_except(dct.popitem, KeyError))


def test_iterexcept_pickle1(protocol):
    dct = {T('a'): T(10)}
    ie = iter_except(dct.popitem, KeyError)
    x = pickle.dumps(ie, protocol=protocol)
    assert list(pickle.loads(x)) == [(T('a'), T(10))]


def test_iterexcept_pickle2(protocol):
    dct = {T('a'): T(10)}
    ie = iter_except(dct.popitem, KeyError, None)
    x = pickle.dumps(ie, protocol=protocol)
    assert list(pickle.loads(x)) == [(T('a'), T(10))]


def test_iterexcept_pickle3(protocol):
    dct = {}
    first = partial(dct.setdefault, T('a'), T(10))
    ie = iter_except(dct.popitem, KeyError, first)
    x = pickle.dumps(ie, protocol=protocol)
    assert list(pickle.loads(x)) == [T(10), (T('a'), T(10))]


def test_iterexcept_pickle4(protocol):
    dct = {}
    first = partial(dct.setdefault, T('a'), T(10))
    ie = iter_except(dct.popitem, KeyError, first)
    assert next(ie) == T(10)
    x = pickle.dumps(ie, protocol=protocol)
    assert list(pickle.loads(x)) == [(T('a'), T(10))]
