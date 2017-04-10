# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins

# This module
import iteration_utilities
from iteration_utilities._compat import map


class T(object):
    def __init__(self, value):
        self.value = value

    def _cmp_cls_and_value(self, other):
        if (type(self) != type(other) or
                type(self.value) != type(other.value)):
            raise TypeError('simulated failure.')

    # Misc
    def __hash__(self):
        return hash(self.value)

    def __bool__(self):
        return bool(self.value)

    def __nonzero__(self):
        return bool(self.value)

    def __len__(self):
        return len(self.value)

    def __repr__(self):
        return '{0.__class__.__name__}({0.value})'.format(self)

    # Mathematical
    def __add__(self, other):
        self._cmp_cls_and_value(other)
        return self.__class__(self.value + other.value)

    def __mul__(self, other):
        self._cmp_cls_and_value(other)
        return self.__class__(self.value * other.value)

    def __rtruediv__(self, other):
        return self.__class__(other / self.value)

    def __pow__(self, other):
        if isinstance(other, T):
            return self.__class__(self.value**other.value)
        else:
            return self.__class__(self.value**other)

    def __abs__(self):
        return self.__class__(abs(self.value))

    # Comparisons
    def __eq__(self, other):
        self._cmp_cls_and_value(other)
        return self.value == other.value

    def __lt__(self, other):
        self._cmp_cls_and_value(other)
        return self.value < other.value

    def __le__(self, other):
        self._cmp_cls_and_value(other)
        return self.value <= other.value

    def __gt__(self, other):
        self._cmp_cls_and_value(other)
        return self.value > other.value

    def __ge__(self, other):
        self._cmp_cls_and_value(other)
        return self.value >= other.value


def toT(iterable):
    """Convenience to create a normal list to a list of `T` instances."""
    return list(map(T, iterable))


# Helper classes for certain fail conditions. Bundled here so the tests don't
# need to reimplement them.


class FailNext(object):
    """An iterator that fails when calling "next" on it.

    The parameter "offset" can be used to set the number of times "next" works
    before it raises an exception.
    """

    EXC_MSG = 'next call failed'
    EXC_TYP = ValueError

    def __init__(self, offset=0, repeats=1):
        self.offset = offset
        self.repeats = repeats

    def __iter__(self):
        return self

    def __next__(self):
        if self.offset:
            self.offset -= 1
            return T(1)
        else:
            raise self.EXC_TYP(self.EXC_MSG)

    next = __next__  # python 2.x compatibility


class FailLengthHint(object):
    """Simple iterator that fails when length_hint is called on it."""

    EXC_MSG = "length_hint call failed"
    EXC_TYP = ValueError

    def __init__(self, it):
        self.it = iter(it)

    def __iter__(self):
        return self

    def __next__(self):
        return next(self.it)

    next = __next__  # python 2.x compatibility

    def __length_hint__(self):
        raise self.EXC_TYP(self.EXC_MSG)


class OverflowLengthHint(object):
    """Simple iterator that allows to set a length_hint so that one can test
    overflow in PyObject_LengthHint.

    Should be used together with "sys.maxsize" so it works on 32bit and 64bit
    builds.
    """
    def __init__(self, it, length_hint):
        self.it = iter(it)
        self.lh = length_hint

    def __iter__(self):
        return self

    def __next__(self):
        return next(self.it)

    next = __next__  # python 2.x compatibility

    def __length_hint__(self):
        return self.lh


if iteration_utilities.EQ_PY2:
    exec("""
import abc

class FailingIsinstanceClass:
    __metaclass__ = abc.ABCMeta

    EXC_MSG = 'isinstance call failed'
    EXC_TYP = TypeError

    @classmethod
    def __subclasshook__(cls, C):
        raise cls.EXC_TYP(cls.EXC_MSG)
""")
else:
    exec("""
import abc

class FailingIsinstanceClass(metaclass=abc.ABCMeta):

    EXC_MSG = 'isinstance call failed'
    EXC_TYP = TypeError

    @classmethod
    def __subclasshook__(cls, C):
        raise cls.EXC_TYP(cls.EXC_MSG)
""")
