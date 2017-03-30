# Licensed under Apache License Version 2.0 - see LICENSE

"""
This module contains callable test cases.
"""
# Built-ins
import copy

# 3rd party
import pytest


def iterator_copy(thing):
    """Normal copies are not officially supported but ``itertools.tee`` uses
    ``__copy__`` if implemented it is either forbid both or none. Given that
    ``itertools.tee`` is a very useful function ``copy.copy`` is allowed but
    no garantuees are made. This function just makes sure they can be copied
    and the result has at least one item in it (call ``next`` on it)"""
    # Even though normal copies are discouraged they should be possible.
    # Cannot do "list" because it may be infinite :-)
    next(copy.copy(thing))


def iterator_setstate_list_fail(thing):
    with pytest.raises(TypeError) as exc:
        thing.__setstate__([])
    assert 'tuple' in str(exc) and 'list' in str(exc)


def iterator_setstate_empty_fail(thing):
    with pytest.raises(TypeError) as exc:
        thing.__setstate__(())
    assert '0 given' in str(exc)
