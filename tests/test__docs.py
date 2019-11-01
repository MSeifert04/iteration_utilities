# Licensed under Apache License Version 2.0 - see LICENSE

import doctest

import pytest

import iteration_utilities
from iteration_utilities import _iteration_utilities
from iteration_utilities._utils import GE_PY36, GE_PY38


# Some of the methods on Iterable don't work under specific conditions.
# To prevent doctest failures these methods are removed during test-collection.
# This isn't very nice but it should work.
_delete_methods = []
if not GE_PY38:
    _delete_methods += ['get_fmean', 'get_quantiles', 'get_multimode', 'get_geometric_mean']
if not GE_PY36:
    _delete_methods += ['get_harmonic_mean']
for _methodname in _delete_methods:
    delattr(iteration_utilities.Iterable, _methodname)


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
