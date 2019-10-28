# Licensed under Apache License Version 2.0 - see LICENSE


class T:
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
