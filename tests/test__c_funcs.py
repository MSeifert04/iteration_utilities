# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper


def test_other_c_funcs():
    assert iteration_utilities.return_True()
    assert not iteration_utilities.return_False()
    assert iteration_utilities.return_None() is None
    assert iteration_utilities.return_identity(1) == 1
    assert iteration_utilities.return_first_arg(1, 2, 3) == 1
    assert iteration_utilities.return_called(int) == 0

    assert iteration_utilities.square(2) == 4
    assert iteration_utilities.reciprocal(2) == 0.5

    assert iteration_utilities.is_None(None)
    assert not iteration_utilities.is_None(False)

    assert not iteration_utilities.is_not_None(None)
    assert iteration_utilities.is_not_None(False)

    assert iteration_utilities.is_even(2)
    assert not iteration_utilities.is_even(1)

    assert iteration_utilities.is_odd(1)
    assert not iteration_utilities.is_odd(2)

    assert not iteration_utilities.is_iterable(1)
    assert iteration_utilities.is_iterable([1])


@pytest.mark.skipif(not iteration_utilities.GE_PY35,
                    reason="requires python3.5")
def test_c_funcs_signatures():
    # Makes sure every user-facing C function has a valid signature.
    from iteration_utilities import Iterable, chained
    from itertools import chain
    from operator import itemgetter
    from inspect import Signature

    # Gett all C functions
    it = Iterable(chain(iteration_utilities._cfuncs.__dict__.items())
                  # only include those that do not start with an underscore,
                  # we only need user-facing functions/classes
                  ).filterfalse(lambda x: x[0].startswith(('_'))
                  # only include those that have a __module__, to exclude things
                  # like "return_None", "first" which do not have a signature
                  ).filter(lambda x: hasattr(x[1], '__module__')
                  # only include those that are really part of the package
                  ).filter(lambda x: x[1].__module__.startswith('iteration_utilities')
                  # remove duplicates
                  ).unique_everseen(itemgetter(0)
                  # get the signature, fails if it can't
                  ).map(lambda x: (x[0], x[1], Signature.from_callable(x[1])))
    # Just need to trigger evaluation, use sorted because it's nice for manual
    # debugging!
    it.get_sorted(key=chained(itemgetter(0), str.lower))
