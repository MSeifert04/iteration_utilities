# Built-ins
from __future__ import absolute_import, division, print_function
from collections import Counter
from gc import get_objects
from weakref import ref


def memory_leak(func, specific_object=None, exclude_object=ref):
    """Compares the number of tracked python objects before and after a
    function call and returns a dict containing differences.

    Parameters
    ----------
    func : callable
        The function that should be tested. Shouldn't return anything!

    specific_object : type or None, optional
        Test all tracked types (None) or only one specific type.
        Default is ``None``.

    exclude_object : type or None, optional
        Exclude specific types or use all (None).
        Default is ``weakref.ref``.

    Returns
    -------
    difference : collections.Counter
        A Counter containing the types after the function call minus the ones
        before the function call. If the function doesn't return anything this
        Counter should be empty. If it contains types the function probably
        contains a memory leak.

    Notes
    -----
    It is convenient to wrap the explicit call inside a function that doesn't
    return. To enhance this useage the ``memory_leak`` function doesn't allow
    to pass arguments to the ``func``!
    """
    # Create Counter before listing the objects otherwise they would
    # be recognized as leak.
    before = Counter()
    after = Counter()

    # Tracked objects before the function call
    before.update(map(type, get_objects()))

    func()

    # Tracked objects after the function call
    after.update(map(type, get_objects()))

    # Return the difference of all types the specified type.
    if specific_object is None:
        result = after - before
        if exclude_object is not None:
            if exclude_object in result:
                del result[exclude_object]
        return result
    else:
        leftover = after[specific_object] - before[specific_object]
        if leftover:
            return Counter({specific_object: leftover})
        else:
            return Counter()
