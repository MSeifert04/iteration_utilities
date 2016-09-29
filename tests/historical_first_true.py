from iteration_utilities import PY2

if PY2:
    from itertools import ifilter as filter


def first_true(iterable, default=False, pred=None):
    """Returns the first true value in the `iterable` or `default`.

    Parameters
    ----------
    iterable : iterable
        The `iterable` for which to determine the first true value.

    default : any type, optional
        The `default` value if no true value was found.
        Default is ``False``.

    pred : callable or None, optional
        If ``None`` find the first true value. Otherwise find the first value
        for which ``pred(value)`` is ``True``.
        Default is ``None``.

    Returns
    -------
    first : any type
        The first true value or the first value for which `pred` is true.
        If there is no such value then `default` is returned.

    Examples
    --------
    >>> from iteration_utilities import first_true
    >>> first_true([0, '', tuple(), 10])
    10

    >>> # First odd number
    >>> first_true([0, 2, 3, 5, 8, 10], pred=lambda x: x%2)
    3

    >>> first_true([0, 0, 0, 0])
    False

    >>> # default value if no true value
    >>> first_true([0, 0, 0, 0], default=100)
    100
    """
    return next(filter(pred, iterable), default)
