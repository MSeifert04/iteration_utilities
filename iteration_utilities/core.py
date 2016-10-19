"""
API: Chainable iteration_utilities
----------------------------------
"""
# Built-ins
from __future__ import absolute_import, division, print_function
from collections import OrderedDict
from itertools import (combinations, combinations_with_replacement,
                       compress, count, cycle,
                       dropwhile,
                       islice,
                       permutations,
                       repeat,
                       starmap,
                       takewhile)

# This module
from iteration_utilities import PY2, _default
from iteration_utilities import (accumulate, append, applyfunc,
                                 deepflatten,
                                 flatten,
                                 grouper,
                                 intersperse, itersubclasses, iter_except,
                                 ncycles,
                                 padnone, powerset, prepend,
                                 repeatfunc,
                                 split, successive,
                                 tabulate, tail,
                                 unique_everseen, unique_justseen)


# Conditional imports
if PY2:
    from itertools import (ifilter as filter,
                           imap as map,
                           ifilterfalse as filterfalse)
else:
    from itertools import filterfalse


__all__ = ['Iterable', 'InfiniteIterable']


def filterkwargs(**kwargs):
    return {k: kwargs[k] for k in kwargs if kwargs[k] is not _default}


class _Base(object):
    def __init__(self, iterable):
        self._iterable = iterable

    def __iter__(self):
        return iter(self._iterable)

    def __repr__(self):
        return '<{0.__class__.__name__} - {0._iterable!r}>'.format(self)

    def _call(self, *args, **kwargs):
        fn = args[0]
        pos = args[1]
        args = list(islice(args, 2, None))
        args.insert(pos, self)
        kwargs = filterkwargs(**kwargs)
        return self.__class__(fn(*args, **kwargs))

    def _call_infinite(self, *args, **kwargs):
        return InfiniteIterable(self._call(*args, **kwargs)._iterable)

    @staticmethod
    def from_count(start=_default, step=_default):
        """See :py:func:`itertools.count`."""
        return InfiniteIterable(count(**filterkwargs(start=start, step=step)))

    @staticmethod
    def from_repeat(object, times=_default):
        """See :py:func:`itertools.repeat`."""
        if times is not _default:
            return Iterable(repeat(object, times))
        else:
            return InfiniteIterable(repeat(object))

    @staticmethod
    def from_itersubclasses(object):
        """See
        :py:func:`~iteration_utilities._recipes._additional.itersubclasses`."""
        return Iterable(itersubclasses(object))

    @staticmethod
    def from_applyfunc(func, initial):
        """See :py:func:`~iteration_utilities.applyfunc`."""
        return InfiniteIterable(applyfunc(func, initial))

    @staticmethod
    def from_iterfunc_sentinel(func, sentinel):
        """See :py:func:`python:iter`."""
        return Iterable(iter(func, sentinel))

    @staticmethod
    def from_iterfunc_exception(func, exception, first=_default):
        """See :py:func:`~iteration_utilities.iter_except`."""
        return Iterable(iter_except(func, exception,
                                    **filterkwargs(first=first)))

    @staticmethod
    def from_repeatfunc(func, *args, **times):
        """See :py:func:`~iteration_utilities._recipes._core.repeatfunc`."""
        if times:
            return Iterable(repeatfunc(func, *args, **times))
        else:
            return InfiniteIterable(repeatfunc(func, *args))

    @staticmethod
    def from_tabulate(func, start=_default):
        """See :py:func:`~iteration_utilities._recipes._core.tabulate`."""
        return InfiniteIterable(tabulate(func, **filterkwargs(start=start)))

    def accumulate(self, func=_default, start=_default):
        """See :py:func:`~iteration_utilities.accumulate`."""
        return self._call(accumulate, 0, func=func, start=start)

    def append(self, element):
        """See :py:func:`~iteration_utilities._recipes._additional.append`."""
        return self._call(append, 1, element)

    def combinations(self, r):
        """See :py:func:`itertools.combinations`."""
        return self._call(combinations, 0, r)

    def combinations_with_replacement(self, r):
        """See :py:func:`itertools.combinations_with_replacement`."""
        return self._call(combinations_with_replacement, 0, r)

    def compress(self, selectors):
        """See :py:func:`itertools.compress`."""
        return self._call(compress, 0, selectors)

    def cycle(self):
        """See :py:func:`itertools.cycle`."""
        return self._call_infinite(cycle, 0)

    def deepflatten(self, depth=_default, types=_default, ignore=_default):
        """See
        :py:func:`~iteration_utilities._recipes._additional.deepflatten`."""
        return self._call(deepflatten, 0, depth=depth, types=types,
                          ignore=ignore)

    def dropwhile(self, predicate):
        """See :py:func:`itertools.dropwhile`."""
        return self._call(dropwhile, 1, predicate)

    def enumerate(self):
        """See :py:func:`python:enumerate`."""
        return self._call(enumerate, 0)

    def filter(self, function):
        """See :py:func:`python:filter`."""
        return self._call(filter, 1, function)

    def filterfalse(self, predicate):
        """See :py:func:`itertools.filterfalse`."""
        return self._call(filterfalse, 1, predicate)

    def flatten(self):
        """See :py:func:`~iteration_utilities._recipes._core.flatten`."""
        return self._call(flatten, 0)

    def grouper(self, n, fillvalue=_default, truncate=_default):
        """See :py:func:`~iteration_utilities.grouper`."""
        return self._call(grouper, 0, n, fillvalue=fillvalue,
                          truncate=truncate)

    def islice(self, *args, **kwargs):
        """See :py:func:`itertools.islice`."""
        # TODO: If stop is given this could transform an infinite to finite
        #       iterable.
        return self._call(islice, 0, *args, **kwargs)

    def intersperse(self, e):
        """See :py:func:`~iteration_utilities.intersperse`."""
        return self._call(intersperse, 0, e)

    def map(self, function):
        """See :py:func:`python:map`."""
        return self._call(map, 1, function)

    def ncycles(self, n):
        """See :py:func:`~iteration_utilities._recipes._core.ncycles`."""
        return self._call(ncycles, 0, n)

    def padnone(self):
        """See :py:func:`~iteration_utilities._recipes._core.padnone`."""
        return self._call_infinite(padnone, 0)

    def permutations(self, r=_default):
        """See :py:func:`itertools.permutations`."""
        return self._call(permutations, 0, r=r)

    def powerset(self):
        """See :py:func:`~iteration_utilities._recipes._core.powerset`."""
        return self._call(powerset, 0)

    def prepend(self, element):
        """See :py:func:`~iteration_utilities._recipes._additional.prepend`."""
        return self._call(prepend, 1, element)

    def split(self, key, maxsplit=_default, keep=_default, eq=_default):
        """See :py:func:`~iteration_utilities.split`."""
        return self._call(split, 0, key, maxsplit=maxsplit, keep=keep, eq=eq)

    def starmap(self, function):
        """See :py:func:`itertools.starmap`."""
        return self._call(starmap, 1, function)

    def successive(self, times):
        """See :py:func:`~iteration_utilities.successive`."""
        return self._call(successive, 0, times)

    def tail(self, n):
        """See :py:func:`~iteration_utilities._recipes._core.tail`."""
        return self._call(tail, 0, n)

    def takewhile(self, predicate):
        """See :py:func:`itertools.takewhile`."""
        return self._call(takewhile, 1, predicate)

    def unique_everseen(self, key=_default):
        """See :py:func:`~iteration_utilities.unique_everseen`."""
        return self._call(unique_everseen, 0, key=key)

    def unique_justseen(self, key=_default):
        """See :py:func:`~iteration_utilities.unique_justseen`."""
        return self._call(unique_justseen, 0, key=key)


