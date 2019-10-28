# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

import iteration_utilities
from iteration_utilities import ItemIdxKey

from helper_cls import T


def test_itemidxkey_repr1():
    # Just make sure the representation does not fail.
    assert repr(ItemIdxKey(T(10), 2))
    assert repr(ItemIdxKey(T(10), 2, T(10)))


def test_itemidxkey_repr2():
    # Just make sure the representation uses the class name
    class Fun(ItemIdxKey):
        pass

    assert 'Fun' in repr(Fun(T(10), 2))
    assert 'Fun' in repr(Fun(T(10), 2, T(10)))


def test_itemidxkey_repr3():
    # Just make sure the representation does not fail.
    iik = ItemIdxKey(10, 2)
    iik.item = [iik]
    assert repr(iik) == 'iteration_utilities.ItemIdxKey(item=[...], idx=2)'


def test_itemidxkey_repr4():
    # Make sure the representation does not segfault if the representation of
    # the item deletes the "key"...
    iik = ItemIdxKey([10, 11], 2, [50, 100])

    class DeleteKey:
        def __repr__(self):
            del iik.key
            return 'DeleteKey()'

    iik.item = DeleteKey()
    iik
    # asserting the representation isn't really the point, the point of this
    # test is that the representation doesn't segfault. However making sure the
    # representation is like that seems a "good idea".
    assert repr(iik) == ('iteration_utilities.ItemIdxKey(item=DeleteKey(), '
                         'idx=2, key=[50, 100])')


def test_itemidxkey_failure1():
    # Too few arguments
    with pytest.raises(TypeError):
        ItemIdxKey()


def test_itemidxkey_failure2():
    # item may not be an ItemIdxKey
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError) as exc:
        ItemIdxKey(iik, 2)
    assert "`ItemIdxKey`" in str(exc.value) and '`item`' in str(exc.value)


def test_itemidxkey_failure3():
    # key may not be an ItemIdxKey
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError) as exc:
        ItemIdxKey(T(10), 2, iik)
    assert "`ItemIdxKey`" in str(exc.value) and '`key`' in str(exc.value)


def test_itemidxkey_failure4():
    # Cannot use <= or >=, these make no sense with ItemIdxKey but if these
    # are implemented just remove that test.
    with pytest.raises(TypeError):
        ItemIdxKey(T(10), 2) >= ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError):
        ItemIdxKey(T(10), 2) <= ItemIdxKey(T(10), 2)


def test_itemidxkey_failure5():
    # Other argument in < and > must be another ItemIdxKey
    with pytest.raises(TypeError):
        ItemIdxKey(T(10), 2) < T(10)


def test_itemidxkey_failure6():
    # The item of the ItemIdxKey instances throws an Error when compared.
    with pytest.raises(TypeError, match='simulated failure'):
        ItemIdxKey(T(10), 2) < ItemIdxKey(T('a'), 2)


def test_itemidxkey_getter():
    iik = ItemIdxKey(T(10), 2)
    assert iik.item == T(10)
    assert iik.idx == 2
    with pytest.raises(AttributeError):
        iik.key

    iik = ItemIdxKey(T(10), 2, T(5))
    assert iik.item == T(10)
    assert iik.idx == 2
    assert iik.key == T(5)


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


def test_itemidxkey_setter_failure1():
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError):
        iik.idx = 'a'

    iik = ItemIdxKey(T(10), 2, T(5))
    with pytest.raises(TypeError):
        iik.idx = 'a'


def test_itemidxkey_setter_failure2():
    # cannot manually assign ItemIdxKey instance for item or key
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(TypeError) as exc:
        iik.item = iik
    assert "`ItemIdxKey`" in str(exc.value) and '`item`' in str(exc.value)

    iik = ItemIdxKey(T(10), 2, T(5))
    with pytest.raises(TypeError) as exc:
        iik.key = iik
    assert "`ItemIdxKey`" in str(exc.value) and '`key`' in str(exc.value)


def test_itemidxkey_deleter():
    iik = ItemIdxKey(T(10), 2)
    with pytest.raises(AttributeError):
        del iik.key
    with pytest.raises(AttributeError):
        iik.key

    iik = ItemIdxKey(T(10), 2, T(5))
    assert iik.key == T(5)
    del iik.key
    with pytest.raises(AttributeError):
        iik.key


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


def test_itemidxkey_pickle1(protocol):
    iik = ItemIdxKey(T(10), 2)
    x = pickle.dumps(iik, protocol=protocol)
    assert pickle.loads(x).item == T(10)
    assert pickle.loads(x).idx == 2


def test_itemidxkey_pickle2(protocol):
    iik = ItemIdxKey(T(10), 2, T(5))
    x = pickle.dumps(iik, protocol=protocol)
    assert pickle.loads(x).item == T(10)
    assert pickle.loads(x).idx == 2
    assert pickle.loads(x).key == T(5)
