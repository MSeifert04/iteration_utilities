from itertools import tee
from iteration_utilities import consume, PY2

if PY2:
    from itertools import izip as zip


def successive(iterable, times=2):
    """Like :py:func:`~._core.pairwise` but allows to get an arbitary number
    of successive elements.

    Parameters
    ----------
    iterable : iterable
        Get the successive elements from this `iterable`.

    times : integer, optional
        The number of successive elements.
        Default is ``2``.

    Returns
    -------
    successive_elements : generator
        The successive elements as generator. Each element of the generator
        is a tuple containing `times` successive elements.

    Examples
    --------
    Like :py:func:`~._core.pairwise` get every item and the following with the
    default `times`::

        >>> from iteration_utilities import successive
        >>> list(successive(range(5)))
        [(0, 1), (1, 2), (2, 3), (3, 4)]

    Varying the `times` can give you also 3 successive elements::

        >>> list(successive(range(5), times=3))
        [(0, 1, 2), (1, 2, 3), (2, 3, 4)]
        >>> list(successive('Hello!', times=2))
        [('H', 'e'), ('e', 'l'), ('l', 'l'), ('l', 'o'), ('o', '!')]
    """
    iterable = iter(iterable)
    its = tee(iterable, times)
    for idx, it in enumerate(its):
        consume(it, idx)
    return zip(*its)
