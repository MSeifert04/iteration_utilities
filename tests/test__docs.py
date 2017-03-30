# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party

# This module
import iteration_utilities

# Test helper
from helper_doctest import doctest_module_no_failure


def test_doctests():
    # classes are added to the main module code. :-)
    doctest_module_no_failure(iteration_utilities)
    doctest_module_no_failure(iteration_utilities._cfuncs)
