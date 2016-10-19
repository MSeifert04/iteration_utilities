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

    def append(self, element):
        """See :py:func:`~iteration_utilities._recipes._additional.append`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).append(10).as_list()
        [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

        >>> Iterable(range(1, 10)).append(element=10).as_list()
        [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        """
        return self._call(append, 1, element)

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
        return self._call(combinations, 0, r)

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
        return self._call(combinations_with_replacement, 0, r)

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
        return self._call(compress, 0, selectors)

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
        """See
        :py:func:`~iteration_utilities._recipes._additional.deepflatten`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> lst = [1, 2, 3, [1, 2, 3, [1, 2, 3]]]
        >>> Iterable(lst).deepflatten().as_list()
        [1, 2, 3, 1, 2, 3, 1, 2, 3]

        >>> Iterable(lst).deepflatten(1, (list), (str)).as_list()
        [1, 2, 3, 1, 2, 3, [1, 2, 3]]

        >>> Iterable(lst).deepflatten(depth=1,
        ...                           types=(list), ignore=(str)).as_list()
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

        >>> Iterable(range(1, 10)).dropwhile(predicate=lambda x: x < 3).as_list()
        [3, 4, 5, 6, 7, 8, 9]
        """
        return self._call(dropwhile, 1, predicate)

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
        """See :py:func:`~iteration_utilities._recipes._core.flatten`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([(1, 2, 3), [3, 2, 1]]).flatten().as_list()
        [1, 2, 3, 3, 2, 1]
        """
        return self._call(flatten, 0)

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
        return self._call(grouper, 0, n, fillvalue=fillvalue,
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
        """
        # TODO: If stop is given this could transform an infinite to finite
        #       iterable.
        return self._call(islice, 0, *args)

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
        return self._call(intersperse, 0, e)

    def map(self, function):
        """See :py:func:`python:map`.

        Examples
        --------
        >>> from iteration_utilities import Iterable, square
        >>> Iterable(range(1, 10)).map(square).as_list()
        {0}

        >>> Iterable(range(1, 10)).map(function=square).as_list()
        {0}
        """
        return self._call(map, 1, function)
    map.__doc__ = map.__doc__.format(
        '[1L, 4L, 9L, 16L, 25L, 36L, 49L, 64L, 81L]' if PY2 else
        '[1, 4, 9, 16, 25, 36, 49, 64, 81]')

    def ncycles(self, n):
        """See :py:func:`~iteration_utilities._recipes._core.ncycles`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).ncycles(3).as_list()
        [1, 2, 3, 1, 2, 3, 1, 2, 3]

        >>> Iterable(range(1, 4)).ncycles(n=3).as_list()
        [1, 2, 3, 1, 2, 3, 1, 2, 3]
        """
        return self._call(ncycles, 0, n)

    def padnone(self):
        """See :py:func:`~iteration_utilities._recipes._core.padnone`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        """
        return self._call_infinite(padnone, 0)

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
        """See :py:func:`~iteration_utilities._recipes._core.powerset`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 4)).powerset().as_list()
        [(), (1,), (2,), (3,), (1, 2), (1, 3), (2, 3), (1, 2, 3)]
        """
        return self._call(powerset, 0)

    def prepend(self, element):
        """See :py:func:`~iteration_utilities._recipes._additional.prepend`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).prepend(100).as_list()
        [100, 1, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> Iterable(range(1, 10)).prepend(element=100).as_list()
        [100, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        """
        return self._call(prepend, 1, element)

    def split(self, key, maxsplit=_default, keep=_default, eq=_default):
        """See :py:func:`~iteration_utilities.split`.

        Examples
        --------
        >>> from iteration_utilities import Iterable, is_even, is_odd
        >>> Iterable(range(1, 10)).split(is_even).as_list()
        [[1], [3], [5], [7], [9]]

        >>> Iterable(range(1, 10)).split(is_even, 2).as_list()
        [[1], [3], [5, 6, 7, 8, 9]]

        >>> Iterable(range(1, 10)).split(3, 1, True, True).as_list()
        [[1, 2], [3], [4, 5, 6, 7, 8, 9]]

        >>> Iterable(range(1, 10)).split(key=2, maxsplit=1,
        ...                              keep=True, eq=True).as_list()
        [[1], [2], [3, 4, 5, 6, 7, 8, 9]]
        """
        return self._call(split, 0, key, maxsplit=maxsplit, keep=keep, eq=eq)

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
        return self._call(successive, 0, times)

    def tail(self, n):
        """See :py:func:`~iteration_utilities._recipes._core.tail`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).tail(2).as_list()
        [8, 9]

        >>> Iterable(range(1, 10)).tail(n=3).as_list()
        [7, 8, 9]
        """
        return self._call(tail, 0, n)

    def takewhile(self, predicate):
        """See :py:func:`itertools.takewhile`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(1, 10)).takewhile(lambda x: x < 4).as_list()
        [1, 2, 3]

        >>> Iterable(range(1, 10)).takewhile(predicate=lambda x: x < 5).as_list()
        [1, 2, 3, 4]
        """
        return self._call(takewhile, 1, predicate)

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

    def as_list(self):
        """See :py:meth:`as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(5)).as_list()
        [0, 1, 2, 3, 4]
        """
        return self.as_(list)

    def as_tuple(self):
        """See :py:meth:`as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(5)).as_tuple()
        (0, 1, 2, 3, 4)
        """
        return self.as_(tuple)

    def as_set(self):
        """See :py:meth:`as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1]).as_set()
        {0}
        """
        return self.as_(set)
    as_set.__doc__ = as_set.__doc__.format('set([1])' if PY2 else '{1}')

    def as_frozen_set(self):
        """See :py:meth:`as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([5]).as_frozen_set()
        frozenset({0})
        """
        return self.as_(frozenset)
    as_frozen_set.__doc__ = as_frozen_set.__doc__.format('[5]' if PY2 else
                                                         '{5}')

    def as_dict(self):
        """See :py:meth:`as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1]).enumerate().as_dict()
        {0: 1}
        """
        return self.as_(dict)

    def as_ordered_dict(self):
        """See :py:meth:`as_`.

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable(range(3, 6)).enumerate().as_ordered_dict()
        OrderedDict([(0, 3), (1, 4), (2, 5)])
        """
        return self.as_(OrderedDict)

    def reversed(self):
        """See :py:func:`python:reversed`.

        .. warning::
           This method requires that the `Iterable` is a `Sequence` or
           implements the `__reversed__` method. Generally this does not work
           with generators!

        Examples
        --------
        >>> from iteration_utilities import Iterable
        >>> Iterable([1, 2, 3]).reversed().as_list()
        [3, 2, 1]
        """
        return self.__class__(reversed(self._iterable))


class InfiniteIterable(_Base):
    # TODO: Needs class documentation
    pass
