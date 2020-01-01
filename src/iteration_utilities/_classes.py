# Licensed under Apache License Version 2.0 - see LICENSE

"""
API: Chainable iteration_utilities
----------------------------------
"""

# Built-ins
from collections import Counter, OrderedDict
from functools import reduce
from heapq import nlargest, nsmallest
from itertools import (chain, combinations, combinations_with_replacement,
                       compress, count, cycle,
                       dropwhile,
                       filterfalse,
                       islice,
                       permutations, product,
                       repeat,
                       starmap,
                       takewhile,
                       zip_longest)
from math import fsum
from operator import length_hint
import statistics

# This module
from iteration_utilities._utils import _default, GE_PY38, GE_PY36
# - generators
from iteration_utilities import (accumulate, always_iterable, applyfunc,
                                 clamp,
                                 deepflatten, duplicates,
                                 empty,
                                 flatten,
                                 getitem, grouper,
                                 insert, intersperse, itersubclasses,
                                 iter_except,
                                 ncycles,
                                 pad, powerset,
                                 remove, repeatfunc, replace, replicate,
                                 split, starfilter, successive,
                                 tabulate, tail,
                                 unique_everseen, unique_justseen)
# - folds
from iteration_utilities import (all_distinct, all_equal, all_monotone,
                                 argmax, argmin, argsorted,
                                 count_items, first, groupedby, last,
                                 minmax, nth, one, partition, second,
                                 third)
# - multiple_iterables
from iteration_utilities import merge, roundrobin
# - helper
from iteration_utilities import all_isinstance, any_isinstance
# - private helpers (must be imported from the private module!!!)
from ._iteration_utilities import _parse_args, _parse_kwargs


__all__ = ['Iterable', 'InfiniteIterable', 'ManyIterables']


