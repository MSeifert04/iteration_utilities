# Licensed under Apache License Version 2.0 - see LICENSE

"""
API: Additional recipes
-----------------------
"""

# Built-ins
from collections import OrderedDict
from itertools import chain, islice, repeat, product, combinations
from operator import itemgetter

# This module
from iteration_utilities import nth, unique_justseen, chained
from ._recipes import tail


__all__ = ['argsorted', 'combinations_from_relations', 'getitem',
           'insert', 'itersubclasses', 'pad', 'remove', 'replace']


def argsorted(iterable, key=None, reverse=False):
    """Returns the indices that would sort the `iterable`.

    Parameters
    ----------
    iterable : iterable
        The `iterable` to sort.

    key : callable, None, optional
        If ``None`` sort the items in the `iterable`, otherwise sort the
        ``key(items)``.
        Default is ``None``.

    reverse : :py:class:`bool`, optional
        If ``False`` sort the `iterable` in increasing order otherwise in
        decreasing order.
        Default is ``False``.

    Returns
    -------
    sortindices : :py:class:`list`
        The indices that would sort the `iterable`.

    Notes
    -----
    See :py:func:`sorted` for more explanations to the parameters.

    Examples
    --------
    To get the indices that would sort a sequence in increasing order::

        >>> from iteration_utilities import argsorted
        >>> argsorted([3, 1, 2])
        [1, 2, 0]

    It also works when sorting in decreasing order::

        >>> argsorted([3, 1, 2], reverse=True)
        [0, 2, 1]

    And when applying a `key` function::

        >>> argsorted([3, 1, -2], key=abs)
        [1, 2, 0]
    """
    if key is None:
        key = itemgetter(1)
    else:
        key = chained(itemgetter(1), key)
    return [i[0] for i in sorted(enumerate(iterable),
                                 key=key, reverse=reverse)]


def combinations_from_relations(dictionary, r):
    """Yield combinations where only one item (or None) of each equivalence
    class is present.

    Parameters
    ----------
    dictionary : :py:class:`dict` with iterable values or convertible to one.
        A dictionary defining the equivalence classes, each key should contain
        all equivalent items as it's value.

        .. warning::
            Each ``value`` in the `dictionary` must be iterable.

        .. note::
            If the `dictionary` isn't ordered then the order in the
            combinations and their order of appearance is not-deterministic.

        .. note::
            If the `dictionary` isn't :py:class:`dict`-like it will be
            converted to an :py:class:`collections.OrderedDict`.

    r : :py:class:`int` or None, optional
        The number of combinations, if ``None`` it defaults to the length of
        the `dictionary`.

    Returns
    -------
    combinations : generator
        The combinations from the dictionary.

    Examples
    --------
    In general the :py:class:`collections.OrderedDict` should be used to call
    the function. But it will also be automatically converted to one if
    one inserts an iterable that is convertible to a dict::

        >>> from iteration_utilities import combinations_from_relations

        >>> classes = [('a', [1, 2]), ('b', [3, 4]), ('c', [5, 6])]
        >>> for comb in combinations_from_relations(classes, 2):
        ...     print(comb)
        (1, 3)
        (1, 4)
        (2, 3)
        (2, 4)
        (1, 5)
        (1, 6)
        (2, 5)
        (2, 6)
        (3, 5)
        (3, 6)
        (4, 5)
        (4, 6)

    This is equivalent to creating the :py:class:`collections.OrderedDict`
    manually::

        >>> from collections import OrderedDict
        >>> odct = OrderedDict(classes)
        >>> for comb in combinations_from_relations(odct, 3):
        ...     print(comb)
        (1, 3, 5)
        (1, 3, 6)
        (1, 4, 5)
        (1, 4, 6)
        (2, 3, 5)
        (2, 3, 6)
        (2, 4, 5)
        (2, 4, 6)
    """
    if not isinstance(dictionary, dict):
        dictionary = OrderedDict(dictionary)

    for keycomb in combinations(dictionary, r):
        yield from product(*itemgetter(*keycomb)(dictionary))


