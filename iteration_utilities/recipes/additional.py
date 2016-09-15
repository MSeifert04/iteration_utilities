"""
Additional recipes
^^^^^^^^^^^^^^^^^^
"""
from __future__ import absolute_import, division, print_function
from collections import Iterable
import sys

from .core import tail


PY2 = sys.version_info.major == 2


__all__ = ['applyfunc', 'deepflatten', 'last_true', 'merge']


def applyfunc(func, value, *args, **kwargs):
    """Successivly apply `func` on `value`.

    Parameters
    ----------
    func : callable
        The function to apply. The `value` is given as first argument.

    value : any type
        The value on which to apply the function.

    args, kwargs :
        Additional parameter for `func`.

    Returns
    -------
    results : generator
        The result of the successivly applied `func`.

    See also
    --------
    .core.repeatfunc : Repeatedly call a function.

    .core.tabulate : Repeatedly call a function on an incrementing value.

    Examples
    --------
    The first element is the initial `value` and the next elements are
    the result of ``func(value)``, then ``func(func(value))``, ...::

        >>> from iteration_utilities import take, applyfunc
        >>> import math
        >>> take(applyfunc(math.sqrt, 10), 4)
        [10, 3.1622776601683795, 1.7782794100389228, 1.333521432163324]

    It is also possible to supply additional arguments to the `func`::

        >>> take(applyfunc(pow, 2, 2), 5)
        [2, 4, 16, 256, 65536]

    .. warning::
        This will return an infinitly long generator so do **not** try to do
        something like ``list(applyfunc())``!
    """
    # It would be nice to specify if the original value should be yielded
    # but that's almost impossible because Python2 doesn't allows keyword-only
    # parameters.
    yield value

    while True:
        value = func(value, *args, **kwargs)
        yield value


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
    :py:func:`~iteration_utilities.recipes.core.first_true`.

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

    See also
    --------
    .core.flatten : Flatten one level of nesting.

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


def _merge(*iterables, **kwargs):
    """Merge sorted `iterables` into one.

    Parameters
    ----------
    iterables : iterable
        Any amount of already sorted `iterable` objects.

    key : callable or None, optional
        If ``None`` compare the elements themselves otherwise compare the
        result of ``key(element)``, like the `key` parameter for
        :py:func:`sorted`.
        Default is ``None``.

    reverse : boolean, optional
        If ``True`` then sort decreasing otherwise sort in increasing order.
        Default is ``False``.

    Returns
    -------
    merged : generator
        The sorted merged iterables as generator.

    See also
    --------
    heapq.merge : Equivalent and faster since Python 3.5 but earlier versions
        do not support the `key` or `reverse` argument.

    sorted : ``sorted(itertools.chain(*iterables))`` supports the same options
        and is much faster but returns a sequence instead of a generator.

    Examples
    --------
    To merge multiple sorted `iterables`::

        >>> from iteration_utilities import merge
        >>> list(merge([1, 3, 5, 7, 9], [2, 4, 6, 8, 10]))
        [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

    It's stable and allows a `key` function::

        >>> list(merge([(1, 3), (3, 3)], [(-1, 3), (-3, 3)], key=lambda x: abs(x[0])))
        [(1, 3), (-1, 3), (3, 3), (-3, 3)]

    Also possible to `reverse` (biggest to smallest order) the merge::

        >>> list(merge([5,1,-8], [10, 2, 1, 0], reverse=True))
        [10, 5, 2, 1, 1, 0, -8]

    But also more than two `iterables`::

        >>> list(merge([1, 10, 11], [2, 9], [3, 8], [4, 7], [5, 6], range(10)))
        [0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11]

    However if the `iterabes` are not sorted the result will be unsorted
    (partially sorted)::

        >>> list(merge(range(10), [6,1,3,2,6,1,6]))
        [0, 1, 2, 3, 4, 5, 6, 6, 1, 3, 2, 6, 1, 6, 7, 8, 9]
    """
    key = kwargs.pop('key', None)
    reverse = kwargs.pop('reverse', None)

    def next_or_remove(iterables, current, idx, key):
        """Get the next element of the `idx`-th iterable or if that iterable is
        exhausted remove the iterable from the list of iterables and the list
        of the "current" first items.
        """
        try:
            x = next(iterables[idx])
        except StopIteration:
            # StopIteration means the iterable is exhausted so we can
            # remove it from the current and iterables list.
            del iterables[idx]
            del current[idx]
        else:
            if key is None:
                current[idx] = x
            else:
                # To calculate key(x) only once for each element wrap it as
                # a tuple. So that this doesn't break the stability criterion
                # we add the idx as second parameter so the original won't be
                # take part in the min/max.
                current[idx] = (key(x), idx, x)

    func = max if reverse else min

    # To use "next" we need iterators.
    iterables = [iter(i) for i in iterables]
    # The current first elements (which are to be compared) as placeholder
    current = [None] * len(iterables)

    # We need the index of the iterables to keep the merge stable and
    # it might be that any  iterable is empty so we need to iterate over
    # them in reverse.
    for i, _ in enumerate(reversed(iterables)):
        next_or_remove(iterables, current, i, key)

    while iterables:
        next_item = func(current)
        idx_next_item = current.index(next_item)
        if key is None:
            yield next_item
        else:
            yield next_item[2]
        next_or_remove(iterables, current, idx_next_item, key)


if PY2:
    merge = _merge
else:
    # For a better function signature.
    py3_version = """
def merge(*iterables, key=None, reverse=False):
    return _merge(*iterables, **{'key': key, 'reverse': reverse})
merge.__doc__ = _merge.__doc__
    """
    exec(py3_version)
