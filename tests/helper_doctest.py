# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import doctest


def doctest_module_no_failure(module):
    doctest.testmod(module, raise_on_error=True)
