# Licensed under Apache License Version 2.0 - see LICENSE

"""
This module contains callable test cases.
"""
import abc
import copy
import operator
import pickle

import pytest

import iteration_utilities
from iteration_utilities._utils import IS_CPYTHON_PY_3_12, IS_PYPY, USES_VECTORCALL

from helper_cls import T


def _skipif_wrapper(func, condition, reason):
    return pytest.mark.skipif(condition, reason=reason)(func)


def skip_on_pypy_because_cache_next_works_differently(func):
    """Not sure what happens there but on PyPy CacheNext doesn't work like on
    CPython.
    """
    return _skipif_wrapper(func, IS_PYPY,
                           reason='PyPy works differently with __next__ cache.')


def skip_on_pypy_because_sizeof_makes_no_sense_there(func):
    """PyPy doesn't support sys.getsizeof().
    """
    return _skipif_wrapper(func, IS_PYPY,
                           reason='PyPy doesn\'t support sys.getsizeof().')


def skip_on_pypy_not_investigated_why(func):
    """PyPy failures - not sure why."""
    return _skipif_wrapper(func, IS_PYPY, reason='PyPy fails here.')


def skip_on_pypy_not_investigated_why_it_segfaults(func):
    """PyPy segfaults - not sure why."""
    return _skipif_wrapper(func, IS_PYPY, reason='PyPy segfaults here.')


def skip_if_vectorcall_is_not_used(func):
    """The vectorcall implementation imposes some additional restrictions that
    haven't been there before.
    """
    return _skipif_wrapper(func, not USES_VECTORCALL,
                           reason='pickle does not work with vectorcall')


def skip_if_not_latest_python(func):
    """If the tests are specifically targeted to the latest Python version
    """
    return _skipif_wrapper(func, not IS_CPYTHON_PY_3_12, reason='requires the latest Python version')


def iterator_copy(thing):
    """Normal copies are not officially supported but ``itertools.tee`` uses
    ``__copy__`` if implemented it is either forbid both or none. Given that
    ``itertools.tee`` is a very useful function ``copy.copy`` is allowed but
    no guarantees are made. This function just makes sure they can be copied
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


def check_lengthhint_iteration(iterator, expected_start_lengthhint):
    for length in range(expected_start_lengthhint, 0, -1):
        assert operator.length_hint(iterator) == length
        next(iterator)
    assert operator.length_hint(iterator) == 0
    with pytest.raises(StopIteration):
        next(iterator)


def round_trip_pickle(obj, protocol):
    tmp = pickle.dumps(obj, protocol=protocol)
    return pickle.loads(tmp)


# Helper classes for certain fail conditions. Bundled here so the tests don't
# need to re-implement them.


def CacheNext(item):
    """Iterator that modifies it "next" method when iterated over."""
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


class FailIter:
    """A class that fails when "iter" is called on it.

    This class is currently not interchangable with a real "iter(x)" failure
    because it raises another exception.
    """

    EXC_MSG = 'iter call failed'
    EXC_TYP = ValueError

    def __iter__(self):
        raise self.EXC_TYP(self.EXC_MSG)


class FailEqNoHash:
    """A class that fails when "==" is called on it."""
    EXC_MSG = 'eq call failed'
    EXC_TYP = ValueError
    __hash__ = None

    def __eq__(self, other):
        raise self.EXC_TYP(self.EXC_MSG)


class FailEqWithHash(FailEqNoHash):
    """A class that fails when "==" is called on it."""

    def __hash__(self):
        return 1


class FailHash:
    """A class that fails when "hash" is called on it."""
    EXC_MSG = 'hash call failed'
    EXC_TYP = ValueError

    def __hash__(self):
        raise self.EXC_TYP(self.EXC_MSG)


class FailBool:
    """A class that fails when "bool" is called on it."""
    EXC_MSG = 'bool call failed'
    EXC_TYP = ValueError

    def __bool__(self):
        raise self.EXC_TYP(self.EXC_MSG)


class FailNext:
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


class FailLengthHint:
    """Simple iterator that fails when length_hint is called on it."""

    EXC_MSG = "length_hint call failed"
    EXC_TYP = ValueError

    def __init__(self, it):
        self.it = iter(it)

    def __iter__(self):
        return self

    def __next__(self):
        return next(self.it)

    def __length_hint__(self):
        raise self.EXC_TYP(self.EXC_MSG)


class OverflowLengthHint:
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

    def __length_hint__(self):
        return self.lh


class FailingIsinstanceClass(metaclass=abc.ABCMeta):

    EXC_MSG = 'isinstance call failed'
    EXC_TYP = TypeError

    @classmethod
    def __subclasshook__(cls, C):
        raise cls.EXC_TYP(cls.EXC_MSG)
