from iteration_utilities import PY2

if PY2:
    from itertools import izip_longest as zip_longest
else:
    from itertools import zip_longest


def grouper(iterable, n, fillvalue=None):
    """Collect data into fixed-length chunks or blocks.

    Parameters
    ----------
    iterable : iterable
        Any `iterable` to group.

    n : :py:class:`int`
        The number of elements in each chunk.

    fillvalue : any type, optional
        The `fillvalue` if one group is not yet filled but the `iterable` is
        consumed.
        Default is ``None``.

    Returns
    -------
    groups : generator
        An `iterable` containing the groups/chunks as ``tuple``.

    Examples
    --------
    >>> from iteration_utilities import grouper
    >>> list(grouper('ABCDEFG', 3, 'x'))
    [('A', 'B', 'C'), ('D', 'E', 'F'), ('G', 'x', 'x')]
    """
    args = [iter(iterable)] * n
    if PY2:
        return zip_longest(fillvalue=fillvalue, *args)
    else:
        return zip_longest(*args, fillvalue=fillvalue)