def itersubclasses(cls, seen=None):
    """Iterate over the subclasses of `cls`. Recipe based on the snippet
    of Gabriel Genellina ([0]_) but modified.

    Parameters
    ----------
    cls : :py:class:`type`
        The class for which to iterate over the subclasses.

    seen : set, None, optional
        Classes to exclude from iteration or ``None`` if all subclasses should
        be returned.
        Default is ``None``.

    Returns
    -------
    subclasses : generator
        The subclasses of `cls`.

    Examples
    --------
    It works with any class and also handles diamond inheritance structures::

        >>> class A: pass
        >>> class B(A): pass
        >>> class C(B): pass
        >>> class D(C): pass
        >>> class E(C): pass
        >>> class F(D, E): pass
        >>> list(i.__name__ for i in itersubclasses(A))
        ['B', 'C', 'D', 'F', 'E']

    There is mostly no need to specify `seen` but this can be used to exclude
    the class and all subclasses for it::

        >>> [i.__name__ for i in itersubclasses(A, seen={C})]
        ['B']

    And it also works for objects subclassing :py:class:`type`::

        >>> class Z(type): pass
        >>> class Y(Z): pass
        >>> [i.__name__ for i in itersubclasses(Z)]
        ['Y']

    The reverse operation: To iterate over the superclasses is possible using
    the ``class_to_test.__mro__`` attribute::

        >>> [i.__name__ for i in F.__mro__]
        ['F', 'D', 'E', 'C', 'B', 'A', 'object']

    References
    ----------
    .. [0] http://code.activestate.com/recipes/576949/
    """
    if seen is None:
        seen = set()

    try:
        subs = cls.__subclasses__()
    except TypeError:
        # fails if cls is "type"
        subs = cls.__subclasses__(cls)

    # This part is some combination of unique_everseen and flatten, however
    # I did not found a way to use these here.
    for sub in subs:
        if sub not in seen:
            seen.add(sub)
            yield sub
            yield from itersubclasses(sub, seen)


def pad(iterable, fillvalue=None, nlead=0, ntail=0):
    """Pad the `iterable` with `fillvalue` in front and behind.

    Parameters
    ----------
    iterable : iterable
        The `iterable` to pad.

    fillvalue : any type, optional
        The padding value.
        Default is ``None``.

    nlead, ntail : :py:class:`int` or None, optional
        The number of times to pad in front (`nlead`) and after (`ntail`) the
        `iterable`. If `ntail` is ``None`` pad indefinitely (not possible for
        `nlead`).
        Default is ``0``.

    Returns
    -------
    padded_iterable : generator
        The padded `iterable`.

    Examples
    --------
    >>> from iteration_utilities import pad, getitem
    >>> list(pad([1,2,3], 0, 5))
    [0, 0, 0, 0, 0, 1, 2, 3]

    >>> list(pad([1,2,3], 0, ntail=5))
    [1, 2, 3, 0, 0, 0, 0, 0]

    >>> list(pad([1,2,3], 0, nlead=5, ntail=5))
    [0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0]

    >>> list(getitem(pad([1,2,3], 0, ntail=None), stop=10))
    [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]

    .. warning::
        This will return an infinitely long generator if ``ntail`` is ``None``,
        so do not try to do something like ``list(pad([], ntail=None))``!
    """
    prepend = repeat(fillvalue, nlead)

    if ntail is None:
        append = repeat(fillvalue)
    else:
        append = repeat(fillvalue, ntail)

    return chain(prepend, iterable, append)


# =============================================================================
# List-like interface methods
#
# getitem: list[x]
# insert: list[x:x] = item
# replace: list[x:y] = item
# remove: del list[x:y]
#
# =============================================================================


