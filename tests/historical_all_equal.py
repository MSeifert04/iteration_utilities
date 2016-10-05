from itertools import groupby


def all_equal(iterable):
    """Checks if all the elements are equal to each other.

    Parameters
    ----------
    iterable : iterable
        Any `iterable` to test.

    Returns
    -------
    all_equal : :py:class:`bool`
        ``True`` if all elements in `iterable` are equal or ``False`` if not.

    Notes
    -----
    If the input is empty the function returns ``True``.

    Examples
    --------
    >>> from iteration_utilities import all_equal
    >>> all_equal([1,1,1,1,1,1,1,1,1])
    True

    >>> all_equal([1,1,1,1,1,1,1,2,1])
    False
    """
    g = groupby(iterable)
    return next(g, True) and not next(g, False)
