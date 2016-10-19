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


def get_iterable_list():
    return iteration_utilities.Iterable(list(range(1, 10)))


def get_iterable_tuple():
    return iteration_utilities.Iterable(tuple(range(1, 10)))


def get_iterable_generator():
    return iteration_utilities.Iterable((i for i in range(1, 10)))


@pytest.mark.parametrize("iterable", [get_iterable_list,
                                      get_iterable_tuple,
                                      get_iterable_generator])
def test_cls(iterable):
    assert iterable().accumulate().as_list == [1, 3, 6, 10, 15, 21, 28, 36, 45]
    assert iterable().append(10).as_list == [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
