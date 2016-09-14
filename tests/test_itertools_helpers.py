from __future__ import absolute_import, division, print_function
import doctest

import pytest

import iteration_utilities


def doctest_module_no_failure(module):
    assert doctest.testmod(module).failed == 0


def test_doctests():
    doctest_module_no_failure(iteration_utilities.helpers.performance)


def test_empty_input():
    empty = []
    assert iteration_utilities.all_isinstance(empty, float)
    assert not iteration_utilities.any_isinstance(empty, float)
