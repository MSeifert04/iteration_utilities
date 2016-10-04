from iteration_utilities import PY2

if PY2:
    from itertools import imap as map


def quantify(iterable, pred=bool):
    """Count how many times the predicate is true.

    Parameters
    ----------
    iterable : iterable
        Any `iterable` to count in.

    pred : callable, optional
        Predicate to test.
        Default is :py:class:`bool`.

    Returns
    -------
    number : number
        The numer of times the predicate is ``True``.

    Examples
    --------
    >>> from iteration_utilities import quantify
    >>> quantify([0,0,'',{}, [], 2])
    1

    >>> def smaller5(val): return val < 5
    >>> quantify([1,2,3,4,5,6,6,7], smaller5)
    4
    """
    return sum(map(pred, iterable))
