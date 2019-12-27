# Licensed under Apache License Version 2.0 - see LICENSE

import copy

import pytest

from iteration_utilities import Placeholder, partial

import helper_funcs as _hf

PlaceholderType = type(Placeholder)


def test_placeholder_only_one_instance():
    p1 = PlaceholderType()
    p2 = PlaceholderType()
    p1 is p2


def test_placeholder_pickle(protocol):
    p = PlaceholderType()
    assert _hf.round_trip_pickle(p, protocol=protocol) is p


def test_placeholder():
    assert partial._ is partial._
    assert copy.copy(partial._) is partial._
    assert copy.deepcopy(partial._) is partial._
    # PlaceholderType.__new__()
    assert type(partial._)() is partial._
    assert repr(partial._) == '_'


def test_placeholder_new():
    with pytest.raises(TypeError, match=r"_PlaceholderType\.__new__` takes no arguments"):
        type(partial._)(1)
    with pytest.raises(TypeError, match=r"_PlaceholderType\.__new__` takes no arguments"):
        type(partial._)(a=1)
