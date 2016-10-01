"""
Additional recipes
^^^^^^^^^^^^^^^^^^
"""
# Built-ins
from __future__ import absolute_import, division, print_function
from collections import Iterable

# This module
from ._core import tail
from .. import PY2

if PY2:
    from itertools import ifilter as filter


__all__ = ['deepflatten', 'itersubclasses', 'last_true']


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


def last_true(iterable, default=False, pred=None):
    """Returns the last true value in the `iterable` or `default`.

    Parameters
    ----------
    iterable : iterable
        The `iterable` for which to determine the last true value.

    default : any type, optional
        The `default` value if no true value was found.
        Default is ``False``.

    pred : callable or `None`, optional
        If ``None`` find the last true value. Otherwise find the last value
        for which ``pred(value)`` is true.
        Default is ``None``.

    Returns
    -------
    last : any type
        The last true value or the last value for which `pred` is true.
        If there is no such value then `default` is returned.

    Notes
    -----
    If the `iterable` is a sequence (i.e. ``list`` or ``tuple``) a more
    efficient way would be to use :py:func:`reversed` and
    :py:func:`~iteration_utilities._cfuncs.first`.

    Examples
    --------
    >>> from iteration_utilities import last_true
    >>> last_true([0, '', tuple(), 10])
    10
    >>> # Last odd number
    >>> last_true([0, 2, 3, 5, 8, 10], pred=lambda x: x%2)
    5
    >>> last_true([0, 0, 0, 0])
    False
    >>> # default value if no true value.
    >>> last_true([0, 0, 0, 0], default=100)
    100
    """
    return next(tail(filter(pred, iterable), 1), default)


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
