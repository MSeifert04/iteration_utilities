# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import platform
import sys

__all__ = ['EQ_PY2', 'GE_PY3', 'GE_PY34', 'GE_PY35', 'GE_PY38', 'IS_CPYTHON', 'IS_PYPY', 'USES_VECTORCALL', '_default']

EQ_PY2 = sys.version_info.major == 2
GE_PY3 = sys.version_info.major >= 3
GE_PY34 = sys.version_info.major > 3 or (sys.version_info.major == 3 and
                                         sys.version_info.minor >= 4)
GE_PY35 = sys.version_info.major > 3 or (sys.version_info.major == 3 and
                                         sys.version_info.minor >= 5)
GE_PY38 = sys.version_info.major > 3 or (sys.version_info.major == 3 and
                                         sys.version_info.minor >= 8)
IS_PYPY = platform.python_implementation() == 'PyPy'
IS_CPYTHON = platform.python_implementation() == 'CPython'

USES_VECTORCALL = IS_CPYTHON and GE_PY38


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