class _Base:
    """Base class for method definitions that are shared by
    :py:class:`.Iterable` and :py:class:`.InfiniteIterable`.
    """
    __slots__ = ('_iterable',)

    def __init__(self, iterable):
        self._iterable = iterable

    def __iter__(self):
        return iter(self._iterable)

    def __getitem__(self, idx):
        """see `get`."""
        if isinstance(idx, (int, tuple, list)):
            return getitem(self._iterable, idx=idx)
        elif isinstance(idx, slice):
            if (isinstance(self, InfiniteIterable) and
                    any(x is not None and x < 0
                        for x in [idx.start, idx.stop, idx.step])):
                raise TypeError('subscripting InfiniteIterables requires '
                                '"start", "stop" and "step" to be positive '
                                'integers or None.')

            if idx.stop is not None and idx.stop > 0:
                meth = self._call_finite
            else:
                meth = self._call
            return meth(getitem, 0, start=idx.start, stop=idx.stop,
                        step=idx.step)
        raise TypeError('can only subscript {0} with integers and slices.'
                        ''.format(self.__class__.__name__))

    def __repr__(self):
        return '<{0.__class__.__name__}: {0._iterable!r}>'.format(self)

    def _call(self, fn, pos, *args, **kwargs):
        args = _parse_args(args, self._iterable, pos)
        _parse_kwargs(kwargs, _default)
        return self.__class__(fn(*args, **kwargs))

    def _call_finite(self, *args, **kwargs):
        res = self._call(*args, **kwargs)
        if isinstance(res, Iterable):
            return res
        return Iterable(res._iterable)

    def _call_infinite(self, *args, **kwargs):
        res = self._call(*args, **kwargs)
        if isinstance(res, InfiniteIterable):
            # There is no use-case to wrap an already infinite iterable with
            # something that newly creates an infinite iterable.
            # For example cycle(count()) makes no sense because we never end
            # with count so cycle never triggers.
            # That may change but I found no useful combination so there is
            # this Exception.
            raise TypeError('impossible to wrap an infinite iterable with '
                            'another infinite iterable.')
        return InfiniteIterable(res._iterable)

    @staticmethod
    def from_count(start=_default, step=_default):
        """See :py:func:`itertools.count`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable.from_count().islice(10).as_list()
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> Iterable.from_count(4, 3).islice(10).as_list()
        [4, 7, 10, 13, 16, 19, 22, 25, 28, 31]

        >>> Iterable.from_count(start=4, step=3).islice(10).as_list()
        [4, 7, 10, 13, 16, 19, 22, 25, 28, 31]

        .. warning::
           This returns an :py:class:`.InfiniteIterable`.
        """
        kwargs = {}
        if start is not _default:
            kwargs['start'] = start
        if step is not _default:
            kwargs['step'] = step
        return InfiniteIterable(count(**kwargs))

    @staticmethod
    def from_empty():
        """Creates an empty :py:class:`Iterable`.

        .. versionadded:: 0.11.0

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable.from_empty().as_list()
        []
        """
        return Iterable(empty)

    @staticmethod
    def from_maybe_iterable(obj, excluded_types=_default, empty_if_none=_default):
        """See :py:func:`~iteration_utilities.always_iterable`.

        .. versionadded:: 0.11.0

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable.from_maybe_iterable([1, 2, 3]).as_list()
        [1, 2, 3]

        >>> Iterable.from_maybe_iterable(1).as_list()
        [1]

        >>> Iterable.from_maybe_iterable([1, 2, 3], excluded_types=list).as_list()
        [[1, 2, 3]]

        >>> Iterable.from_maybe_iterable(None, empty_if_none=True).as_list()
        []
        """
        kwargs = {'excluded_types': excluded_types, 'empty_if_none': empty_if_none}
        _parse_kwargs(kwargs, _default)
        return Iterable(always_iterable(obj, **kwargs))

    @staticmethod
    def from_repeat(object, times=_default):
        """See :py:func:`itertools.repeat`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable.from_repeat(5).islice(10).as_list()
        [5, 5, 5, 5, 5, 5, 5, 5, 5, 5]

        >>> Iterable.from_repeat(5, 5).as_list()
        [5, 5, 5, 5, 5]

        >>> Iterable.from_repeat(object=5, times=5).as_list()
        [5, 5, 5, 5, 5]

        .. warning::
           This returns an :py:class:`.InfiniteIterable` if `times` is not
           given.
        """
        if times is not _default:
            return Iterable(repeat(object, times))
        else:
            return InfiniteIterable(repeat(object))

    @staticmethod
    def from_itersubclasses(object):
        """See \
:py:func:`~iteration_utilities.itersubclasses`.

        Examples
        --------
        >>> from iteration_utilities import Iterable

        >>> class A: pass
        >>> class B(A): pass
        >>> class C(A): pass
        >>> class D(C): pass

        >>> Iterable.from_itersubclasses(A).as_list()
        [<class 'iteration_utilities._classes.B'>, \
<class 'iteration_utilities._classes.C'>, \
<class 'iteration_utilities._classes.D'>]

        >>> Iterable.from_itersubclasses(C).as_list()
        [<class 'iteration_utilities._classes.D'>]
        """
        return Iterable(itersubclasses(object))

    @staticmethod
    def from_applyfunc(func, initial):
        """See :py:func:`~iteration_utilities.applyfunc`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable.from_applyfunc(lambda x: x*2, 10).islice(5).as_list()
        [20, 40, 80, 160, 320]

        >>> Iterable.from_applyfunc(func=lambda x: x*2,
        ...                         initial=10).islice(5).as_list()
        [20, 40, 80, 160, 320]

        .. warning::
           This returns an :py:class:`.InfiniteIterable`.
        """
        return InfiniteIterable(applyfunc(func=func, initial=initial))

    @staticmethod
    def from_iterfunc_sentinel(func, sentinel):
        """See :py:func:`python:iter`.

        Examples
        --------
        >>> from iteration_utilities import Iterable

        >>> class Func:
        ...     def __init__(self):
        ...         self.val = 0
        ...     def __call__(self):
        ...         self.val += 1
        ...         return 4 if self.val < 8 else 10

        >>> Iterable.from_iterfunc_sentinel(Func(), 10).as_list()
        [4, 4, 4, 4, 4, 4, 4]
        """
        # TODO: Update example to something useful
        return Iterable(iter(func, sentinel))

    @staticmethod
    def from_iterfunc_exception(func, exception, first=_default):
        """See :py:func:`~iteration_utilities.iter_except`.

        Examples
        --------
        >>> from iteration_utilities import Iterable

        >>> class Func:
        ...     def __init__(self):
        ...         self.val = 0
        ...     def setlim(self, val=3):
        ...         self.val = val
        ...         return 'init'
        ...     def __call__(self):
        ...         self.val += 1
        ...         if self.val < 8:
        ...             return 3
        ...         raise ValueError()

        >>> Iterable.from_iterfunc_exception(Func(), ValueError).as_list()
        [3, 3, 3, 3, 3, 3, 3]

        >>> f = Func()
        >>> Iterable.from_iterfunc_exception(f, ValueError, f.setlim).as_list()
        ['init', 3, 3, 3, 3]
        """
        if first is _default:
            return Iterable(iter_except(func, exception))
        else:
            return Iterable(iter_except(func, exception, first=first))

    @staticmethod
    def from_repeatfunc(func, *args, **times):
        """See :py:func:`~iteration_utilities.repeatfunc`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable.from_repeatfunc(int).islice(10).as_list()
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        >>> Iterable.from_repeatfunc(int, times=10).as_list()
        [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        >>> import random  # doctest: +SKIP
        >>> # Something more useful: Creating 10 random integer
        >>> Iterable.from_repeatfunc(random.randint, 0, 5,
        ...                          times=10).as_list()  # doctest: +SKIP
        [1, 3, 1, 3, 5, 2, 4, 1, 0, 1]

        .. warning::
           This returns an :py:class:`.InfiniteIterable` if `times` is not
           given.
        """
        if times:
            return Iterable(repeatfunc(func, *args, **times))
        else:
            return InfiniteIterable(repeatfunc(func, *args))

    @staticmethod
    def from_tabulate(func, start=_default):
        """See :py:func:`~iteration_utilities.tabulate`.

        Examples
        --------
        >>> from iteration_utilities import Iterable, chained
        >>> roundint = chained(round, int)
        >>> import operator
        >>> Iterable.from_tabulate(operator.neg).islice(8).as_list()
        [0, -1, -2, -3, -4, -5, -6, -7]

        >>> from math import gamma
        >>> Iterable.from_tabulate(gamma, 1).islice(8).map(roundint).as_tuple()
        (1, 1, 2, 6, 24, 120, 720, 5040)

        >>> Iterable.from_tabulate(func=gamma, start=2).islice(7).map(roundint).as_tuple()
        (1, 2, 6, 24, 120, 720, 5040)

        .. warning::
           This returns an :py:class:`.InfiniteIterable`.
        """
        if start is _default:
            return InfiniteIterable(tabulate(func))
        else:
            return InfiniteIterable(tabulate(func, start=start))

    def accumulate(self, func=_default, start=_default):
        """See :py:func:`~iteration_utilities.accumulate`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).accumulate().as_list()
        [1, 3, 6, 10, 15, 21, 28, 36, 45]

        >>> from operator import mul
        >>> Iterable(range(1, 10)).accumulate(mul, 2).as_list()
        [2, 4, 12, 48, 240, 1440, 10080, 80640, 725760]

        >>> Iterable(range(1, 10)).accumulate(func=mul, start=3).as_list()
        [3, 6, 18, 72, 360, 2160, 15120, 120960, 1088640]
        """
        return self._call(accumulate, 0, func=func, start=start)

    def clamp(self, low=_default, high=_default, inclusive=_default,
              remove=_default):
        """See :py:func:`~iteration_utilities.clamp`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).clamp(2, 7, True).as_list()
        [3, 4, 5, 6]

        >>> Iterable(range(10)).clamp(low=2, high=7, inclusive=True).as_list()
        [3, 4, 5, 6]

        >>> Iterable(range(10)).clamp(low=2, high=7, remove=False).as_list()
        [2, 2, 2, 3, 4, 5, 6, 7, 7, 7]
        """
        return self._call(clamp, 0, low=low, high=high, inclusive=inclusive,
                          remove=remove)

    def combinations(self, r):
        """See :py:func:`itertools.combinations`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).combinations(2).as_list()
        [(1, 2), (1, 3), (2, 3)]

        >>> Iterable(range(1, 4)).combinations(r=2).as_list()
        [(1, 2), (1, 3), (2, 3)]
        """
        return self._call(combinations, 0, r=r)

    def combinations_with_replacement(self, r):
        """See :py:func:`itertools.combinations_with_replacement`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).combinations_with_replacement(2).as_list()
        [(1, 1), (1, 2), (1, 3), (2, 2), (2, 3), (3, 3)]

        >>> Iterable(range(1, 4)).combinations_with_replacement(r=2).as_list()
        [(1, 1), (1, 2), (1, 3), (2, 2), (2, 3), (3, 3)]
        """
        return self._call(combinations_with_replacement, 0, r=r)

    def compress(self, selectors):
        """See :py:func:`itertools.compress`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> sel = [0, 1, 0, 1, 0, 1, 1, 1, 0]
        >>> Iterable(range(1, 10)).compress(sel).as_list()
        [2, 4, 6, 7, 8]

        >>> Iterable(range(1, 10)).compress(selectors=sel).as_list()
        [2, 4, 6, 7, 8]
        """
        return self._call(compress, 0, selectors=selectors)

    def cycle(self):
        """See :py:func:`itertools.cycle`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> it = Iterable([1, 2]).cycle()
        >>> for item in it.islice(5):
        ...     print(item)
        1
        2
        1
        2
        1
        """
        return self._call_infinite(cycle, 0)

    def deepflatten(self, depth=_default, types=_default, ignore=_default):
        """See \
:py:func:`~iteration_utilities.deepflatten`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> lst = [1, 2, 3, [1, 2, 3, [1, 2, 3]]]
        >>> Iterable(lst).deepflatten().as_list()
        [1, 2, 3, 1, 2, 3, 1, 2, 3]

        >>> Iterable(lst).deepflatten(1, list, str).as_list()
        [1, 2, 3, 1, 2, 3, [1, 2, 3]]

        >>> Iterable(lst).deepflatten(depth=1,
        ...                           types=list, ignore=str).as_list()
        [1, 2, 3, 1, 2, 3, [1, 2, 3]]
        """
        return self._call(deepflatten, 0, depth=depth, types=types,
                          ignore=ignore)

    def dropwhile(self, predicate):
        """See :py:func:`itertools.dropwhile`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).dropwhile(lambda x: x < 5).as_list()
        [5, 6, 7, 8, 9]

        >>> Iterable(range(1, 10)).dropwhile(
        ...     predicate=lambda x: x < 3).as_list()
        [3, 4, 5, 6, 7, 8, 9]
        """
        return self._call(dropwhile, 1, predicate)

    def duplicates(self, key=_default):
        """See :py:func:`~iteration_utilities.duplicates`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 1, 2, 1]).duplicates().as_list()
        [1, 1]

        >>> Iterable([1, -1, 2, 1]).duplicates(abs).as_list()
        [-1, 1]

        >>> Iterable([1, -1, 2, 1]).duplicates(key=abs).as_list()
        [-1, 1]
        """
        return self._call(duplicates, 0, key=key)

    def enumerate(self, start=_default):
        """See :py:func:`python:enumerate`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 8)).enumerate().as_list()
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7)]

        >>> Iterable(range(1, 8)).enumerate(4).as_list()
        [(4, 1), (5, 2), (6, 3), (7, 4), (8, 5), (9, 6), (10, 7)]

        >>> Iterable(range(1, 8)).enumerate(start=2).as_list()
        [(2, 1), (3, 2), (4, 3), (5, 4), (6, 5), (7, 6), (8, 7)]
        """
        return self._call(enumerate, 0, start=start)

    def filter(self, function):
        """See :py:func:`python:filter`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).filter(None).as_list()
        [1, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> from iteration_utilities import is_even
        >>> Iterable(range(1, 10)).filter(is_even).as_list()
        [2, 4, 6, 8]

        >>> Iterable(range(1, 10)).filter(function=is_even).as_list()
        [2, 4, 6, 8]
        """
        return self._call(filter, 1, function)

    def filterfalse(self, predicate):
        """See :py:func:`itertools.filterfalse`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).filterfalse(None).as_list()
        []

        >>> from iteration_utilities import is_odd
        >>> Iterable(range(1, 10)).filterfalse(is_odd).as_list()
        [2, 4, 6, 8]

        >>> Iterable(range(1, 10)).filterfalse(predicate=is_odd).as_list()
        [2, 4, 6, 8]
        """
        return self._call(filterfalse, 1, predicate)

    def flatten(self):
        """See :py:func:`~iteration_utilities.flatten`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([(1, 2, 3), [3, 2, 1]]).flatten().as_list()
        [1, 2, 3, 3, 2, 1]
        """
        return self._call(flatten, 0)

    def getitem(self, item):
        """See \
:py:func:`~iteration_utilities.getitem`

        Parameters
        ----------
        item : :py:class:`int` or :py:class:`slice`
            The item or items to retrieve

        Returns
        -------
        parts : any type or generator
            If `item` was an integer the return is a singly item otherwise it
            returns a generator of the items.

        Examples
        --------
        With integers::

            >>> from iteration_utilities import Iterable
            >>> it = Iterable(range(10))
            >>> it[2]
            2

            >>> it[-1]  # -1 is the **only** allowed negative integer.
            9

        With a tuple of integer (they will be sorted internally!)::

            >>> Iterable(range(100))[-1, 8, 3, 10, 46]  # -1 indicates last
            [3, 8, 10, 46, 99]

            >>> Iterable(range(100))[3, 8, 10, 46]
            [3, 8, 10, 46]

        With slices::

            >>> it[1:].as_list()
            [1, 2, 3, 4, 5, 6, 7, 8, 9]

            >>> it[1:8:2].as_list()
            [1, 3, 5, 7]

        Slices with negative values (only these cases are possible!)::

            >>> # start and stop negative; step None
            >>> it[-5:-2].as_list()
            [5, 6, 7]

            >>> # start and stop negative; step positive
            >>> it[-6:-1:2].as_list()
            [4, 6, 8]

            >>> # start negative, stop and step None
            >>> it[-6:].as_list()
            [4, 5, 6, 7, 8, 9]

            >>> # start negative, stop None, step positive
            >>> it[-6::2].as_list()
            [4, 6, 8]

        It's also possible to use ``getitem`` method directly, but you have to
        pass in the appropriate value(s) or :py:class:`slice`::

            >>> Iterable(range(10)).getitem(3)
            3

            >>> Iterable(range(10)).getitem(slice(5, 8)).as_tuple()
            (5, 6, 7)

        .. note::
           This function might also turn an :py:class:`.InfiniteIterable` into
           an :py:class:`.Iterable` if the slice has a positive stop.

           >>> Iterable.from_count()[:4]  # doctest: +ELLIPSIS
           <Iterable: <itertools.islice object at ...>>
        """
        return self[item]

    def grouper(self, n, fillvalue=_default, truncate=_default):
        """See :py:func:`~iteration_utilities.grouper`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).grouper(2).as_list()
        [(1, 2), (3, 4), (5, 6), (7, 8), (9,)]

        >>> Iterable(range(1, 10)).grouper(2, None).as_list()
        [(1, 2), (3, 4), (5, 6), (7, 8), (9, None)]

        >>> Iterable(range(1, 10)).grouper(n=2, fillvalue=None).as_list()
        [(1, 2), (3, 4), (5, 6), (7, 8), (9, None)]

        >>> Iterable(range(1, 10)).grouper(n=2, truncate=True).as_list()
        [(1, 2), (3, 4), (5, 6), (7, 8)]
        """
        return self._call(grouper, 0, n=n, fillvalue=fillvalue,
                          truncate=truncate)

    def islice(self, *args):
        """See :py:func:`itertools.islice`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).islice(2).as_list()
        [1, 2]

        >>> Iterable(range(1, 10)).islice(2, 6).as_list()
        [3, 4, 5, 6]

        >>> Iterable(range(1, 10)).islice(2, 6, 2).as_list()
        [3, 5]

        >>> Iterable([1, 2, 3, 4]).islice(1, None).as_list()
        [2, 3, 4]

        >>> Iterable([1, 2, 3, 4]).islice(None).as_list()
        [1, 2, 3, 4]

        .. note::
           This method converts an :py:class:`.InfiniteIterable` to a normal
           :py:class:`.Iterable` if a `stop` is given.
        """
        nargs = len(args)
        meth = self._call
        if nargs == 1:
            if args[0] is not None:
                meth = self._call_finite
        elif nargs > 1:
            if args[1] is not None:
                meth = self._call_finite
        return meth(islice, 0, *args)

    def insert(self, element, idx, unpack=_default):
        """See :py:func:`~iteration_utilities.insert`

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).insert(100, 2).as_list()
        [0, 1, 100, 2, 3, 4, 5, 6, 7, 8, 9]

        .. warning::
           This returns an :py:class:`.InfiniteIterable` if ``unpack=True`` and
           the `element` is an :py:class:`.InfiniteIterable`.

        >>> Iterable(range(10)).insert(Iterable.from_count(), 3, unpack=True) \
# doctest: +ELLIPSIS
        <InfiniteIterable: <itertools.chain object at ...>>
        """
        if unpack and isinstance(element, InfiniteIterable):
            meth = self._call_infinite
        else:
            meth = self._call
        return meth(insert, 0, element=element, idx=idx, unpack=unpack)

    def intersperse(self, e):
        """See :py:func:`~iteration_utilities.intersperse`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).intersperse(0).as_list()
        [1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9]

        >>> Iterable(range(1, 10)).intersperse(e=0).as_list()
        [1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9]
        """
        return self._call(intersperse, 0, e=e)

    def map(self, function):
        """See :py:func:`python:map`.

        Examples
        --------
        >>> from iteration_utilities import Iterable, square
        >>> Iterable(range(1, 10)).map(square).as_list()
        [1, 4, 9, 16, 25, 36, 49, 64, 81]

        >>> Iterable(range(1, 10)).map(function=square).as_list()
        [1, 4, 9, 16, 25, 36, 49, 64, 81]
        """
        return self._call(map, 1, function)

    def ncycles(self, n):
        """See :py:func:`~iteration_utilities.ncycles`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).ncycles(3).as_list()
        [1, 2, 3, 1, 2, 3, 1, 2, 3]

        >>> Iterable(range(1, 4)).ncycles(n=3).as_list()
        [1, 2, 3, 1, 2, 3, 1, 2, 3]
        """
        return self._call(ncycles, 0, n=n)

    def pad(self, fillvalue=_default, nlead=_default, ntail=_default):
        """See :py:func:`~iteration_utilities.pad`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([2]).pad(None, ntail=None).islice(10).as_list()
        [2, None, None, None, None, None, None, None, None, None]

        >>> Iterable([2]).pad(nlead=9).as_list()
        [None, None, None, None, None, None, None, None, None, 2]

        >>> Iterable([2]).pad(0, ntail=9).as_list()
        [2, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        >>> Iterable([2]).pad(0, 1, 2).as_list()
        [0, 2, 0, 0]

        .. warning::
           This returns an :py:class:`.InfiniteIterable` if ``ntail=None``.
        """
        if ntail is None:
            meth = self._call_infinite
        else:
            meth = self._call
        return meth(pad, 0, fillvalue=fillvalue, nlead=nlead, ntail=ntail)

    def permutations(self, r=_default):
        """See :py:func:`itertools.permutations`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).permutations().as_list()
        [(1, 2, 3), (1, 3, 2), (2, 1, 3), (2, 3, 1), (3, 1, 2), (3, 2, 1)]

        >>> Iterable(range(1, 4)).permutations(2).as_list()
        [(1, 2), (1, 3), (2, 1), (2, 3), (3, 1), (3, 2)]

        >>> Iterable(range(1, 4)).permutations(r=2).as_list()
        [(1, 2), (1, 3), (2, 1), (2, 3), (3, 1), (3, 2)]
        """
        return self._call(permutations, 0, r=r)

    def powerset(self):
        """See :py:func:`~iteration_utilities.powerset`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).powerset().as_list()
        [(), (1,), (2,), (3,), (1, 2), (1, 3), (2, 3), (1, 2, 3)]
        """
        return self._call(powerset, 0)

    def remove(self, idx=_default, start=_default, stop=_default):
        """See :py:func:`~iteration_utilities.remove`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).remove(idx=2).as_list()
        [0, 1, 3, 4, 5, 6, 7, 8, 9]

        .. note::
           This function might also turn an :py:class:`.InfiniteIterable` into
           an :py:class:`.Iterable` if `idx` and `stop` are ``None``.

        >>> Iterable.from_count().remove(start=4)  # doctest: +ELLIPSIS
        <Iterable: <itertools.islice object at ...>>
        """
        if ((idx is _default or idx is None) and
                (stop is None or stop is _default)):
            meth = self._call_finite
        else:
            meth = self._call
        return meth(remove, 0, idx=idx, start=start, stop=stop)

    def replace(self, element, idx=_default, start=_default, stop=_default,
                unpack=_default):
        """See :py:func:`~iteration_utilities.replace`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).replace(10, idx=2).as_list()
        [0, 1, 10, 3, 4, 5, 6, 7, 8, 9]

        .. warning::
           This returns an :py:class:`.InfiniteIterable` if ``unpack=True`` and
           the `element` is an :py:class:`.InfiniteIterable`.

        >>> Iterable(range(10)).replace(Iterable.from_count(), 4, unpack=True)\
# doctest: +ELLIPSIS
        <InfiniteIterable: <itertools.chain object at ...>>

        .. note::
           But this function might also turn an :py:class:`.InfiniteIterable`
           into an :py:class:`.Iterable` if `idx` and `stop` are ``None``.

        >>> Iterable.from_count().replace(10, start=4)  # doctest: +ELLIPSIS
        <Iterable: <itertools.chain object at ...>>
        """
        if unpack and isinstance(element, InfiniteIterable):
            meth = self._call_infinite
        elif ((idx is _default or idx is None) and
              (stop is None or stop is _default)):
            meth = self._call_finite
        else:
            meth = self._call
        return meth(replace, 0, element=element, idx=idx, start=start,
                    stop=stop, unpack=unpack)

    def replicate(self, times):
        """See :py:func:`~iteration_utilities.replicate`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).replicate(3).as_list()
        [1, 1, 1, 2, 2, 2, 3, 3, 3]

        >>> Iterable(range(1, 4)).replicate(times=4).as_list()
        [1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3]
        """
        return self._call(replicate, 0, times=times)

    def split(self, key, maxsplit=_default,
              keep=_default, keep_before=_default, keep_after=_default,
              eq=_default):
        """See :py:func:`~iteration_utilities.split`.

        Examples
        --------
        >>> from iteration_utilities import Iterable, is_even
        >>> Iterable(range(1, 10)).split(is_even).as_list()
        [[1], [3], [5], [7], [9]]

        >>> Iterable(range(1, 10)).split(is_even, 2).as_list()
        [[1], [3], [5, 6, 7, 8, 9]]

        >>> Iterable(range(1, 10)).split(3, 1, True, False, False, True).as_list()
        [[1, 2], [3], [4, 5, 6, 7, 8, 9]]

        >>> Iterable(range(1, 10)).split(3, 1, False, True, False, True).as_list()
        [[1, 2, 3], [4, 5, 6, 7, 8, 9]]

        >>> Iterable(range(1, 10)).split(3, 1, False, False, True, True).as_list()
        [[1, 2], [3, 4, 5, 6, 7, 8, 9]]

        >>> Iterable(range(1, 10)).split(key=2, maxsplit=1,
        ...                              keep=True, eq=True).as_list()
        [[1], [2], [3, 4, 5, 6, 7, 8, 9]]
        """
        return self._call(split, 0, key=key, maxsplit=maxsplit, keep=keep,
                          keep_before=keep_before, keep_after=keep_after,
                          eq=eq)

    def starfilter(self, pred):
        """See :py:func:`iteration_utilities.starfilter`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> from operator import eq
        >>> Iterable([1] * 20).enumerate().starfilter(eq).as_list()
        [(1, 1)]
        """
        return self._call(starfilter, 1, pred)

    def starmap(self, function):
        """See :py:func:`itertools.starmap`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).enumerate().starmap(pow).as_list()
        [0, 1, 8, 81, 1024, 15625, 279936, 5764801, 134217728]
        """
        return self._call(starmap, 1, function)

    def successive(self, times):
        """See :py:func:`~iteration_utilities.successive`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).successive(2).as_list()
        [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 8), (8, 9)]

        >>> Iterable(range(1, 10)).successive(times=2).as_list()
        [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 8), (8, 9)]
        """
        return self._call(successive, 0, times=times)

    def tail(self, n):
        """See :py:func:`~iteration_utilities.tail`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).tail(2).as_list()
        [8, 9]

        >>> Iterable(range(1, 10)).tail(n=3).as_list()
        [7, 8, 9]
        """
        return self._call(tail, 0, n=n)

    def takewhile(self, predicate):
        """See :py:func:`itertools.takewhile`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).takewhile(lambda x: x < 4).as_list()
        [1, 2, 3]

        >>> Iterable(range(1, 10)).takewhile(
        ...     predicate=lambda x: x < 5).as_list()
        [1, 2, 3, 4]

        .. warning::
           This method converts an :py:class:`.InfiniteIterable` to a normal
           :py:class:`.Iterable`.
        """
        return self._call_finite(takewhile, 1, predicate)

    def unique_everseen(self, key=_default):
        """See :py:func:`~iteration_utilities.unique_everseen`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).unique_everseen().as_list()
        [1, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> Iterable(range(1, 10)).unique_everseen(lambda x: x // 3).as_list()
        [1, 3, 6, 9]

        >>> from iteration_utilities import is_even
        >>> Iterable(range(1, 10)).unique_everseen(key=is_even).as_list()
        [1, 2]
        """
        return self._call(unique_everseen, 0, key=key)

    def unique_justseen(self, key=_default):
        """See :py:func:`~iteration_utilities.unique_justseen`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable('aaAAbbBcCcddDDDEEEee').unique_justseen().as_list()
        ['a', 'A', 'b', 'B', 'c', 'C', 'c', 'd', 'D', 'E', 'e']

        >>> from operator import methodcaller
        >>> Iterable('aaAAbbBcCcddDDDEEEee').unique_justseen(
        ...     methodcaller('upper')).as_list()
        ['a', 'b', 'c', 'd', 'E']

        >>> Iterable('aaAAbbBcCcddDDDEEEee').unique_justseen(
        ...     key=methodcaller('lower')).as_list()
        ['a', 'b', 'c', 'd', 'E']
        """
        return self._call(unique_justseen, 0, key=key)


