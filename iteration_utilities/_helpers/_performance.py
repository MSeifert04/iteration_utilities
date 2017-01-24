"""
API: Performant helpers
-----------------------
"""
# Licensed under Apache License Version 2.0 - see LICENSE.rst

# Built-ins
from __future__ import absolute_import, division, print_function
from itertools import repeat

# This module
from .. import EQ_PY2


if EQ_PY2:
    from itertools import imap as map


__all__ = ['all_isinstance', 'any_isinstance']


def all_isinstance(iterable, types):
    """Like :py:func:`isinstance` but checks if all items in `iterable` are
    instances of `types`.

    Parameters
    ----------
    iterable : iterable
        Each item of the `iterable` is tested with ``isinstance(item, types)``.

    types : object or tuple of objects
        Test for this type if it's a single class or test if the item is of any
        of the types (if types is a tuple).

    Returns
    -------
    all : bool
        ``True`` if all elements in `iterable` are instances of `types`,
        ``False`` if not.

    Examples
    --------
    This function is equivalent (but faster) than
    ``all(isinstance(item, types) for item in iterable)``::

        >>> from iteration_utilities import all_isinstance
        >>> all_isinstance(range(100), int)
        True

        >>> all_isinstance([1, 2, 3.2], (int, float))
        True

    .. warning::
        This function returns ``True`` if the `iterable` is empty.
    """
    return all(map(isinstance, iterable, repeat(types)))


def any_isinstance(iterable, types):
    """Like :py:func:`isinstance` but checks if any item in `iterable` is an
    instance of `types`.

    Parameters
    ----------
    iterable : iterable
        Each item of the `iterable` is tested with ``isinstance(item, types)``.

    types : object or tuple of objects
        Test for this type if it's a single class or test if the item is of any
        of the types (if types is a tuple).

    Returns
    -------
    all : bool
        ``True`` if any elements in `iterable` is an instance of `types`,
        ``False`` if not.

    Examples
    --------
    This function is equivalent (but faster) than
    ``any(isinstance(item, types) for item in iterable)``

        >>> from iteration_utilities import any_isinstance
        >>> all_isinstance(range(100), int)
        True

        >>> any_isinstance([1, 2, 3.2], float)
        True
    """
    return any(map(isinstance, iterable, repeat(types)))
