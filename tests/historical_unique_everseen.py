# This module
from .. import PY2

# Replace list-generating functions by generator functions
if PY2:
    from itertools import ifilterfalse as filterfalse
else:
    from itertools import filterfalse


def unique_everseen(iterable, key=None):
    """List unique elements, preserving order. Remember all elements ever seen.

    Parameters
    ----------
    iterable : iterable
        `Iterable` containing the elements.

    key : callable or None, optional
        If ``None`` the values are taken as they are. If it's a callable the
        callable is applied to the value before comparing it.
        Default is ``None``.

    returns
    -------
    iterable : generator
        An iterable containing all unique values ever seen in the `iterable`.

    Examples
    --------
    >>> from iteration_utilities import unique_everseen
    >>> list(unique_everseen('AAAABBBCCDAABBB'))
    ['A', 'B', 'C', 'D']

    >>> list(unique_everseen('ABBCcAD', str.lower))
    ['A', 'B', 'C', 'D']
    """
    seen = set()
    seen_add = seen.add
    if key is None:
        for element in filterfalse(seen.__contains__, iterable):
            seen_add(element)
            yield element
    else:
        for element in iterable:
            k = key(element)
            if k not in seen:
                seen_add(k)
                yield element