def getitem(iterable, idx=None, start=None, stop=None, step=None):
    """Get the item at `idx` or the items specified by `start`, `stop` and
    `step`.

    Parameters
    ----------
    iterable : iterable
        The iterable from which to extract the items.

    idx : positive :py:class:`int`, -1, tuple/list thereof, or None, optional
        If not ``None``, get the item at `idx`. If it's a tuple or list get
        all the items specified in the tuple (they will be sorted so the
        specified indices are retrieved).
        Default is ``None``.

        .. note::
           This parameter must not be ``None`` if also `start`, `stop` and
           `step` are ``None``.

    start : :py:class:`int` or None, optional
        If ``None`` then take all items before `stop`, otherwise take only
        the items starting by `start`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    stop : :py:class:`int` or None, optional
        If ``None`` then take all items starting by `start`, otherwise only
        take the items before `stop`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    step : positive :py:class:`int` or None, optional
        If ``None`` then take all items separated by `step`, otherwise take
        successive items.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    Returns
    -------
    items : any type or generator
        If `idx` was not ``None`` then it returns the item, otherwise it
        returns the items specified by `start`, `stop` and `step`.

    Examples
    --------
    The main bulk of examples is in
    :py:meth:`~iteration_utilities.Iterable.getitem` because that's where
    this function was originally implemented.
    """
    if idx is None and start is None and stop is None and step is None:
        raise TypeError('one of "idx", "start" or "stop" must be given.')

    it = iter(iterable)

    if idx is not None:
        if not isinstance(idx, (tuple, list)):
            if idx < -1:
                raise ValueError('index must be -1 or bigger.')
            return nth(idx)(iterable)
        elif not idx:
            return []
        else:
            # A list of indices, we sort it (insert -1 at the end because it's
            # the last one) and then extract all the values.
            idx = sorted(idx, key=lambda x: x if x != -1 else float('inf'))
            if idx[0] < -1:
                raise ValueError('index must be -1 or bigger.')
            current = 0
            ret = []
            for i in unique_justseen(idx):
                ret.append(nth(i-current)(it))
                current = i+1
            return ret

    start_gt_0 = start is None or start > 0
    step_gt_0 = step is None or step > 0

    start_lt_0 = start is not None and start < 0
    stop_lt_0 = stop is not None and stop < 0
    step_lt_0 = step is not None and step < 0

    # Several possibilities:

    # - start None, stop None, step None = self
    # if start is None and stop is None and step is None:
    #     return iterable

    # - start None or > 0, stop None, step None or > 0 = islice
    if start_gt_0 and stop is None and step_gt_0:
        return islice(iterable, start, stop, step)

    # - start None or > 0, stop > 0, step None or > 0 = finite islice
    elif start_gt_0 and stop is not None and stop > 0 and step_gt_0:
        return islice(iterable, start, stop, step)

    # There could be valid cases with negative steps, for example if
    # reversed can be applied. But I won't go down that road!
    elif step_lt_0:
        raise ValueError('negative "step" is not possible.')

    # Any other combination requires the start to be not None and
    # negative.
    elif start_lt_0:
        # - start < 0, stop < 0, step None or > 0 = tail then islice.
        if stop_lt_0 and step_gt_0:
            it = tail(iterable, -start)
            it = islice(it, 0, stop-start, step)
            return it
        # - start < 0, stop None, step None = tail
        elif stop is None and step is None:
            it = tail(iterable, -start)
            return it
        # - start < 0, stop None, step > 0 = tail and islice
        elif stop is None and step > 0:
            it = tail(iterable, -start)
            it = islice(it, 0, None, step)
            return it
    else:
        raise ValueError('{0} cannot be subscripted with any '
                         'combination of negative "start", "stop" or '
                         '"step". This combination wasn\'t allowed.')


def insert(iterable, element, idx, unpack=False):
    """Insert one `element` into `iterable`.

    Parameters
    ----------
    iterable : iterable
        The `iterable` in which to insert the `element`.

    element : any type
        The `element` to insert to the `iterable`.

    idx : positive :py:class:`int` or :py:class:`str`
        The index at which to insert the `element`. If it's a string it must be
        ``'start'`` if the `element` should be prepended to `iterable` or
        ``'end'`` if it should be appended.

    unpack : :py:class:`bool`, optional
        If ``False`` the `element` is inserted as it is. If ``True`` then the
        `element` must be an iterable and it is unpacked into the `iterable`.
        Default is ``False``.

    Returns
    -------
    inserted : generator
        The `element` inserted into `iterable` at `idx` as generator.

    Examples
    --------
    To prepend a value::

        >>> from iteration_utilities import insert
        >>> list(insert(range(10), 100, 'start'))  # 'start' is equivalent to 0
        [100, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

    To append a value::

        >>> list(insert(range(10), 100, 'end'))
        [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100]

    Or to insert it at a given index::

        >>> list(insert(range(10), 100, 2))
        [0, 1, 100, 2, 3, 4, 5, 6, 7, 8, 9]

    It is also possible to unpack another iterable into another one with the
    `unpack` argument::

        >>> list(insert(range(10), [1, 2, 3], 0, unpack=True))
        [1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

    If the `unpack` argument is not given the iterable is inserted as it is::

        >>> list(insert(range(10), [1, 2, 3], 0))
        [[1, 2, 3], 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    """
    if not unpack:
        element = [element]

    it = iter(iterable)

    # TODO: Implement multiple indices at which to insert the item, this is
    #       quite nontrivial while supporting "start" and "end"...

    if idx == 'start':
        return chain(element, it)
    elif idx == 'end':
        return chain(it, element)
    else:
        return chain(islice(it, idx), element, it)


