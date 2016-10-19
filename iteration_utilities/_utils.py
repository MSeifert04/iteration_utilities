# Built-ins
from __future__ import absolute_import, division, print_function
import sys

__all__ = ['PY2', 'PY3', 'PY34', '_default']

PY2 = sys.version_info.major == 2
PY3 = sys.version_info.major == 3
PY34 = PY3 and sys.version_info.minor >= 4


string_types = basestring if PY2 else str


class _SentinelFactory(object):
    __slots__ = ('_name', )

    def __init__(self, name):
        if not isinstance(name, string_types):
            raise TypeError('name must be a string and not '
                            'a "{0}"'.format(name.__class__.__name__))
        object.__setattr__(self, '_name', name)

    # Representation and casting to strings
    def __repr__(self):
        return self._name

    def __str__(self):
        return self._name

    # The sentinal should evaluate to False
    # Python3
    def __bool__(self):
        return False

    # Python2
    def __nonzero__(self):
        return False

    # No copy of instances, just return self
    def __copy__(self):
        return self

    def __deepcopy__(self, memo):
        return self

    # TODO: Probably with pickling it could be modified. No need to do that
    # just now...

    # The name attribute should be unchangable
    def __setattr__(self, *args, **kwargs):
        raise TypeError('{0} cannot be modified.'
                        ''.format(self.__class__.__name__))

    def __delattr__(self, *args, **kwargs):
        raise TypeError('{0} cannot be modified.'
                        ''.format(self.__class__.__name__))


_default = _SentinelFactory('<default>')
