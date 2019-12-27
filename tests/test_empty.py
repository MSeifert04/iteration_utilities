# Licensed under Apache License Version 2.0 - see LICENSE

import operator

import pytest

from iteration_utilities import empty

import helper_funcs as _hf

EmptyType = type(empty)


def test_empty():
    assert list(empty) == []
    assert tuple(empty) == ()
    assert set(empty) == set()


def test_empty_type_construct():
    with pytest.raises(TypeError, match=r"_EmptyType\.__new__` takes no arguments"):
        EmptyType(1)
    with pytest.raises(TypeError, match=r"_EmptyType\.__new__` takes no arguments"):
        EmptyType(a=1)


def test_empty_only_one_instance():
    e1 = EmptyType()
    e2 = EmptyType()
    assert e1 is e2


def test_empty_length_hint():
    assert operator.length_hint(empty, -1) == 0


def test_empty_pickle(protocol):
    e = EmptyType()
    assert _hf.round_trip_pickle(e, protocol=protocol) is e
    assert list(_hf.round_trip_pickle(e, protocol=protocol)) == []


def test_empty_subiter_pickle(protocol):
    e = enumerate(EmptyType())
    assert list(_hf.round_trip_pickle(e, protocol=protocol)) == []
