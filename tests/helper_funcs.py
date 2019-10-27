# Licensed under Apache License Version 2.0 - see LICENSE

"""
This module contains callable test cases.
"""
# Built-ins
import copy

# 3rd party
import pytest

# This module
import iteration_utilities
from iteration_utilities._compat import filter

# helper
from helper_cls import T


def _skipif_wrapper(func, condition, reason):
    return pytest.mark.skipif(condition, reason=reason)(func)


def skip_because_iterators_cannot_be_pickled_before_py3(func):
    """Most iterators like list-iterator, map, zip, ... can not be pickled in
    Python 2.x.
    """
    return _skipif_wrapper(func, iteration_utilities.EQ_PY2,
                           reason='pickle does not work on Python 2')


def skip_before_py34_because_length_hint_was_added_in_py34(func):
    """Support for __length_hint__ was added in Python 3.4.
    """
    return _skipif_wrapper(func, not iteration_utilities.GE_PY34,
                           reason='length_hint does not work before Python 3.4')


def skip_before_py34_because_method_descriptors_cannot_be_pickled(func):
    """Pickling a method descriptor is not possible for Python 3.3 and before
    Also ``operator.methodcaller`` loses it's method name when pickled for
    Python 3.4 and lower.
    """
    return _skipif_wrapper(func, not iteration_utilities.GE_PY34,
                           reason='pickle does not work before Python 3.4'
                                  ' on method descriptors')


def skip_on_pypy_because_cache_next_works_differently(func):
    """Not sure what happens there but on PyPy CacheNext doesn't work like on
    CPython.
    """
    return _skipif_wrapper(func, iteration_utilities.IS_PYPY,
                           reason='PyPy works differently with __next__ cache.')


def skip_on_pypy_because_sizeof_makes_no_sense_there(func):
    """PyPy doesn't support sys.getsizeof().
    """
    return _skipif_wrapper(func, iteration_utilities.IS_PYPY,
                           reason='PyPy doesn\'t support sys.getsizeof().')


def skip_on_pypy_not_investigated_why(func):
    """PyPy failures - not sure why."""
    return _skipif_wrapper(func, iteration_utilities.IS_PYPY,
                           reason='PyPy fails here.')


def skip_on_pypy_not_investigated_why_it_segfaults(func):
    """PyPy segfaults - not sure why."""
    return _skipif_wrapper(func, iteration_utilities.IS_PYPY,
                           reason='PyPy segfaults here.')


def skip_if_vectorcall_is_not_used(func):
    """The vectorcall implementation imposes some additional restrictions that
    haven't been there before.
    """
    return _skipif_wrapper(func, not iteration_utilities.USES_VECTORCALL,
                           reason='pickle does not work with vectorcall')


def iterator_copy(thing):
    """Normal copies are not officially supported but ``itertools.tee`` uses
    ``__copy__`` if implemented it is either forbid both or none. Given that
    ``itertools.tee`` is a very useful function ``copy.copy`` is allowed but
    no garantuees are made. This function just makes sure they can be copied
    and the result has at least one item in it (call ``next`` on it)"""
    # Even though normal copies are discouraged they should be possible.
    # Cannot do "list" because it may be infinite :-)
    next(copy.copy(thing))


def iterator_setstate_list_fail(thing):
    with pytest.raises(TypeError) as exc:
        thing.__setstate__([])
    assert 'tuple' in str(exc.value) and 'list' in str(exc.value)


def iterator_setstate_empty_fail(thing):
    with pytest.raises(TypeError, match='0 given'):
        thing.__setstate__(())


# Helper classes for certain fail conditions. Bundled here so the tests don't
# need to re-implement them.


def CacheNext(item):
    """Iterator that modifies it "next" method when iterated over."""
    if iteration_utilities.EQ_PY2:
        def subiter():
            def newnext(self):
                raise CacheNext.EXC_TYP(CacheNext.EXC_MSG)
            Iterator.next = newnext
            yield item

        # Need to subclass a C iterator because only the "tp_iternext" slot is
        # cached, the "__next__" method itself always behaves as expected.
        class Iterator(filter):
            pass
    else:
        def subiter():
            def newnext(self):
                raise CacheNext.EXC_TYP(CacheNext.EXC_MSG)
            Iterator.__next__ = newnext
            yield item

        # Need to subclass a C iterator because only the "tp_iternext" slot is
        # cached, the "__next__" method itself always behaves as expected.
        class Iterator(filter):
            pass

    return Iterator(iteration_utilities.return_True, subiter())


CacheNext.EXC_MSG = 'next call failed, because it was modified'
CacheNext.EXC_TYP = ValueError


class FailIter(object):
    """A class that fails when "iter" is called on it.

    This class is currently not interchangable with a real "iter(x)" failure
    because it raises another exception.
    """

    EXC_MSG = 'iter call failed'
    EXC_TYP = ValueError

    def __iter__(self):
        raise self.EXC_TYP(self.EXC_MSG)


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