class Iterable(_Base):
    # TODO: Needs class documentation
    def as_(self, cls):
        """Convert `Iterable` to other class.

        Parameters
        ----------
        cls : type
            Convert the content of `Iterable` to this class.

        Returns
        -------
        iterable : cls
            The `Iterable` as `cls`.

        Notes
        -----
        Be careful if you use this method because the `Iterable` may be
        infinite.
        """
        return cls(self._iterable)

    @property
    def as_list(self):
        """See :py:meth:`as_`."""
        return self.as_(list)

    @property
    def as_tuple(self):
        """See :py:meth:`as_`."""
        return self.as_(tuple)

    @property
    def as_set(self):
        """See :py:meth:`as_`."""
        return self.as_(set)

    @property
    def as_frozen_set(self):
        """See :py:meth:`as_`."""
        return self.as_(frozenset)

    @property
    def as_dict(self):
        """See :py:meth:`as_`."""
        return self.as_(dict)

    @property
    def as_ordered_dict(self):
        """See :py:meth:`as_`."""
        return self.as_(OrderedDict)

    def reversed(self):
        """See :py:func:`python:reversed`.

        ..warning::
           This method requires that the `Iterable` is a `Sequence` or
           implements the `__reversed__` method. Generally this does not work
           with generators!
        """
        return self._call(reversed, 0)


class InfiniteIterable(_Base):
    # TODO: Needs class documentation
    pass
