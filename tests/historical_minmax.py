from iteration_utilities import PY2

if PY2:
    from itertools import izip_longest as zip_longest
else:
    from itertools import zip_longest


def _minmax(iterable, key=None, default=None):
    """Historical function, by now it is replaced by a C-function.
    """
    it = iter(iterable)

    try:
        lo = hi = next(it)
    except StopIteration:
        if default is None:
            raise ValueError('minmax() arg is an empty sequence')
        return default

    # Different branches depending on the presence of key. This saves a lot
    # of unimportant copies which would slow the "key=None" branch
    # significantly down.
    if key is None:
        for x, y in zip_longest(it, it, fillvalue=lo):
            if x > y:
                x, y = y, x
            if x < lo:
                lo = x
            if y > hi:
                hi = y

    else:
        lo_key = hi_key = key(lo)

        for x, y in zip_longest(it, it, fillvalue=lo):

            x_key, y_key = key(x), key(y)

            if x_key > y_key:
                x, y, x_key, y_key = y, x, y_key, x_key
            if x_key < lo_key:
                lo, lo_key = x, x_key
            if y_key > hi_key:
                hi, hi_key = y, y_key

    return lo, hi