class Iterable(_Base):
    """A convenience class that allows chaining the
    :py:mod:`iteration_utilities` functions.

    Parameters
    ----------
    iterable : iterable
        Any kind of `iterable`.

    Notes
    -----

    .. warning::
       If the `iterable` is infinite you should **not** create the
       :py:class:`.Iterable` instance directly (i.e. ``Iterable(count())``.
       You could use the ``Iterable.from_count()`` or create an
       :py:class:`.InfiniteIterable`: ``InfiniteIterable(count())``.

    Available methods:

    =================================================== ======================================================
    Method                                              Reference
    =================================================== ======================================================
    :py:meth:`~.Iterable.accumulate`                    See :py:func:`~iteration_utilities.accumulate`.
    :py:meth:`~.Iterable.as_`                           Convert :py:class:`.Iterable` to other class.
    :py:meth:`~.Iterable.as_counter`                    See :py:meth:`.as_`.
    :py:meth:`~.Iterable.as_dict`                       See :py:meth:`.as_`.
    :py:meth:`~.Iterable.as_frozenset`                  See :py:meth:`.as_`.
    :py:meth:`~.Iterable.as_list`                       See :py:meth:`.as_`.
    :py:meth:`~.Iterable.as_ordereddict`                See :py:meth:`.as_`.
    :py:meth:`~.Iterable.as_set`                        See :py:meth:`.as_`.
    :py:meth:`~.Iterable.as_string`                     Get the iterable as string.
    :py:meth:`~.Iterable.as_tuple`                      See :py:meth:`.as_`.
    :py:meth:`~.Iterable.clamp`                         See :py:func:`~iteration_utilities.clamp`.
    :py:meth:`~.Iterable.combinations`                  See :py:func:`itertools.combinations`.
    :py:meth:`~.Iterable.combinations_with_replacement` See :py:func:`itertools.combinations_with_replacement`.
    :py:meth:`~.Iterable.compress`                      See :py:func:`itertools.compress`.
    :py:meth:`~.Iterable.cycle`                         See :py:func:`itertools.cycle`.
    :py:meth:`~.Iterable.deepflatten`                   See :py:func:`~iteration_utilities.deepflatten`.
    :py:meth:`~.Iterable.dropwhile`                     See :py:func:`itertools.dropwhile`.
    :py:meth:`~.Iterable.duplicates`                    See :py:func:`~iteration_utilities.duplicates`.
    :py:meth:`~.Iterable.enumerate`                     See :py:func:`python:enumerate`.
    :py:meth:`~.Iterable.filter`                        See :py:func:`python:filter`.
    :py:meth:`~.Iterable.filterfalse`                   See :py:func:`itertools.filterfalse`.
    :py:meth:`~.Iterable.flatten`                       See :py:func:`~iteration_utilities.flatten`.
    :py:meth:`~.Iterable.from_applyfunc`                See :py:func:`~iteration_utilities.applyfunc`.
    :py:meth:`~.Iterable.from_count`                    See :py:func:`itertools.count`.
    :py:meth:`~.Iterable.from_empty`                    See :py:func:`~iteration_utilities.empty`.
    :py:meth:`~.Iterable.from_iterfunc_exception`       See :py:func:`~iteration_utilities.iter_except`.
    :py:meth:`~.Iterable.from_iterfunc_sentinel`        See :py:func:`python:iter`.
    :py:meth:`~.Iterable.from_itersubclasses`           See :py:func:`~iteration_utilities.itersubclasses`.
    :py:meth:`~.Iterable.from_repeat`                   See :py:func:`itertools.repeat`.
    :py:meth:`~.Iterable.from_repeatfunc`               See :py:func:`~iteration_utilities.repeatfunc`.
    :py:meth:`~.Iterable.from_tabulate`                 See :py:func:`~iteration_utilities.tabulate`.
    :py:meth:`~.Iterable.get_all`                       See :py:func:`python:all`.
    :py:meth:`~.Iterable.get_all_distinct`              See :py:func:`~iteration_utilities.all_distinct`.
    :py:meth:`~.Iterable.get_all_equal`                 See :py:func:`~iteration_utilities.all_equal`.
    :py:meth:`~.Iterable.get_all_monotone`              See :py:func:`~iteration_utilities.all_monotone`.
    :py:meth:`~.Iterable.get_any`                       See :py:func:`python:any`.
    :py:meth:`~.Iterable.get_argmax`                    See :py:func:`~iteration_utilities.argmax`.
    :py:meth:`~.Iterable.get_argmin`                    See :py:func:`~iteration_utilities.argmin`.
    :py:meth:`~.Iterable.get_argsorted`                 See :py:func:`~iteration_utilities.argsorted`.
    :py:meth:`~.Iterable.get_count_items`               See :py:func:`~iteration_utilities.count_items`.
    :py:meth:`~.Iterable.get_first`                     See :py:func:`~iteration_utilities.nth`.
    :py:meth:`~.Iterable.get_fmean`                     See :py:func:`statistics.fmean`. (Python >= 3.8)
    :py:meth:`~.Iterable.get_fsum`                      See :py:func:`math.fsum`.
    :py:meth:`~.Iterable.get_geometric_mean`            See :py:func:`statistics.geometric_mean`. (Python >= 3.8)
    :py:meth:`~.Iterable.get_groupedby`                 See :py:func:`~iteration_utilities.groupedby`.
    :py:meth:`~.Iterable.get_harmonic_mean`             See :py:func:`statistics.harmonic_mean`. (Python >= 3.6)
    :py:meth:`~.Iterable.get_last`                      See :py:func:`~iteration_utilities.nth`.
    :py:meth:`~.Iterable.get_max`                       See :py:func:`python:max`.
    :py:meth:`~.Iterable.get_mean`                      See :py:func:`statistics.mean`.
    :py:meth:`~.Iterable.get_median`                    See :py:func:`statistics.median`.
    :py:meth:`~.Iterable.get_median_grouped`            See :py:func:`statistics.median_grouped`.
    :py:meth:`~.Iterable.get_median_high`               See :py:func:`statistics.median_high`.
    :py:meth:`~.Iterable.get_median_low`                See :py:func:`statistics.median_low`.
    :py:meth:`~.Iterable.get_min`                       See :py:func:`python:min`.
    :py:meth:`~.Iterable.get_minmax`                    See :py:func:`~iteration_utilities.minmax`.
    :py:meth:`~.Iterable.get_mode`                      See :py:func:`statistics.mode`.
    :py:meth:`~.Iterable.get_multimode`                 See :py:func:`statistics.multimode`. (Python >= 3.8)
    :py:meth:`~.Iterable.get_nlargest`                  See :py:func:`heapq.nlargest`.
    :py:meth:`~.Iterable.get_nsmallest`                 See :py:func:`heapq.nsmallest`.
    :py:meth:`~.Iterable.get_nth`                       See :py:func:`~iteration_utilities.nth`.
    :py:meth:`~.Iterable.get_one`                       See :py:func:`~iteration_utilities.one`.
    :py:meth:`~.Iterable.get_partition`                 See :py:func:`~iteration_utilities.partition`.
    :py:meth:`~.Iterable.get_pstdev`                    See :py:func:`statistics.pstdev`.
    :py:meth:`~.Iterable.get_pvariance`                 See :py:func:`statistics.pvariance`.
    :py:meth:`~.Iterable.get_quantiles`                 See :py:func:`statistics.quantiles`. (Python >= 3.8)
    :py:meth:`~.Iterable.get_reduce`                    See :py:func:`functools.reduce`.
    :py:meth:`~.Iterable.get_second`                    See :py:func:`~iteration_utilities.nth`.
    :py:meth:`~.Iterable.get_sorted`                    See :py:func:`python:sorted`.
    :py:meth:`~.Iterable.get_stdev`                     See :py:func:`statistics.stdev`.
    :py:meth:`~.Iterable.get_sum`                       See :py:func:`python:sum`.
    :py:meth:`~.Iterable.get_third`                     See :py:func:`~iteration_utilities.nth`.
    :py:meth:`~.Iterable.get_variance`                  See :py:func:`statistics.variance`.
    :py:meth:`~.Iterable.getitem`                       See :py:func:`~iteration_utilities.getitem`
    :py:meth:`~.Iterable.grouper`                       See :py:func:`~iteration_utilities.grouper`.
    :py:meth:`~.Iterable.insert`                        See :py:func:`~iteration_utilities.insert`
    :py:meth:`~.Iterable.intersperse`                   See :py:func:`~iteration_utilities.intersperse`.
    :py:meth:`~.Iterable.islice`                        See :py:func:`itertools.islice`.
    :py:meth:`~.Iterable.map`                           See :py:func:`python:map`.
    :py:meth:`~.Iterable.ncycles`                       See :py:func:`~iteration_utilities.ncycles`.
    :py:meth:`~.Iterable.pad`                           See :py:func:`~iteration_utilities.pad`.
    :py:meth:`~.Iterable.permutations`                  See :py:func:`itertools.permutations`.
    :py:meth:`~.Iterable.powerset`                      See :py:func:`~iteration_utilities.powerset`.
    :py:meth:`~.Iterable.remove`                        See :py:func:`~iteration_utilities.remove`.
    :py:meth:`~.Iterable.replace`                       See :py:func:`~iteration_utilities.replace`.
    :py:meth:`~.Iterable.replicate`                     See :py:func:`~iteration_utilities.replicate`.
    :py:meth:`~.Iterable.reversed`                      See :py:func:`python:reversed`.
    :py:meth:`~.Iterable.split`                         See :py:func:`~iteration_utilities.split`.
    :py:meth:`~.Iterable.starfilter`                    See :py:func:`~iteration_utilities.starfilter`.
    :py:meth:`~.Iterable.starmap`                       See :py:func:`itertools.starmap`.
    :py:meth:`~.Iterable.successive`                    See :py:func:`~iteration_utilities.successive`.
    :py:meth:`~.Iterable.tail`                          See :py:func:`~iteration_utilities.tail`.
    :py:meth:`~.Iterable.takewhile`                     See :py:func:`itertools.takewhile`.
    :py:meth:`~.Iterable.unique_everseen`               See :py:func:`~iteration_utilities.unique_everseen`.
    :py:meth:`~.Iterable.unique_justseen`               See :py:func:`~iteration_utilities.unique_justseen`.
    =================================================== ======================================================

    Examples
    --------
    You can create an instance from any object that implements the iteration
    protocol. For example the Python types :py:class:`list`, :py:class:`tuple`,
    :py:class:`set`, :py:class:`frozenset`, :py:class:`str`, :py:class:`dict`,
    :py:meth:`dict.values`, :py:meth:`dict.items()`, :py:class:`range` just
    to name a few::

        >>> from iteration_utilities import Iterable
        >>> Iterable([1,2,3,4])
        <Iterable: [1, 2, 3, 4]>

        >>> Iterable('abcdefghijklmnopqrstuvwxyz')
        <Iterable: 'abcdefghijklmnopqrstuvwxyz'>

    :py:class:`.Iterable` is because allows chaining of several functions
    implemented in Python (:py:func:`map`, :py:func:`filter`, ...),
    :py:mod:`itertools` and :py:mod:`iteration_utilities`::

        >>> Iterable([1,2,3,4]).islice(1,3).map(float).as_list()
        [2.0, 3.0]

    The methods :py:meth:`.islice` and :py:meth:`.map` are only evaluated on the
    iterable when :py:meth:`.as_list` is called.

    The class can also be used in ``for`` loops::

        >>> from iteration_utilities import is_even
        >>> for item in Iterable(range(100, 120)).filter(is_even).accumulate():
        ...     print(item)
        100
        202
        306
        412
        520
        630
        742
        856
        972
        1090

    Some methods (:py:meth:`.Iterable.cycle`) create an
    :py:class:`.InfiniteIterable`::

        >>> Iterable(range(10)).cycle()  # doctest: +ELLIPSIS
        <InfiniteIterable: <itertools.cycle object at ...>>

    As well as some of the staticmethods (``from_x``)::

        >>> Iterable.from_count()
        <InfiniteIterable: count(0)>

        >>> Iterable.from_repeat(10)
        <InfiniteIterable: repeat(10)>

        >>> Iterable.from_repeat(10, times=2)  # but not always!
        <Iterable: repeat(10, 2)>

    This logic allows the class to be aware if the iterable is infinitely long or
    not and prevent accidental infinite loops.
    Some methods can also convert an :py:class:`.InfiniteIterable` to a normal
    :py:class:`.Iterable` again::

        >>> Iterable.from_count().islice(2, 5)  # doctest: +ELLIPSIS
        <Iterable: <itertools.islice object at ...>>

        >>> Iterable.from_count().takewhile(lambda x: x < 100)  \
# doctest: +ELLIPSIS
        <Iterable: <itertools.takewhile object at ...>>

    :py:class:`.Iterable` implements some constructors for Python types as
    methods::

        >>> Iterable(range(10)).as_list()
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> # But also some less common ones, like OrderedDict
        >>> Iterable(range(6)).enumerate(4).as_ordereddict()
        OrderedDict([(4, 0), (5, 1), (6, 2), (7, 3), (8, 4), (9, 5)])

    .. warning::
       these latter methods are (obviously) not available for
       :py:class:`.InfiniteIterable`!
    """
    __slots__ = ('_iterable',)

    def __length_hint__(self):
        return length_hint(self._iterable)

    def as_(self, cls):
        """Convert :py:class:`.Iterable` to other class.

        Parameters
        ----------
        cls : :py:class:`type`
            Convert the content of :py:class:`.Iterable` to this class.

        Returns
        -------
        iterable : cls
            The :py:class:`.Iterable` as `cls`.

        Notes
        -----
        Be careful if you use this method because the :py:class:`.Iterable` may
        be infinite.
        """
        return cls(self._iterable)

    def as_list(self):
        """See :py:meth:`.as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(5)).as_list()
        [0, 1, 2, 3, 4]
        """
        return self.as_(list)

    def as_tuple(self):
        """See :py:meth:`.as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(5)).as_tuple()
        (0, 1, 2, 3, 4)
        """
        return self.as_(tuple)

    def as_set(self):
        """See :py:meth:`.as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1]).as_set()
        {1}
        """
        return self.as_(set)

    def as_frozenset(self):
        """See :py:meth:`.as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([5]).as_frozenset()
        frozenset({5})
        """
        return self.as_(frozenset)

    def as_dict(self):
        """See :py:meth:`.as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1]).enumerate().as_dict()
        {0: 1}
        """
        return self.as_(dict)

    def as_ordereddict(self):
        """See :py:meth:`.as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(3, 6)).enumerate().as_ordereddict()
        OrderedDict([(0, 3), (1, 4), (2, 5)])
        """
        return self.as_(OrderedDict)

    def as_counter(self):
        """See :py:meth:`.as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable('supercalifragilisticexpialidocious').as_counter()  \
# doctest: +SKIP
        Counter({'i': 7, 's': 3, 'l': 3, 'c': 3, 'a': 3, 'e': 2, 'o': 2, \
'p': 2, 'r': 2, 'u': 2, 'd': 1, 'g': 1, 'f': 1, 'x': 1, 't': 1})

        >>> Iterable([1, 1, 1]).as_counter()
        Counter({1: 3})
        """
        return self.as_(Counter)

    def as_string(self, seperator=''):
        """Get the :py:class:`.Iterable` as string.

        .. warning::
           This method **does not** use :py:meth:`.as_` and differs from
           ``str(Iterable(sth))``; It uses :py:meth:`str.join`.

        Parameters
        ----------
        seperator : :py:class:`str`, optional
            The separator between each item from the iterable in the output
            string.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(5)).as_string()
        '01234'
        >>> Iterable(range(5)).as_string(' ')
        '0 1 2 3 4'
        """
        return seperator.join(map(str, self._iterable))

    def reversed(self):
        """See :py:func:`python:reversed`.

        .. warning::
           This method requires that the wrapped iterable is a `Sequence` or
           implements the `reversed` iterator protocol. Generally this does not
           work with generators!

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, 3]).reversed().as_list()
        [3, 2, 1]
        """
        return self.__class__(reversed(self._iterable))

    def _get(self, fn, pos, *args, **kwargs):
        args = _parse_args(args, self._iterable, pos)
        _parse_kwargs(kwargs, _default)
        return fn(*args, **kwargs)

    def _get_iter(self, fn, pos, *args, **kwargs):
        args = _parse_args(args, self._iterable, pos)
        _parse_kwargs(kwargs, _default)
        return fn(*args, **kwargs)

    def get_all(self):
        """See :py:func:`python:all`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).map(lambda x: x > 2).get_all()
        False

        >>> Iterable(range(10)).map(lambda x: x >= 0).get_all()
        True
        """
        return self._get(all, 0)

    def get_all_distinct(self):
        """See :py:func:`~iteration_utilities.all_distinct`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_all_distinct()
        True

        >>> Iterable([1, 2, 3, 4, 5, 6, 7, 1]).get_all_distinct()
        False
        """
        return self._get(all_distinct, 0)

    def get_all_equal(self):
        """See :py:func:`~iteration_utilities.all_equal`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_all_equal()
        False

        >>> Iterable([1]*100).get_all_equal()
        True
        """
        return self._get(all_equal, 0)

    def get_all_monotone(self, decreasing=_default, strict=_default):
        """See :py:func:`~iteration_utilities.all_monotone`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_all_monotone()
        True

        >>> Iterable(range(10)).get_all_monotone(decreasing=False, \
strict=False)
        True

        >>> Iterable(range(10)).get_all_monotone(decreasing=True, strict=True)
        False
        """
        return self._get(all_monotone, 0, decreasing=decreasing, strict=strict)

    def get_any(self):
        """See :py:func:`python:any`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).map(lambda x: x > 2).get_any()
        True

        >>> Iterable(range(10)).map(lambda x: x >= 10).get_any()
        False
        """
        return self._get(any, 0)

    def get_argmax(self, key=_default, default=_default):
        """See :py:func:`~iteration_utilities.argmax`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, -5, 3, 4]).get_argmax()
        4

        >>> Iterable([1, 2, -5, 3, 4]).get_argmax(abs)
        2

        >>> Iterable([1, 2, -5, 3, 4]).get_argmax(key=abs)
        2

        >>> Iterable([]).get_argmax(key=abs, default=-1)
        -1
        """
        return self._get(argmax, 0, key=key, default=default)

    def get_argmin(self, key=_default, default=_default):
        """See :py:func:`~iteration_utilities.argmin`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, -5, 3, 4]).get_argmin()
        2

        >>> Iterable([1, 2, -5, 3, 4]).get_argmin(abs)
        0

        >>> Iterable([1, 2, -5, 3, 4]).get_argmin(key=abs)
        0

        >>> Iterable([]).get_argmin(key=abs, default=-1)
        -1
        """
        return self._get(argmin, 0, key=key, default=default)

    def get_argsorted(self, key=_default, reverse=_default):
        """See :py:func:`~iteration_utilities.argsorted`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, -5, 3, 4]).get_argsorted()
        [2, 0, 1, 3, 4]

        >>> Iterable([1, 2, -5, 3, 4]).get_argsorted(reverse=True)
        [4, 3, 1, 0, 2]

        >>> Iterable([1, 2, -5, 3, 4]).get_argsorted(key=abs)
        [0, 1, 3, 4, 2]

        >>> Iterable([1, 2, -5, 3, 4]).get_argsorted(abs, True)
        [2, 4, 3, 1, 0]
        """
        return self._get(argsorted, 0, key=key, reverse=reverse)

    def get_count_items(self, pred=_default, eq=_default):
        """See :py:func:`~iteration_utilities.count_items`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable((i for i in range(10))).get_count_items()
        10

        >>> Iterable([1, 2, 3, 2, 1]).get_count_items(2, True)
        2

        >>> Iterable([1, 2, 3, 2, 1]).get_count_items(pred=2, eq=True)
        2
        """
        return self._get(count_items, 0, pred=pred, eq=eq)

    def get_first(self, default=_default, pred=_default, truthy=_default,
                  retpred=_default, retidx=_default):
        """See :py:func:`~iteration_utilities.nth`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_first()
        0

        >>> Iterable(range(1, 10, 2)).get_first(pred=lambda x: x > 5)
        7
        """
        return self._get(first, 0, default=default, pred=pred, truthy=truthy,
                         retpred=retpred, retidx=retidx)

    def get_fsum(self):
        """See :py:func:`math.fsum`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_fsum()
        45.0
        """
        return self._get(fsum, 0)

    def get_groupedby(self, key, keep=_default, reduce=_default,
                      reducestart=_default):
        """See :py:func:`~iteration_utilities.groupedby`.

        Examples
        --------
        >>> from iteration_utilities import Iterable, is_even
        >>> grp = Iterable(range(10)).get_groupedby(is_even)
        >>> grp[True]
        [0, 2, 4, 6, 8]
        >>> grp[False]
        [1, 3, 5, 7, 9]
        """
        return self._get(groupedby, 0, key, keep=keep, reduce=reduce,
                         reducestart=reducestart)

    def get_last(self, default=_default, pred=_default, truthy=_default,
                 retpred=_default, retidx=_default):
        """See :py:func:`~iteration_utilities.nth`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_last()
        9

        >>> Iterable(range(1, 10, 2)).get_last(pred=lambda x: x > 5)
        9
        """
        return self._get(last, 0, default=default, pred=pred, truthy=truthy,
                         retpred=retpred, retidx=retidx)

    def get_max(self, key=_default, default=_default):
        """See :py:func:`python:max`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, -5, 3, 4]).get_max()
        4

        >>> Iterable([1, 2, -5, 3, 4]).get_max(abs)
        -5

        >>> Iterable([1, 2, -5, 3, 4]).get_max(key=abs)
        -5

        >>> Iterable([]).get_max(key=abs, default=-1)
        -1
        """
        return self._get(max, 0, key=key, default=default)

    def get_min(self, key=_default, default=_default):
        """See :py:func:`python:min`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, -5, 3, 4]).get_min()
        -5

        >>> Iterable([1, 2, -5, 3, 4]).get_min(abs)
        1

        >>> Iterable([1, 2, -5, 3, 4]).get_min(key=abs)
        1

        >>> Iterable([]).get_min(key=abs, default=-1)
        -1
        """
        return self._get(min, 0, key=key, default=default)

    def get_minmax(self, key=_default, default=_default):
        """See :py:func:`~iteration_utilities.minmax`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, -5, 3, 4]).get_minmax()
        (-5, 4)

        >>> Iterable([1, 2, -5, 3, 4]).get_minmax(abs)
        (1, -5)

        >>> Iterable([1, 2, -5, 3, 4]).get_minmax(key=abs)
        (1, -5)

        >>> Iterable([]).get_minmax(key=abs, default=-1)
        (-1, -1)
        """
        return self._get(minmax, 0, key=key, default=default)

    def get_nth(self, n, default=_default, pred=_default, truthy=_default,
                retpred=_default, retidx=_default):
        """See :py:func:`~iteration_utilities.nth`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_nth(6)
        6

        >>> Iterable(range(1, 10, 2)).get_nth(0, pred=lambda x: x > 5)
        7
        """
        return self._get(nth(n), 0, default=default, pred=pred, truthy=truthy,
                         retpred=retpred, retidx=retidx)

    def get_nlargest(self, n, key=_default):
        """See :py:func:`heapq.nlargest`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([4,1,2,3,1,5,8,2,-10]).get_nlargest(3)
        [8, 5, 4]

        >>> Iterable([4,1,2,3,1,5,8,2,-10]).get_nlargest(3, key=abs)
        [-10, 8, 5]
        """
        return self._get(nlargest, 1, n, key=key)

    def get_nsmallest(self, n, key=_default):
        """See :py:func:`heapq.nsmallest`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([4,1,2,3,1,5,8,2]).get_nsmallest(3)
        [1, 1, 2]
        """
        return self._get(nsmallest, 1, n, key=key)

    def get_one(self):
        """See :py:func:`~iteration_utilities.one`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1]).get_one()
        1
        """
        return self._get(one, 0)

    def get_partition(self, pred=_default):
        """See :py:func:`~iteration_utilities.partition`.

        Examples
        --------
        >>> from iteration_utilities import Iterable, is_even
        >>> Iterable(range(5)).get_partition(is_even)
        ([1, 3], [0, 2, 4])
        """
        return self._get(partition, 0, pred=pred)

    def get_reduce(self, *args):
        """See :py:func:`functools.reduce`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> from operator import add
        >>> Iterable(range(5)).get_reduce(add)
        10
        """
        return self._get(reduce, 1, *args)

    def get_second(self, default=_default, pred=_default, truthy=_default,
                   retpred=_default, retidx=_default):
        """See :py:func:`~iteration_utilities.nth`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_second()
        1

        >>> Iterable(range(1, 10, 2)).get_second(pred=lambda x: x > 5)
        9
        """
        return self._get(second, 0, default=default, pred=pred, truthy=truthy,
                         retpred=retpred, retidx=retidx)

    def get_sorted(self, key=_default, reverse=_default):
        """See :py:func:`python:sorted`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([3, 1, 5, 12, 7]).get_sorted()
        [1, 3, 5, 7, 12]
        """
        return self._get(sorted, 0, key=key, reverse=reverse)

    def get_sum(self, start=_default):
        """See :py:func:`python:sum`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([3, 1, 5, 12, 7]).get_sum()
        28

        >>> Iterable([3, 1, 5, 12, 7]).get_sum(10)
        38
        """
        if start is _default:
            return self._get(sum, 0)
        return self._get(sum, 0, start)

    def get_third(self, default=_default, pred=_default, truthy=_default,
                  retpred=_default, retidx=_default):
        """See :py:func:`~iteration_utilities.nth`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_third()
        2

        >>> Iterable(range(1, 10, 2)).get_third(default=-1,
        ...                                     pred=lambda x: x > 5)
        -1
        """
        return self._get(third, 0, default=default, pred=pred, truthy=truthy,
                         retpred=retpred, retidx=retidx)

    def get_mean(self):
        """See :py:func:`statistics.mean`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_mean()
        4.5
        """
        return self._get_iter(statistics.mean, 0)

    def get_median(self):
        """See :py:func:`statistics.median`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(11)).get_median()
        5
        """
        return self._get_iter(statistics.median, 0)

    def get_median_low(self):
        """See :py:func:`statistics.median_low`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_median_low()
        4
        """
        return self._get_iter(statistics.median_low, 0)

    def get_median_high(self):
        """See :py:func:`statistics.median_high`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_median_high()
        5
        """
        return self._get_iter(statistics.median_high, 0)

    def get_median_grouped(self, interval=_default):
        """See :py:func:`statistics.median_grouped`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(10)).get_median_grouped(interval=4)
        3.0
        """
        return self._get_iter(statistics.median_grouped, 0,
                                interval=interval)

    def get_mode(self):
        """See :py:func:`statistics.mode`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_mode()
        1
        """
        return self._get_iter(statistics.mode, 0)

    def get_pstdev(self, mu=_default):
        """See :py:func:`statistics.pstdev`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_pstdev()
        2.635667953694125
        """
        return self._get_iter(statistics.pstdev, 0, mu=mu)

    def get_pvariance(self, mu=_default):
        """See :py:func:`statistics.pvariance`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_pvariance()  \
# doctest: +ELLIPSIS
        6.94674556...
        """
        return self._get_iter(statistics.pvariance, 0, mu=mu)

    def get_stdev(self, xbar=_default):
        """See :py:func:`statistics.stdev`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_stdev()
        2.743290182543769
        """
        return self._get_iter(statistics.stdev, 0, xbar=xbar)

    def get_variance(self, mu=_default):
        """See :py:func:`statistics.variance`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_variance()
        7.5256410256410255
        """
        return self._get_iter(statistics.variance, 0, mu=mu)

    def get_harmonic_mean(self):
        """See :py:func:`statistics.harmonic_mean`.

        .. note::
            Python >= 3.6 is required for this function.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_harmonic_mean()  # doctest: +ELLIPSIS
        2.369791...
        """
        return self._get_iter(statistics.harmonic_mean, 0)

    def get_fmean(self):
        """See :py:func:`statistics.fmean`.

        .. note::
            Python >= 3.8 is required for this function.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_fmean()  # doctest: +ELLIPSIS
        4.230769...
        """
        return self._get_iter(statistics.fmean, 0)

    def get_geometric_mean(self):
        """See :py:func:`statistics.geometric_mean`.

        .. note::
            Python >= 3.8 is required for this function.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_geometric_mean()  # doctest: +ELLIPSIS
        3.250146...
        """
        return self._get_iter(statistics.geometric_mean, 0)

    def get_multimode(self):
        """See :py:func:`statistics.multimode`.

        .. note::
            Python >= 3.8 is required for this function.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,2,3,4,5,6,7,7,8,8]).get_multimode()
        [1, 2]
        """
        return self._get_iter(statistics.multimode, 0)

    def get_quantiles(self, n=_default, method=_default):
        """See :py:func:`statistics.quantiles`.

        .. note::
            Python >= 3.8 is required for this function.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_quantiles()
        [1.5, 4.0, 7.0]
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_quantiles(n=10)
        [1.0, 1.0, 2.0, 2.6, 4.0, 5.4, 6.8, 7.2, 8.0]
        >>> Iterable([1,1,1,2,2,3,4,5,6,7,7,8,8]).get_quantiles(n=10, method='inclusive')
        [1.0, 1.4, 2.0, 2.8, 4.0, 5.2, 6.4, 7.0, 7.8]
        """
        return self._get_iter(statistics.quantiles, 0, n=n, method=method)


