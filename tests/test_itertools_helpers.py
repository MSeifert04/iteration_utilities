# Built-ins
from __future__ import absolute_import, division, print_function

# This module
import iteration_utilities

# Test helper
from helper_doctest import doctest_module_no_failure


def test_doctests():
    doctest_module_no_failure(iteration_utilities._helpers._performance)


def test_empty_input():
    empty = []
    assert iteration_utilities.all_isinstance(empty, float)
    assert not iteration_utilities.any_isinstance(empty, float)
