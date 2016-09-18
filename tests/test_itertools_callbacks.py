from __future__ import absolute_import, division, print_function
import doctest
import sys

import iteration_utilities


PY2 = sys.version_info.major == 2


def doctest_module_no_failure(module):
    assert doctest.testmod(module).failed == 0


def test_doctests():
    doctest_module_no_failure(iteration_utilities.callbacks.simplecallbacks)


def test_callbacks():
    assert iteration_utilities.returnTrue()
