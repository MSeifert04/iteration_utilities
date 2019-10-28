# Licensed under Apache License Version 2.0 - see LICENSE

import doctest

import pytest

import iteration_utilities
from iteration_utilities import _iteration_utilities


@pytest.mark.parametrize("mod",
                         [iteration_utilities,
                          _iteration_utilities,
                          iteration_utilities._recipes,
                          iteration_utilities._additional_recipes,
                          iteration_utilities._classes,
                          ])
def test_doctests(mod):
    # classes are added to the main module code. :-)
    doctest.testmod(mod, raise_on_error=True)
