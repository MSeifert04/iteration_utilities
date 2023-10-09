# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
import platform
import sys

__all__ = ['IS_CPYTHON', 'IS_CPYTHON_PY_3_12', 'IS_PYPY', 'USES_VECTORCALL', '_default']


_GE_PY38 = sys.version_info.major > 3 or (sys.version_info.major == 3 and
                                         sys.version_info.minor >= 8)
IS_CPYTHON_PY_3_12 = sys.version_info.major == 3 and sys.version_info.minor == 12
IS_PYPY = platform.python_implementation() == 'PyPy'
IS_CPYTHON = platform.python_implementation() == 'CPython'

USES_VECTORCALL = IS_CPYTHON and _GE_PY38


class _SentinelFactory:
    __slots__ = ('_name', )

    def __init__(self, name):
        self._name = name

    # Representation and casting to strings
    def __repr__(self):
        return str(self._name)

    def __str__(self):
        return str(self._name)


_default = _SentinelFactory('<default>')