class InfiniteIterable(_Base):
    """Like :py:class:`.Iterable` but indicates that the wrapped iterable is
    infinitely long.

    .. warning::
       The ``Iterable.as_*`` methods are not available for
       :py:class:`.InfiniteIterable` because it would be impossible to create
       these types. Use :py:meth:`.InfiniteIterable.islice` or
       :py:meth:`.InfiniteIterable.takewhile` to convert an infinite iterable
       to a finite iterable. It is still possible to iterate over the iterable
       with ``for item in ...`` or using the Python constructors like
       :py:class:`list` directly. This may fail fatally!

    Mostly it is not necessary to use :py:class:`.InfiniteIterable` directly
    because the corresponding methods on :py:class:`.Iterable` return an
    :py:class:`.InfiniteIterable` when appropriate. However using
    ``isinstance(some_iterable, InfiniteIterable)`` could be used to determine
    if the :py:class:`.Iterable` is infinite!

    Available methods:

    =========================================================== =======================================================
    Method                                                      Reference
    =========================================================== =======================================================
    :py:meth:`~.InfiniteIterable.accumulate`                    See :py:func:`~iteration_utilities.accumulate`.
    :py:meth:`~.InfiniteIterable.clamp`                         See :py:func:`~iteration_utilities.clamp`.
    :py:meth:`~.InfiniteIterable.combinations`                  See :py:func:`itertools.combinations`.
    :py:meth:`~.InfiniteIterable.combinations_with_replacement` See :py:func:`itertools.combinations_with_replacement`.
    :py:meth:`~.InfiniteIterable.compress`                      See :py:func:`itertools.compress`.
    :py:meth:`~.InfiniteIterable.cycle`                         See :py:func:`itertools.cycle`.
    :py:meth:`~.InfiniteIterable.deepflatten`                   See :py:func:`~iteration_utilities.deepflatten`.
    :py:meth:`~.InfiniteIterable.dropwhile`                     See :py:func:`itertools.dropwhile`.
    :py:meth:`~.InfiniteIterable.duplicates`                    See :py:func:`~iteration_utilities.duplicates`.
    :py:meth:`~.InfiniteIterable.enumerate`                     See :py:func:`python:enumerate`.
    :py:meth:`~.InfiniteIterable.filter`                        See :py:func:`python:filter`.
    :py:meth:`~.InfiniteIterable.filterfalse`                   See :py:func:`itertools.filterfalse`.
    :py:meth:`~.InfiniteIterable.flatten`                       See :py:func:`~iteration_utilities.flatten`.
    :py:meth:`~.InfiniteIterable.from_applyfunc`                See :py:func:`~iteration_utilities.applyfunc`.
    :py:meth:`~.InfiniteIterable.from_count`                    See :py:func:`itertools.count`.
    :py:meth:`~.InfiniteIterable.from_empty`                    See :py:func:`~iteration_utilities.empty`.
    :py:meth:`~.InfiniteIterable.from_iterfunc_exception`       See :py:func:`~iteration_utilities.iter_except`.
    :py:meth:`~.InfiniteIterable.from_iterfunc_sentinel`        See :py:func:`python:iter`.
    :py:meth:`~.InfiniteIterable.from_itersubclasses`           See :py:func:`~iteration_utilities.itersubclasses`.
    :py:meth:`~.InfiniteIterable.from_repeat`                   See :py:func:`itertools.repeat`.
    :py:meth:`~.InfiniteIterable.from_repeatfunc`               See :py:func:`~iteration_utilities.repeatfunc`.
    :py:meth:`~.InfiniteIterable.from_tabulate`                 See :py:func:`~iteration_utilities.tabulate`.
    :py:meth:`~.InfiniteIterable.getitem`                       See :py:func:`~iteration_utilities.getitem`
    :py:meth:`~.InfiniteIterable.grouper`                       See :py:func:`~iteration_utilities.grouper`.
    :py:meth:`~.InfiniteIterable.insert`                        See :py:func:`~iteration_utilities.insert`
    :py:meth:`~.InfiniteIterable.intersperse`                   See :py:func:`~iteration_utilities.intersperse`.
    :py:meth:`~.InfiniteIterable.islice`                        See :py:func:`itertools.islice`.
    :py:meth:`~.InfiniteIterable.map`                           See :py:func:`python:map`.
    :py:meth:`~.InfiniteIterable.ncycles`                       See :py:func:`~iteration_utilities.ncycles`.
    :py:meth:`~.InfiniteIterable.pad`                           See :py:func:`~iteration_utilities.pad`.
    :py:meth:`~.InfiniteIterable.permutations`                  See :py:func:`itertools.permutations`.
    :py:meth:`~.InfiniteIterable.powerset`                      See :py:func:`~iteration_utilities.powerset`.
    :py:meth:`~.InfiniteIterable.remove`                        See :py:func:`~iteration_utilities.remove`.
    :py:meth:`~.InfiniteIterable.replace`                       See :py:func:`~iteration_utilities.replace`.
    :py:meth:`~.InfiniteIterable.replicate`                     See :py:func:`~iteration_utilities.replicate`.
    :py:meth:`~.InfiniteIterable.split`                         See :py:func:`~iteration_utilities.split`.
    :py:meth:`~.InfiniteIterable.starfilter`                    See :py:func:`~iteration_utilities.starfilter`.
    :py:meth:`~.InfiniteIterable.starmap`                       See :py:func:`itertools.starmap`.
    :py:meth:`~.InfiniteIterable.successive`                    See :py:func:`~iteration_utilities.successive`.
    :py:meth:`~.InfiniteIterable.tail`                          See :py:func:`~iteration_utilities.tail`.
    :py:meth:`~.InfiniteIterable.takewhile`                     See :py:func:`itertools.takewhile`.
    :py:meth:`~.InfiniteIterable.unique_everseen`               See :py:func:`~iteration_utilities.unique_everseen`.
    :py:meth:`~.InfiniteIterable.unique_justseen`               See :py:func:`~iteration_utilities.unique_justseen`.
    =========================================================== =======================================================
    """
    __slots__ = ('_iterable',)


