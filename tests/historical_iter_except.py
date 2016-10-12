def iter_except(func, exception, first=None):
    """Call a function repeatedly until an `exception` is raised.

    Converts a call-until-exception interface to an iterator interface.
    Like ``iter(func, sentinel)`` but uses an `exception` instead of a sentinel
    to end the loop.

    Parameters
    ----------
    func : callable
        The function that is called until `exception` is raised.

    exception : Exception
        The `exception` which terminates the iteration.

    first : callable or None, optional
        If not ``None`` this function is called once before the `func` is
        executed.

    Returns
    -------
    result : generator
        The result of the `func` calls as generator.

    Examples
    --------
    >>> from iteration_utilities import iter_except
    >>> from collections import OrderedDict

    >>> d = OrderedDict([('a', 1), ('b', 2)])
    >>> list(iter_except(d.popitem, KeyError))
    [('b', 2), ('a', 1)]

    .. note::
        ``d.items()`` would yield the same result. At least with Python3.

    >>> from math import sqrt
    >>> import sys

    >>> g = (sqrt(i) for i in [5, 4, 3, 2, 1, 0, -1, -2, -3])
    >>> func = g.next if sys.version_info.major == 2 else g.__next__
    >>> def say_go():
    ...     return 'go'
    >>> list(iter_except(func, ValueError, say_go))
    ['go', 2.23606797749979, 2.0, 1.7320508075688772, 1.4142135623730951, 1.0\
, 0.0]

    Notes
    -----
    Further examples:

    - ``bsddbiter = iter_except(db.next, bsddb.error, db.first)``
    - ``heapiter = iter_except(functools.partial(heappop, h), IndexError)``
    - ``dictiter = iter_except(d.popitem, KeyError)``
    - ``dequeiter = iter_except(d.popleft, IndexError)``
    - ``queueiter = iter_except(q.get_nowait, Queue.Empty)``
    - ``setiter = iter_except(s.pop, KeyError)``
    """
    try:
        if first is not None:
            yield first()
        while 1:
            yield func()
    except exception:
        pass
