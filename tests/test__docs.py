# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import doctest

# 3rd party
import pytest

# This module
import iteration_utilities
import _iteration_utilities


@pytest.mark.parametrize("mod",
                         [iteration_utilities,
                          _iteration_utilities,
                          iteration_utilities._recipes,
                          iteration_utilities._additional_recipes,
                          iteration_utilities._classes,
                          iteration_utilities._performant_helpers,
                          ])
def test_doctests(mod):
    # classes are added to the main module code. :-)
    doctest.testmod(mod, raise_on_error=True)
