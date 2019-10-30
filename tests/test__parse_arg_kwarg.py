# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities
from iteration_utilities import _iteration_utilities

# Test helper
from helper_cls import T


parse_args = _iteration_utilities._parse_args
parse_kwargs = _iteration_utilities._parse_kwargs
default = iteration_utilities._utils._default


def test_parseargs_normal1():
    assert parse_args((T(1), T(2), T(3)), T(4), 0) == (T(4), T(1), T(2), T(3))


def test_parseargs_normal2():
    assert parse_args((T(1), T(2), T(3)), T(4), 1) == (T(1), T(4), T(2), T(3))


def test_parseargs_normal3():
    assert parse_args((T(1), T(2), T(3)), T(4), 2) == (T(1), T(2), T(4), T(3))


def test_parseargs_normal4():
    assert parse_args((T(1), T(2), T(3)), T(4), 3) == (T(1), T(2), T(3), T(4))


def test_parseargs_empty1():
    assert parse_args(tuple(), T(1), 0) == (T(1),)


def test_parsekwargs_empty1():
    dct = {}
    parse_kwargs(dct, default)
    assert dct == {}


def test_parsekwargs_normal1():
    # One removed
    dct = {'a': 10, 'b': default}
    parse_kwargs(dct, default)
    assert dct == {'a': 10}


def test_parsekwargs_normal2():
    # No removed
    dct = {'a': 10, 'b': 20}
    parse_kwargs(dct, default)
    assert dct == {'a': 10, 'b': 20}


def test_parsekwargs_normal3():
    # All removed
    dct = {'a': default, 'b': default}
    parse_kwargs(dct, default)
    assert dct == {}


def test_parsekwargs_normal4():
    # Tests an implementation detail: For more than 5 elements it allocates an
    # array on the heap (for less elements in the dict it uses an array on the
    # stack).
    dct = {
        'a': default, 'b': default, 'c': default,
        'd': default, 'e': 1, 'f': 2
        }
    parse_kwargs(dct, default)
    assert dct == {'e': 1, 'f': 2}
