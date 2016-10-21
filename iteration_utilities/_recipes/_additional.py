"""
API: Additional recipes
-----------------------
"""
# Built-ins
from __future__ import absolute_import, division, print_function
from collections import Iterable
from itertools import chain, islice, repeat

# This module
from .. import PY2


if PY2:
    from itertools import imap as map


__all__ = ['append', 'cutout', 'deepflatten', 'itersubclasses', 'pad',
           'prepend', 'replicate', 'unpack']


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


def deepflatten(iterable, depth=None, types=Iterable, ignore=None):
    """Flatten an `iterable` with given `depth`.

    Parameters
    ----------
    iterable : iterable
        Any `iterable` to flatten.

    depth : int or None, optional
        Flatten `depth` levels of nesting or all if ``None``.
        Default is ``None``.

    types : type, iterable of types, optional
        Which types should be flattened. If the types are known and not
        collections base classes setting this parameter can **significantly**
        speedup the function.
        Default is ``collections.Iterable``.

    ignore : type, iterable of types or None, optional
        The types which should not be flattened. If ``None`` all `types` are
        flattened.
        Default is ``None``.

    Returns
    -------
    flattened_iterable : generator
        The `iterable` with the `depth` level of nesting flattened.

    Examples
    --------
    To flatten a given depth::

        >>> from iteration_utilities import deepflatten
        >>> list(deepflatten([1, [1,2], [[1,2]], [[[1,2]]]], depth=1))
        [1, 1, 2, [1, 2], [[1, 2]]]

    To completly flatten it::

        >>> list(deepflatten([1, [1,2], [[1,2]], [[[1,2]]]]))
        [1, 1, 2, 1, 2, 1, 2]

    To ignore for example dictionaries::

        >>> # Only the keys of a dictionary will be kept with deepflatten.
        >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}]))
        [1, 2, 1, 2, 1, 2]
        >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}], ignore=dict))
        [1, 2, 1, 2, {1: 10, 2: 10}]

    In this case we could have also chosen only to flatten the lists::

        >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}], types=list))
        [1, 2, 1, 2, {1: 10, 2: 10}]

    .. warning::
        If the iterable contains string-like objects you either need to set
        ``ignore=str`` or a `depth` that is not ``None``. Otherwise this will
        raise an ``RecursionError`` because each item in a string is itself a
        string!

    See for example::

        >>> list(deepflatten([1, 2, [1,2], 'abc'], depth=1))
        [1, 2, 1, 2, 'a', 'b', 'c']
        >>> list(deepflatten([1, 2, [1,2], 'abc'], ignore=str))
        [1, 2, 1, 2, 'abc']

    For Python2 you should ignore ``basestring`` instead of ``str``.
    """
    if ignore is None:
        ignore = ()
    if depth is None:
        # Use infinite depth so have no branching in the loop.
        depth = float('inf')

    # Need -1 because we don't want to yield the input sequence (this would
    # create another nesting level)
    if depth == -1:
        yield iterable
    else:
        for x in iterable:
            if isinstance(x, types) and not isinstance(x, ignore):
                # Python 3.3+ could use here:
                # yield from deepflatten(x, depth - 1, ignore)
                for item in deepflatten(x, depth - 1, types, ignore):
                    yield item
            else:
                yield x


def append(element, iterable):
    """Append one `element` on `iterable`.

    Parameters
    ----------
    element : any type
        The `element` to append to the `iterable`.

    iterable : iterable
        The `iterable`.

    Returns
    -------
    appended : generator
        The `iterable` followed by `element` as generator.

    Examples
    --------
    Some simple examples::

        >>> from iteration_utilities import append
        >>> list(append(10, range(3)))
        [0, 1, 2, 10]

        >>> list(append(0, []))
        [0]
    """
    return chain(iterable, [element])


def prepend(element, iterable):
    """Prepend one `element` on `iterable`.

    Parameters
    ----------
    element : any type
        The `element` to prepend to the `iterable`.

    iterable : iterable
        The `iterable`.

    Returns
    -------
    prepended : generator
        The `element` followed by `iterable` as generator.

    Examples
    --------
    Some simple examples::

        >>> from iteration_utilities import prepend
        >>> list(prepend(10, range(3)))
        [10, 0, 1, 2]

        >>> list(prepend(0, []))
        [0]
    """
    return chain([element], iterable)


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


def cutout(iterable, start, stop):
    """Removes the items from start (inclusive) to stop (exclusive) from the
    `iterable`.

    Parameters
    ----------
    iterable : iterable
        The iterable from which to remove the items.

    start : positive integer
        The index from which to remove the elements.

    stop : positive integer
        Remove the items till this index. The item at the stop index is **not**
        removed.

    Returns
    -------
    residuals : generator
        The values from `iterable` except those starting at index `start` to
        `stop`.

    Examples
    --------
    A simple example::

        >>> from iteration_utilities import cutout
        >>> list(cutout(range(10), 2, 5))
        [0, 1, 5, 6, 7, 8, 9]

    This is the equivalent to the removing by slicing::

        >>> lst = list(range(10))
        >>> del lst[2:5]
        >>> lst
        [0, 1, 5, 6, 7, 8, 9]
    """
    iterable = iter(iterable)
    return chain(islice(iterable, 0, start),
                 islice(iterable, stop-start, None))


def unpack(iterable, into, idx):
    """Insert an `iterable` `into` another at the given `idx`.

    Parameters
    ----------
    iterable : iterable
        The iterable to insert.

    into : iterable
        The iterable in which `iterable` is inserted.

    idx : positive integer
        The index before which the `iterable` is inserted.

    Returns
    -------
    inserted : generator
        The iterable with `iterable` inserted into `into`.

    Examples
    --------
    A simple example::

        >>> from iteration_utilities import unpack
        >>> list(unpack(range(3), range(10), 3))
        [0, 1, 2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

    This is the equivalent to inserting an iterable with slicing into a list::

        >>> lst = list(range(10))
        >>> lst[3:3] = range(3)
        >>> lst
        [0, 1, 2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    """
    into = iter(into)
    return chain(islice(into, idx), iterable, into)
