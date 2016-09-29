def applyfunc(func, value, *args, **kwargs):
    """Successivly apply `func` on `value`.

    Parameters
    ----------
    func : callable
        The function to apply. The `value` is given as first argument.

    value : any type
        The value on which to apply the function.

    args, kwargs :
        Additional parameter for `func`.

    Returns
    -------
    results : generator
        The result of the successivly applied `func`.

    See also
    --------
    ._core.repeatfunc : Repeatedly call a function.

    ._core.tabulate : Repeatedly call a function on an incrementing value.

    Examples
    --------
    The first element is the initial `value` and the next elements are
    the result of ``func(value)``, then ``func(func(value))``, ...::

        >>> from iteration_utilities import take, applyfunc
        >>> import math
        >>> take(applyfunc(math.sqrt, 10), 4)
        [10, 3.1622776601683795, 1.7782794100389228, 1.333521432163324]

    It is also possible to supply additional arguments to the `func`::

        >>> take(applyfunc(pow, 2, 2), 5)
        [2, 4, 16, 256, 65536]

    .. warning::
        This will return an infinitly long generator so do **not** try to do
        something like ``list(applyfunc())``!
    """
    # It would be nice to specify if the original value should be yielded
    # but that's almost impossible because Python2 doesn't allows keyword-only
    # parameters.
    yield value

    while True:
        value = func(value, *args, **kwargs)
        yield value
