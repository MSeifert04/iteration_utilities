# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_doctest import doctest_module_no_failure


def test_doctests():
    doctest_module_no_failure(iteration_utilities.core)


Iterable = iteration_utilities.Iterable


def test_sentinelfactory():
    as_str = str(iteration_utilities._default)
    as_repr = repr(iteration_utilities._default)
    assert as_str == as_repr
    assert as_str == "<default>"


def test_cls_exception():
    with pytest.raises(TypeError):
        Iterable.from_count().pad(ntail=None)

    # __getitem__ : negative idx
    with pytest.raises(ValueError):
        Iterable(range(10))[-2]

    # __getitem__ : negative step
    with pytest.raises(ValueError):
        Iterable(range(10))[::-2]

    # __getitem__ : positive start with negative stop
    with pytest.raises(ValueError):
        Iterable(range(10))[2:-1]

    # __getitem__ : negative start/stop with infinite iterable.
    with pytest.raises(TypeError):
        Iterable.from_count()[-5:-3]

    # __getitem__ : not int, not slice
    with pytest.raises(TypeError):
        Iterable.from_count()['bad']
