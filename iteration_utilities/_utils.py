# Licensed under Apache License Version 2.0 - see LICENSE.rst

# Built-ins
from __future__ import absolute_import, division, print_function
import sys

__all__ = ['PY2', 'PY3', 'PY34', '_default']

PY2 = sys.version_info.major == 2
PY3 = sys.version_info.major == 3
PY34 = PY3 and sys.version_info.minor >= 4


class _SentinelFactory(object):
    __slots__ = ('_name', )

    def __init__(self, name):
        self._name = name

    # Representation and casting to strings
    def __repr__(self):
        return str(self._name)

    def __str__(self):
        return str(self._name)


_default = _SentinelFactory('<default>')
