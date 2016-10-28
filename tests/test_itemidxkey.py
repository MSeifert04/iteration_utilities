# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak


ItemIdxKey = iteration_utilities.ItemIdxKey


class T(object):
    def __init__(self, value):
        self.value = value


def test_itemidxkey_getter():
    iik = ItemIdxKey(10, 2)
    assert iik.item == 10
    assert iik.idx == 2
    assert iik.key is None

    iik = ItemIdxKey(10, 2, 5)
    assert iik.item == 10
    assert iik.idx == 2
    assert iik.key == 5

    def test():
        iik = ItemIdxKey(T(10), 2)
        i = iik.item
        j = iik.idx
        k = iik.key

        iik = ItemIdxKey(T(10), 2, T(10))
        i = iik.item
        j = iik.idx
        k = iik.key
    assert not memory_leak(test)


def test_itemidxkey_setter():
    iik = ItemIdxKey(10, 2)
    iik.item = 20
    assert iik.item == 20
    iik.idx = 10
    assert iik.idx == 10
    iik.key = 0
    assert iik.key == 0

    iik = ItemIdxKey(10, 2, 5)
    iik.item = 20
    assert iik.item == 20
    iik.idx = 10
    assert iik.idx == 10
    iik.key = 0
    assert iik.key == 0

    def test():
        iik = ItemIdxKey(T(10), 2)
        iik.item = T(20)
        iik.idx = 10
        iik.key = T(0)
        i = iik.item
        j = iik.idx
        k = iik.key

        iik = ItemIdxKey(T(10), 2, T(10))
        iik.item = T(20)
        iik.idx = 10
        iik.key = T(0)
        i = iik.item
        j = iik.idx
        k = iik.key
    assert not memory_leak(test)


def test_itemidxkey_setter_failure():
    iik = ItemIdxKey(10, 2)
    with pytest.raises(TypeError):
        iik.idx = 'a'

    iik = ItemIdxKey(10, 2, 5)
    with pytest.raises(TypeError):
        iik.idx = 'a'

    def test():
        iik = ItemIdxKey(T(10), 2)
        with pytest.raises(TypeError):
            iik.idx = 'a'

        iik = ItemIdxKey(T(10), 2, T(10))
        with pytest.raises(TypeError):
            iik.idx = 'a'
    assert not memory_leak(test)


def test_itemidxkey_deleter():
    iik = ItemIdxKey(10, 2)
    del iik.key
    assert iik.key is None

    iik = ItemIdxKey(10, 2, 5)
    del iik.key
    assert iik.key is None

    def test():
        iik = ItemIdxKey(T(10), 2)
        del iik.key
        k = iik.key

        iik = ItemIdxKey(T(10), 2, T(10))
        del iik.key
        k = iik.key
    assert not memory_leak(test)


def test_itemidxkey_deleter_failure():
    iik = ItemIdxKey(10, 2)
    with pytest.raises(TypeError):
        del iik.item
    with pytest.raises(TypeError):
        del iik.idx

    iik = ItemIdxKey(10, 2, 5)
    with pytest.raises(TypeError):
        del iik.item
    with pytest.raises(TypeError):
        del iik.idx

    def test():
        iik = ItemIdxKey(T(10), 2)
        with pytest.raises(TypeError):
            del iik.item
        with pytest.raises(TypeError):
            del iik.idx

        iik = ItemIdxKey(T(10), 2, T(10))
        with pytest.raises(TypeError):
            del iik.item
        with pytest.raises(TypeError):
            del iik.idx
    assert not memory_leak(test)


def test_itemidxkey_pickle1():
    iik = ItemIdxKey(10, 2)
    x = pickle.dumps(iik)
    assert pickle.loads(x).item == 10
    assert pickle.loads(x).idx == 2

    def test():
        iik = ItemIdxKey(T(10), 2)
        x = pickle.dumps(iik)
        y = pickle.loads(x)
    memory_leak(test)
    assert not memory_leak(test)


def test_itemidxkey_pickle2():
    iik = ItemIdxKey(10, 2, 11)
    x = pickle.dumps(iik)
    assert pickle.loads(x).item == 10
    assert pickle.loads(x).idx == 2
    assert pickle.loads(x).key == 11

    def test():
        iik = ItemIdxKey(T(10), 2, T(11))
        x = pickle.dumps(iik)
        y = pickle.loads(x)
    assert not memory_leak(test)
