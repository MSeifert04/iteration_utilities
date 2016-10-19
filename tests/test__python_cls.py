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


def test_cls_exception():
    with pytest.raises(TypeError):
        Iterable.from_count().padnone()
