from operator import itemgetter

from iteration_utilities import PY2

if PY2:
    from itertools import imap as map, groupby


def unique_justseen(iterable, key=None):
    """List unique elements, preserving order. Remember only the element just \
            seen.

    Parameters
    ----------
    iterable : iterable
        `Iterable` to check.

    key : callable or None, optional
        If ``None`` the values are taken as they are. If it's a callable the
        callable is applied to the value before comparing it.
        Default is ``None``.

    Returns
    -------
    iterable : generator
        An iterable containing all unique values just seen in the `iterable`.

    Examples
    --------
    >>> from iteration_utilities import unique_justseen
    >>> list(unique_justseen('AAAABBBCCDAABBB'))
    ['A', 'B', 'C', 'D', 'A', 'B']

    >>> list(unique_justseen('ABBCcAD', str.lower))
    ['A', 'B', 'C', 'A', 'D']
    """
    return map(next, map(itemgetter(1), groupby(iterable, key)))
