# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

import iteration_utilities
from iteration_utilities import applyfunc, getitem

from helper_cls import T
from helper_funcs import iterator_copy


def test_applyfunc_normal1():
    assert list(getitem(applyfunc(lambda x: x**T(2), T(2)),
                        stop=3)) == [T(4), T(16), T(256)]


def test_applyfunc_normal2():
    assert list(getitem(applyfunc(lambda x: x, T(2)),
                        stop=3)) == [T(2), T(2), T(2)]


def test_applyfunc_failure1():
    with pytest.raises(TypeError):
        list(getitem(applyfunc(lambda x: x**T(2), T('a')), stop=3))


def test_applyfunc_attributes1():
    it = applyfunc(iteration_utilities.square, 2)
    assert it.func is iteration_utilities.square
    assert it.current == 2


def test_applyfunc_failure2():
    # Too few arguments
    with pytest.raises(TypeError):
        applyfunc(bool)


def test_applyfunc_copy1():
    iterator_copy(applyfunc(lambda x: x**T(2), T(2)))


def test_applyfunc_pickle1(protocol):
    apf = applyfunc(iteration_utilities.square, T(2))
    assert next(apf) == T(4)
    x = pickle.dumps(apf, protocol=protocol)
    assert next(pickle.loads(x)) == T(16)
