from iteration_utilities import PY2, tail

if PY2:
    from itertools import ifilter as filter


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
