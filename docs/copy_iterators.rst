Iterators and copy
------------------

Often it is simply not necessary to make a copy of an iterator. However there
are certain cases where a copy is wanted. In these cases there three major
options:

- :py:func:`itertools.tee`
- :py:mod:`pickle`
- :py:func:`copy.deepcopy`

Given these three possibilities the iterators in :py:mod:`iteration_utilities`
should **never** be copied using :py:func:`copy.copy`. Generally
:py:func:`itertools.tee` should be the best choice except when file
serialization is needed. Then :py:mod:`pickle` should be used. The
:py:func:`copy.deepcopy` function makes, like ``pickle`` a deep (recursive)
copy but doesn't support file serialization.

A simple example using :py:func:`itertools.tee`::

    >>> from iteration_utilities import roundrobin
    >>> from itertools import tee
    >>> # Create an iterator
    >>> a = roundrobin([1, 2, 3], [4, 5], [6])
    >>> # Create a new iterator
    >>> # Overwrite "a" because the input for "tee" shouldn't be reused after
    >>> # the call to "tee".
    >>> a, b = tee(a, 2)
    >>> next(a)
    1
    >>> next(b)
    1
