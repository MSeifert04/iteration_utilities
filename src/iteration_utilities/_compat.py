# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import itertools

# This module
from ._utils import EQ_PY2, GE_PY35


__all__ = ['filter', 'filterfalse', 'map', 'UserString', 'range', 'zip',
           'zip_longest', 'string_types']

filter = itertools.ifilter if EQ_PY2 else filter
filterfalse = itertools.ifilterfalse if EQ_PY2 else itertools.filterfalse
map = itertools.imap if EQ_PY2 else map
range = xrange if EQ_PY2 else range
zip = itertools.izip if EQ_PY2 else zip
zip_longest = itertools.izip_longest if EQ_PY2 else itertools.zip_longest

string_types = basestring if EQ_PY2 else str

RecursionError = RecursionError if GE_PY35 else RuntimeError
AttributeUnwriteableException = TypeError if EQ_PY2 else AttributeError

if EQ_PY2:
    from UserString import UserString
else:
    from collections import UserString
