def partition(iterable, pred):
    """Use a predicate to partition entries into ``False`` entries and ``True``
    entries.

    Parameters
    ----------
    iterable : iterable
        `Iterable` to partition.

    pred : callable
        The predicate which determines the group.

    Returns
    -------
    false_values : list
        An list containing the values for which the predicate was False.

    true_values : list
        An list containing the values for which the predicate was True.

    See also
    --------
    ._core.ipartition : Generator variant of partition.

    Examples
    --------
    >>> from iteration_utilities import partition
    >>> def is_odd(val): return val % 2
    >>> partition(range(10), is_odd)
    ([0, 2, 4, 6, 8], [1, 3, 5, 7, 9])

    .. note::
        While ``ipartition`` also allows ``pred=None`` this is not allowed
        for ``partition``.

    .. warning::
        In case the `pred` is expensive then ``partition`` can be noticable
        faster than ``ipartition``.
    """
    falsy, truthy = [], []
    falsy_append, truthy_append = falsy.append, truthy.append

    for item in iterable:
        if pred(item):
            truthy_append(item)
        else:
            falsy_append(item)

    return falsy, truthy
