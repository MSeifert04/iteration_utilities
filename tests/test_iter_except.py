# Built-ins
from __future__ import absolute_import, division, print_function
from functools import partial
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_cls import T, toT, failingTIterator
from helper_funcs import iterator_copy
from helper_leak import memory_leak_decorator


iter_except = iteration_utilities.iter_except


@memory_leak_decorator()
def test_iterexcept_normal1():
    dct = {T('a'): T(10)}
    assert list(iter_except(dct.popitem, KeyError)) == [(T('a'), T(10))]


@memory_leak_decorator()
def test_iterexcept_normal2():
    # None as "first" argument is equivalent to not passing in a "first".
    dct = {T('a'): T(10)}
    assert list(iter_except(dct.popitem, KeyError, None)) == [(T('a'), T(10))]


@memory_leak_decorator()
def test_iterexcept_first():
    d = {}

    def insert():
        d[T('a')] = T(10)

    exp_out = [None, (T('a'), T(10))]
    assert list(iter_except(d.popitem, KeyError, insert)) == exp_out


@memory_leak_decorator(collect=True)
def test_iterexcept_failure1():
    # wrong exception
    with pytest.raises(KeyError):
        list(iter_except(({T('a'): T(10)}).popitem, ValueError))


@memory_leak_decorator(collect=True)
def test_iterexcept_failure2():
    # too few arguments
    with pytest.raises(TypeError):
        iter_except()


@memory_leak_decorator(collect=True)
def test_iterexcept_copy1():
    dct = {T('a'): T(10)}
    iterator_copy(iter_except(dct.popitem, KeyError))


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='cannot pickle this on Python < 3.4.')
@memory_leak_decorator(offset=1)
def test_iterexcept_pickle1():
    dct = {T('a'): T(10)}
    ie = iter_except(dct.popitem, KeyError)
    x = pickle.dumps(ie)
    assert list(pickle.loads(x)) == [(T('a'), T(10))]


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='cannot pickle this on Python < 3.4.')
@memory_leak_decorator(offset=1)
def test_iterexcept_pickle2():
    dct = {T('a'): T(10)}
    ie = iter_except(dct.popitem, KeyError, None)
    x = pickle.dumps(ie)
    assert list(pickle.loads(x)) == [(T('a'), T(10))]


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='cannot pickle this on Python < 3.4.')
@memory_leak_decorator(offset=1)
def test_iterexcept_pickle3():
    dct = {}
    first = partial(dct.setdefault, T('a'), T(10))
    ie = iter_except(dct.popitem, KeyError, first)
    x = pickle.dumps(ie)
    assert list(pickle.loads(x)) == [T(10), (T('a'), T(10))]


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='cannot pickle this on Python < 3.4.')
@memory_leak_decorator(offset=1)
def test_iterexcept_pickle4():
    dct = {}
    first = partial(dct.setdefault, T('a'), T(10))
    ie = iter_except(dct.popitem, KeyError, first)
    assert next(ie) == T(10)
    x = pickle.dumps(ie)
    assert list(pickle.loads(x)) == [(T('a'), T(10))]
