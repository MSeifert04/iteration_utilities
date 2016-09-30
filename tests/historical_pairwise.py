from itertools import tee
from iteration_utilities import PY2

if PY2:
    from itertools import izip as zip


def pairwise(iterable):
    """s -> (s0,s1), (s1,s2), (s2, s3), ...

    Parameters
    ----------
    iterable : iterable
        Any `iterable` to pairwise combine.

    Returns
    -------
    pairwise : generator
        An `iterable` containing tuples of sucessive elements of the iterable.

    Examples
    --------
    >>> from iteration_utilities import pairwise
    >>> list(pairwise([1,2,3]))
    [(1, 2), (2, 3)]
    """
    a, b = tee(iterable)
    next(b, None)
    return zip(a, b)
