# Built-ins
from __future__ import absolute_import, division, print_function

# This module
import iteration_utilities

# Test helper
from helper_doctest import doctest_module_no_failure


def test_doctests():
    doctest_module_no_failure(iteration_utilities._minmax)
    doctest_module_no_failure(iteration_utilities._returnx)
    doctest_module_no_failure(iteration_utilities._isx)
    doctest_module_no_failure(iteration_utilities._mathematical)


def test_callbacks():
    assert iteration_utilities.return_True()
    assert not iteration_utilities.return_False()
    assert iteration_utilities.return_None() is None

    assert iteration_utilities.square(2) == 4

    assert iteration_utilities.is_None(None)
    assert not iteration_utilities.is_None(False)

    assert iteration_utilities.return_first_positional_argument(1, 2, 3) == 1
