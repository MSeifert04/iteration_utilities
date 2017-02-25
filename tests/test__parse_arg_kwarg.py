# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


parse_args = iteration_utilities._cfuncs._parse_args
parse_kwargs = iteration_utilities._cfuncs._parse_kwargs
default = iteration_utilities.core._default


@memory_leak_decorator()
def test_parseargs_normal1():
    assert parse_args((T(1), T(2), T(3)), T(4), 0) == [T(4), T(1), T(2), T(3)]


@memory_leak_decorator()
def test_parseargs_normal2():
    assert parse_args((T(1), T(2), T(3)), T(4), 1) == [T(1), T(4), T(2), T(3)]


@memory_leak_decorator()
def test_parseargs_normal3():
    assert parse_args((T(1), T(2), T(3)), T(4), 2) == [T(1), T(2), T(4), T(3)]


@memory_leak_decorator()
def test_parseargs_normal4():
    assert parse_args((T(1), T(2), T(3)), T(4), 3) == [T(1), T(2), T(3), T(4)]


@memory_leak_decorator()
def test_parseargs_empty1():
    assert parse_args(tuple(), T(1), 0) == [T(1)]


@memory_leak_decorator()
def test_parsekwargs_empty1():
    dct = {}
    parse_kwargs(dct, default)
    assert dct == {}


@memory_leak_decorator()
def test_parsekwargs_normal1():
    # One removed
    dct = {'a': 10, 'b': default}
    parse_kwargs(dct, default)
    assert dct == {'a': 10}


@memory_leak_decorator()
def test_parsekwargs_normal2():
    # No removed
    dct = {'a': 10, 'b': 20}
    parse_kwargs(dct, default)
    assert dct == {'a': 10, 'b': 20}


@memory_leak_decorator()
def test_parsekwargs_normal3():
    # All removed
    dct = {'a': default, 'b': default}
    parse_kwargs(dct, default)
    assert dct == {}
