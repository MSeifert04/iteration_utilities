from itertools import cycle, islice

from iteration_utilities import PY2


def roundrobin(*iterables):
    """Round-Robin implementation ([0]_).

    Parameters
    ----------
    iterables : iterable
        `Iterables` to combine using the round-robin. Any amount of iterables
        are supported.

    Returns
    -------
    roundrobin : generator
        Iterable filled with the values of the `iterables`.

    Examples
    --------
    >>> from iteration_utilities import roundrobin
    >>> list(roundrobin('ABC', 'D', 'EF'))
    ['A', 'D', 'E', 'B', 'F', 'C']

    References
    ----------
    .. [0] https://en.wikipedia.org/wiki/Round-robin_scheduling
    """
    # Recipe credited to George Sakkis
    pending = len(iterables)
    if PY2:
        nexts = cycle(iter(it).next for it in iterables)
    else:
        nexts = cycle(iter(it).__next__ for it in iterables)
    while pending:
        try:
            for next in nexts:
                yield next()
        except StopIteration:
            pending -= 1
            nexts = cycle(islice(nexts, pending))
