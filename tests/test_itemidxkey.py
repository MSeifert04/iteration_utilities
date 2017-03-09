# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


ItemIdxKey = iteration_utilities.ItemIdxKey


@memory_leak_decorator()
def test_itemidxkey_repr1():
    # Just make sure the representation does not fail.
    assert repr(ItemIdxKey(T(10), 2))
    assert repr(ItemIdxKey(T(10), 2, T(10)))


@memory_leak_decorator(collect=True)
def test_itemidxkey_repr2():
    # Just make sure the representation does not fail.
    iik = ItemIdxKey(10, 2)
    iik.item = [iik]
    assert repr(iik) == 'ItemIdxKey(item=[ItemIdxKey(...)], idx=2)'


@memory_leak_decorator(collect=True)
def test_itemidxkey_failure1():
    # Too few arguments
    with pytest.raises(TypeError):
        ItemIdxKey()


@memory_leak_decorator(collect=True)
def test_itemidxkey_failure2():
    # item may not be an ItemIdxKey
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError) as exc:
        ItemIdxKey(iik, 2)
    assert "cannot use `ItemIdxKey` instance as `item`." in str(exc)


@memory_leak_decorator(collect=True)
def test_itemidxkey_failure3():
    # key may not be an ItemIdxKey
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError) as exc:
        ItemIdxKey(T(10), 2, iik)
    assert "cannot use `ItemIdxKey` instance as `key`." in str(exc)


@memory_leak_decorator(collect=True)
def test_itemidxkey_failure4():
    # Cannot use <= or >=, these make no sense with ItemIdxKey but if these
    # are implemented just remove that test.
    with pytest.raises(TypeError):
        ItemIdxKey(T(10), 2) >= ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError):
        ItemIdxKey(T(10), 2) <= ItemIdxKey(T(10), 2)


@memory_leak_decorator(collect=True)
def test_itemidxkey_failure5():
    # Other argument in < and > must be another ItemIdxKey
    with pytest.raises(TypeError):
        ItemIdxKey(T(10), 2) < T(10)


@memory_leak_decorator(collect=True)
def test_itemidxkey_failure6():
    # The item of the ItemIdxKey instances throws an Error when compared.
    with pytest.raises(TypeError) as exc:
        ItemIdxKey(T(10), 2) < ItemIdxKey(T('a'), 2)
    assert 'simulated failure.' in str(exc)


@memory_leak_decorator()
def test_itemidxkey_getter():
    iik = ItemIdxKey(T(10), 2)
    assert iik.item == T(10)
    assert iik.idx == 2
    assert iik.key is None

    iik = ItemIdxKey(T(10), 2, T(5))
    assert iik.item == T(10)
    assert iik.idx == 2
    assert iik.key == T(5)


@memory_leak_decorator()
def test_itemidxkey_setter():
    iik = ItemIdxKey(T(10), 2)
    iik.item = T(20)
    assert iik.item == T(20)
    iik.idx = 10
    assert iik.idx == 10
    iik.key = T(0)
    assert iik.key == T(0)

    iik = ItemIdxKey(T(10), 2, T(5))
    iik.item = T(20)
    assert iik.item == T(20)
    iik.idx = 10
    assert iik.idx == 10
    iik.key = T(0)
    assert iik.key == T(0)


@memory_leak_decorator(collect=True)
def test_itemidxkey_setter_failure1():
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError):
        iik.idx = 'a'

    iik = ItemIdxKey(T(10), 2, T(5))
    with pytest.raises(TypeError):
        iik.idx = 'a'


@memory_leak_decorator(collect=True)
def test_itemidxkey_setter_failure2():
    # cannot manually assign ItemIdxKey instance for item or key
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError) as exc:
        iik.item = iik
    assert 'cannot use `ItemIdxKey` instance as `item`.' in str(exc)

    iik = ItemIdxKey(T(10), 2, T(5))
    with pytest.raises(TypeError) as exc:
        iik.key = iik
    assert 'cannot use `ItemIdxKey` instance as `key`.' in str(exc)


@memory_leak_decorator()
def test_itemidxkey_deleter():
    iik = ItemIdxKey(T(10), 2)
    del iik.key
    assert iik.key is None

    iik = ItemIdxKey(T(10), 2, T(5))
    del iik.key
    assert iik.key is None


@memory_leak_decorator(collect=True)
def test_itemidxkey_deleter_failure():
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError):
        del iik.item
    with pytest.raises(TypeError):
        del iik.idx

    iik = ItemIdxKey(T(10), 2, T(5))
    with pytest.raises(TypeError):
        del iik.item
    with pytest.raises(TypeError):
        del iik.idx


@memory_leak_decorator(offset=1)
def test_itemidxkey_pickle1():
    iik = ItemIdxKey(T(10), 2)
    x = pickle.dumps(iik)
    assert pickle.loads(x).item == T(10)
    assert pickle.loads(x).idx == 2


@memory_leak_decorator(offset=1)
def test_itemidxkey_pickle2():
    iik = ItemIdxKey(T(10), 2, T(5))
    x = pickle.dumps(iik)
    assert pickle.loads(x).item == T(10)
    assert pickle.loads(x).idx == 2
    assert pickle.loads(x).key == T(5)