def replace(iterable, element, idx=None, start=None, stop=None, unpack=False):
    """Removes the item at `idx` or from `start` (inclusive) to `stop`
    (exclusive) and then inserts the `element` there.

    Parameters
    ----------
    iterable : iterable
        The iterable in which to replace the item(s).

    element : any type
        The element to insert after removing.

    idx : positive :py:class:`int`, list/tuple thereof, None, optional
        If not ``None``, remove the item at `idx` and insert `element` there.
        If it's a tuple or list the `element` is inserted at each of the
        indices in the `idx` (the values are sorted before, so the element is
        always inserted at the given indices).
        Default is ``None``.

        .. note::
           This parameter must not be ``None`` if also `start` and `stop` are
           ``None``.

    start : positive :py:class:`int` or None, optional
        If ``None`` then remove all items before `stop`, otherwise remove only
        the items starting by `start`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    stop : positive :py:class:`int` or None, optional
        If ``None`` then remove all items starting by `start`, otherwise only
        remove the items before `stop`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    unpack : :py:class:`bool`, optional
        If ``False`` the `element` is inserted as it is. If ``True`` then the
        `element` must be an iterable and it is unpacked into the `iterable`.
        Default is ``False``.

    Returns
    -------
    replaced : generator
        The `iterable` with the specified items removed and `element` inserted
        in their place.

    Examples
    --------
    To replace one item::

        >>> from iteration_utilities import replace
        >>> list(replace(range(10), 100, idx=2))
        [0, 1, 100, 3, 4, 5, 6, 7, 8, 9]

    To replace multiple items::

        >>> list(replace(range(10), 100, (3, 5, 1)))
        [0, 100, 2, 100, 4, 100, 6, 7, 8, 9]

    To replace slices::

        >>> list(replace(range(10), 100, start=2))
        [0, 1, 100]

        >>> list(replace(range(10), 100, stop=2))
        [100, 2, 3, 4, 5, 6, 7, 8, 9]

        >>> list(replace(range(10), 100, start=2, stop=5))
        [0, 1, 100, 5, 6, 7, 8, 9]
    """
    if idx is None and start is None and stop is None:
        raise TypeError('one of "idx", "start" or "stop" must be given.')

    if not unpack:
        element = [element]

    it = iter(iterable)

    if idx is not None:
        if not isinstance(idx, (list, tuple)):
            return chain(islice(it, idx), element, islice(it, 1, None))
        elif not idx:
            return iterable
        else:
            idx = sorted(idx)
            ret = []
            current = 0
            for num, i in enumerate(unique_justseen(idx)):
                if not num:
                    ret.append(islice(it, i))
                else:
                    ret.append(islice(it, 1, i-current))
                ret.append(element)
                current = i
            ret.append(islice(it, 1, None))
            return chain.from_iterable(ret)

    if start is not None and stop is not None:
        range_ = stop - start
        if range_ <= 0:
            raise ValueError('"stop" must be greater than "start".')
        return chain(islice(it, start), element, islice(it, range_, None))
    elif start is not None:
        return chain(islice(it, start), element)
    else:  # elif stop is not None!
        return chain(element, islice(it, stop, None))


def remove(iterable, idx=None, start=None, stop=None):
    """Removes the item at `idx` or from `start` (inclusive) to `stop`
    (exclusive).

    Parameters
    ----------
    iterable : iterable
        The iterable in which to remove the item(s).

    idx : positive :py:class:`int`, list/tuple thereof, None, optional
        If not ``None``, remove the item at `idx`. If it's a tuple or list then
        replace all the present indices (they will be sorted so only the
        specified indices are removed).
        Default is ``None``.

        .. note::
           This parameter must not be ``None`` if also `start` and `stop` are
           ``None``.

    start : positive :py:class:`int` or None, optional
        If ``None`` then remove all items before `stop`, otherwise remove only
        the items starting by `start`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    stop : positive :py:class:`int` or None, optional
        If ``None`` then remove all items starting by `start`, otherwise only
        remove the items before `stop`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    Returns
    -------
    replaced : generator
        The `iterable` with the specified items removed.

    Examples
    --------
    To remove one item::

        >>> from iteration_utilities import remove
        >>> list(remove(range(10), idx=2))
        [0, 1, 3, 4, 5, 6, 7, 8, 9]

    To remove several items just provide a tuple as idx (the values are sorted,
    so exactly the specified elements are removed)::

        >>> list(remove(range(10), (4, 6, 8, 5, 1)))
        [0, 2, 3, 7, 9]

    To remove a slice::

        >>> list(remove(range(10), start=2))
        [0, 1]

        >>> list(remove(range(10), stop=2))
        [2, 3, 4, 5, 6, 7, 8, 9]

        >>> list(remove(range(10), start=2, stop=5))
        [0, 1, 5, 6, 7, 8, 9]
    """
    if idx is None and start is None and stop is None:
        raise TypeError('one of "idx", "start" or "stop" must be given.')

    it = iter(iterable)

    if idx is not None:
        if not isinstance(idx, (list, tuple)):
            return chain(islice(it, idx), islice(it, 1, None))
        elif not idx:
            return iterable
        else:
            idx = sorted(idx)
            ret = []
            current = 0
            for num, i in enumerate(unique_justseen(idx)):
                if not num:
                    ret.append(islice(it, i))
                else:
                    ret.append(islice(it, 1, i-current))
                current = i
            ret.append(islice(it, 1, None))
            return chain.from_iterable(ret)

    if start is not None and stop is not None:
        range_ = stop - start
        if range_ < 0:
            raise ValueError('"stop" must be greater than or equal to '
                             '"start".')
        return chain(islice(it, start), islice(it, range_, None))
    elif start is not None:
        return islice(it, start)
    else:
        return islice(it, stop, None)
