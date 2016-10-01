from iteration_utilities import PY2

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

        >>> seq1 = [(1, 3), (3, 3)]
        >>> seq2 = [(-1, 3), (-3, 3)]
        >>> list(merge(seq1, seq2, key=lambda x: abs(x[0])))
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