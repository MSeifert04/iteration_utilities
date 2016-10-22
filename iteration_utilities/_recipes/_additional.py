"""
API: Additional recipes
-----------------------
"""
# Built-ins
from __future__ import absolute_import, division, print_function
from itertools import chain, islice, repeat

# This module
from .. import PY2


if PY2:
    from itertools import imap as map


__all__ = ['insert', 'itersubclasses', 'pad', 'remove', 'replace', 'replicate']


def itersubclasses(cls, seen=None):
    """Iterate over the subclasses of `cls`. Recipe based on the snippet
    of Gabriel Genellina ([0]_) but modified.

    Parameters
    ----------
    cls : class
        The class for which to iterate over the subclasses.

    seen : set or None, optional
        Classes to exclude from iteration or ``None`` if all subclasses should
        be returned.
        Default is ``None``.

    Returns
    -------
    subclasses : generator
        The subclasses of `cls`.

    Examples
    --------
    To get all subclasses for a ``set``::

        >>> from iteration_utilities import itersubclasses
        >>> list(itersubclasses(set))
        []

    It even works with custom classes and diamond structures::

        >>> class A(object): pass
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

    And it also works for objects subclassing ``type``::

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
    except AttributeError:
        # old-style class has no __subclasses__ attribute
        raise TypeError('old-style "cls" ({0}) is not supported.'.format(cls))

    # This part is some combination of unique_everseen and flatten, however
    # I did not found a way to use these here.
    for sub in subs:
        if sub not in seen:
            seen.add(sub)
            yield sub
            # Could also use "yield from itersubclasses(sub, seen)" in
            # Python3.3+
            for sub in itersubclasses(sub, seen):
                yield sub


def pad(iterable, fillvalue=None, nlead=0, ntail=0):
    """Pad the `iterable` with `fillvalue` in front and behind.

    Parameters
    ----------
    iterable : iterable
        The `iterable` to pad.

    fillvalue : any type, optional
        The padding value.
        Default is ``None``.

    nlead, ntail : int or None, optional
        The number of times to pad in front (`nlead`) and after (`ntail`) the
        `iterable. If `ntail` is ``None`` pad indefinitly (not possible for
        `nlead`).
        Default is ``0``.

    Returns
    -------
    padded_iterable : generator
        The padded `iterable`.

    Examples
    --------
    >>> from iteration_utilities import pad, take
    >>> list(pad([1,2,3], 0, 5))
    [0, 0, 0, 0, 0, 1, 2, 3]

    >>> list(pad([1,2,3], 0, ntail=5))
    [1, 2, 3, 0, 0, 0, 0, 0]

    >>> list(pad([1,2,3], 0, nlead=5, ntail=5))
    [0, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0, 0, 0]

    >>> take(pad([1,2,3], 0, ntail=None), 10)
    [1, 2, 3, 0, 0, 0, 0, 0, 0, 0]

    .. warning::
        This will return an infinitly long generator if either ``before`` or
        ``after`` are ``None``, so do not try to do something like
        ``list(pad([], before=None))``!
    """
    prepend = repeat(fillvalue, nlead)

    if ntail is None:
        append = repeat(fillvalue)
    else:
        append = repeat(fillvalue, ntail)

    return chain(prepend, iterable, append)


def replicate(iterable, times):
    """Replicates each item in the `iterable` for `times` times.

    Parameters
    ----------
    iterable : iterable
        The iterable which contains the elements to be replicated.

    times : positive integer
        The number of `times` each element is replicated.

    Returns
    -------
    repeated_iterable : generator
        A generator containing the replicated items from `iterable`.

    Examples
    --------
    >>> from iteration_utilities import replicate
    >>> ''.join(replicate('abc', 3))
    'aaabbbccc'

    >>> list(replicate(range(3), 5))
    [0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2]
    """
    return chain.from_iterable(map(repeat, iterable, repeat(times)))


# =============================================================================
# List-like interface methods
#
# insert: list[x:x] = item
# replace: list[x:y] = item
# remove: del list[x:y]
#
# =============================================================================


def insert(iterable, element, idx, unpack=False):
    """Insert one `element` into `iterable`.

    Parameters
    ----------
    iterable : iterable
        The `iterable` in which to insert the `element`.

    element : any type
        The `element` to insert to the `iterable`.

    idx : positive integer or str
        The index at which to insert the `element`. If it's a string it must be
        ``'start'`` if the `element` should be prepended to `iterable` or
        ``'end'`` if it should be appended.

    unpack : bool, optional
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

    idx : positive integer or None, optional
        If not ``None``, remove the item at `idx` and insert `element` there.
        Default is ``None``.

        .. note::
           This parameter must not be ``None`` if also `start` and `stop` are
           ``None``.

    start : positive integer or None, optional
        If ``None`` then remove all items before `stop`, otherwise remove only
        the items starting by `start`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    stop : positive integer or None, optional
        If ``None`` then remove all items starting by `start`, otherwise only
        remove the items before `stop`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    unpack : bool, optional
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
        return chain(islice(it, idx), element, islice(it, 1, None))

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

    idx : positive integer or None, optional
        If not ``None``, remove the item at `idx`.
        Default is ``None``.

        .. note::
           This parameter must not be ``None`` if also `start` and `stop` are
           ``None``.

    start : positive integer or None, optional
        If ``None`` then remove all items before `stop`, otherwise remove only
        the items starting by `start`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    stop : positive integer or None, optional
        If ``None`` then remove all items starting by `start`, otherwise only
        remove the items before `stop`.
        Default is ``None``.

        .. note::
           This parameter is ignored if `idx` is not ``None``.

    Returns
    -------
    replaced : generator
        The `iterable` with the specified items removed and `element` inserted
        in their place.

    Examples
    --------
    To replace one item::

        >>> from iteration_utilities import remove
        >>> list(remove(range(10), idx=2))
        [0, 1, 3, 4, 5, 6, 7, 8, 9]

    To replace multiple items::

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
        return chain(islice(it, idx), islice(it, 1, None))

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