class ManyIterables:
    __slots__ = ('_iterables',)

    def __init__(self, *iterables):
        """:py:class:`.ManyIterables` stores several `iterables` and implements
        methods to convert these to one :py:class:`.Iterable`.

        .. warning::
           :py:class:`.ManyIterables` itself cannot be iterated!

        Parameters
        ----------
        *iterables : any amount of iterables
            The `iterables` to store.

        Notes
        -----
        This is just a convenience class to separate the expressions dealing
        with multiple iterables from those applying on one.

        Available methods:

        ===================================== ===============================================
        Method                                Reference
        ===================================== ===============================================
        :py:meth:`~ManyIterables.chain`       See :py:func:`itertools.chain`.
        :py:meth:`~ManyIterables.map`         See :py:func:`python:map`.
        :py:meth:`~ManyIterables.merge`       See :py:func:`~iteration_utilities.merge`.
        :py:meth:`~ManyIterables.product`     See :py:func:`itertools.product`.
        :py:meth:`~ManyIterables.roundrobin`  See :py:func:`~iteration_utilities.roundrobin`.
        :py:meth:`~ManyIterables.zip`         See :py:func:`python:zip`.
        :py:meth:`~ManyIterables.zip_longest` See :py:func:`itertools.zip_longest`.
        ===================================== ===============================================

        Examples
        --------
        Depending on the function and the types of the `iterables` the returned
        class may be different. For example :py:meth:`map` returns an
        :py:class:`.InfiniteIterable` if **all** `iterables` are infinite::

            >>> from iteration_utilities import ManyIterables
            >>> ManyIterables(Iterable.from_count(10), range(10)).map(pow)  \
# doctest: +ELLIPSIS
            <Iterable: <map object at ...>>

            >>> ManyIterables(Iterable.from_count(10),
            ...               Iterable.from_count(10)).map(pow)  \
# doctest: +ELLIPSIS
            <InfiniteIterable: <map object at ...>>

        While other methods also return an :py:class:`.InfiniteIterable` if
        **any** of the `iterables` is infinite::

            >>> ManyIterables(range(10), Iterable.from_count(10)).merge()  \
# doctest: +ELLIPSIS
            <InfiniteIterable: <iteration_utilities.merge object at ...>>

            >>> ManyIterables(range(10), range(10)).merge()  \
# doctest: +ELLIPSIS
            <Iterable: <iteration_utilities.merge object at ...>>

        Each method has a note explicitly stating to which of these categories
        it belongs.
        """
        self._iterables = iterables

    def _call(self, fn, infinitecheck, *args, **kwargs):
        iterables = self._iterables
        if infinitecheck and any_isinstance(iterables, InfiniteIterable):
            cls = InfiniteIterable
        elif not infinitecheck and all_isinstance(iterables, InfiniteIterable):
            cls = InfiniteIterable
        else:
            cls = Iterable
        if args:
            iterables = args + iterables
        _parse_kwargs(kwargs, _default)
        return cls(fn(*iterables, **kwargs))

    def chain(self):
        """See :py:func:`itertools.chain`.

        .. note::
           If any of the `iterables` is infinite then this will also
           return an :py:class:`.InfiniteIterable`.

        Examples
        --------
        >>> from iteration_utilities import ManyIterables
        >>> ManyIterables([1,3,5,7,9], [0,2,4,6,8]).chain().as_list()
        [1, 3, 5, 7, 9, 0, 2, 4, 6, 8]
        """
        return self._call(chain, True)

    def map(self, function):
        """See :py:func:`python:map`.

        .. note::
           If **all** of the `iterables` is infinite then this will also return
           an :py:class:`.InfiniteIterable`.

        Examples
        --------
        >>> from iteration_utilities import ManyIterables
        >>> ManyIterables([1,3,5,7,9], [0,2,4,6,8]).map(pow).as_list()
        [1, 9, 625, 117649, 43046721]
        """
        return self._call(map, False, function)

    def merge(self, key=_default, reverse=_default):
        """See :py:func:`~iteration_utilities.merge`.

        .. note::
           If any of the `iterables` is infinite then this will also return
           an :py:class:`.InfiniteIterable`.

        Examples
        --------
        >>> from iteration_utilities import ManyIterables
        >>> ManyIterables([1,3,5,7,9], [0,2,4,6,8]).merge().as_list()
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> from operator import neg
        >>> ManyIterables([1,3,5,7,9], [0,2,4,6,8]).merge(neg, True).as_list()
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> ManyIterables([1,3,5,7,9], [0,2,4,6,8]).merge(
        ...     key=neg, reverse=True).as_list()
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        """
        return self._call(merge, True, key=key, reverse=reverse)

    def product(self, repeat=_default):
        """See :py:func:`itertools.product`.

        .. note::
           If any of the `iterables` is infinite then this will also return
           an :py:class:`.InfiniteIterable`.

        Examples
        --------
        >>> from iteration_utilities import ManyIterables
        >>> ManyIterables([1,2], [10, 11, 12]).product().as_list()
        [(1, 10), (1, 11), (1, 12), (2, 10), (2, 11), (2, 12)]

        >>> ManyIterables([1], [10, 11]).product(2).as_list()
        [(1, 10, 1, 10), (1, 10, 1, 11), (1, 11, 1, 10), (1, 11, 1, 11)]

        >>> ManyIterables([1], [10, 11]).product(repeat=2).as_list()
        [(1, 10, 1, 10), (1, 10, 1, 11), (1, 11, 1, 10), (1, 11, 1, 11)]
        """
        return self._call(product, True, repeat=repeat)

    def roundrobin(self):
        """See :py:func:`~iteration_utilities.roundrobin`.

        .. note::
           If any of the `iterables` is infinite then this will also return
           an :py:class:`.InfiniteIterable`.

        Examples
        --------
        >>> from iteration_utilities import ManyIterables
        >>> ManyIterables([1,2,3,4], [10, 11, 12]).roundrobin().as_list()
        [1, 10, 2, 11, 3, 12, 4]
        """
        return self._call(roundrobin, True)

    def zip(self):
        """See :py:func:`python:zip`.

        .. note::
           If **all** of the `iterables` is infinite then this will also return
           an :py:class:`.InfiniteIterable`.

        Examples
        --------
        >>> from iteration_utilities import ManyIterables
        >>> ManyIterables([1,2,3,4], [2,3,4,5]).zip().as_list()
        [(1, 2), (2, 3), (3, 4), (4, 5)]
        """
        return self._call(zip, False)

    def zip_longest(self, fillvalue=_default):
        """See :py:func:`itertools.zip_longest`.

        .. note::
           If any of the `iterables` is infinite then this will also return
           an :py:class:`.InfiniteIterable`.

        Examples
        --------
        >>> from iteration_utilities import ManyIterables
        >>> ManyIterables([1,2,3,4], [2,3,4]).zip_longest().as_list()
        [(1, 2), (2, 3), (3, 4), (4, None)]

        >>> ManyIterables([1,2,3,4], [2,3,4]).zip_longest('x').as_list()
        [(1, 2), (2, 3), (3, 4), (4, 'x')]

        >>> ManyIterables([1,2,3,4], [2,3,4]).zip_longest(
        ...     fillvalue='x').as_list()
        [(1, 2), (2, 3), (3, 4), (4, 'x')]
        """
        return self._call(zip_longest, True, fillvalue=fillvalue)
