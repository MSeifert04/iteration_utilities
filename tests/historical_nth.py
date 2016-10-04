from itertools import islice


def nth(iterable, n, default=None):
    """Returns the `n`-th item or a `default` value.

    Parameters
    ----------
    iterable : iterable
        The `iterable` from which to take the item.

    n : :py:class:`int`
        Index of the item.

    default : any type, optional
        `Default` value if the iterable doesn't contain the index.
        Default is ``None``.

    Returns
    -------
    nth_item : any type
        The `n`-th item of the `iterable` or `default` if the index wasn't
        present in the `iterable`.

    Examples
    --------
    Without `default` value::

        >>> from iteration_utilities import nth
        >>> g = (x**2 for x in range(10))
        >>> nth(g, 5)
        25

    Or with `default` if the index is not present::

        >>> g = (x**2 for x in range(10))
        >>> nth(g, 15, 0)
        0
    """
    return next(islice(iterable, n, None), default)
