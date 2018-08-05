# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities
import _iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator


@memory_leak_decorator(dummy=True)
def test_nothing():
    """This is just here so that the leak-decorator runs once to avoid spurious
    false-positives on the first invocation of the decorator."""
    pass


@memory_leak_decorator()
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


# this is the first function that is executed that throws exceptions, no
# idea why it "thinks" there are reference leaks in the first run but they
# did disappear when I used "offset=1"...
@memory_leak_decorator(collect=True, offset=1)
def test_other_c_funcs_failures():
    with pytest.raises(TypeError):
        # no argument given.
        iteration_utilities.return_first_arg()
    with pytest.raises(TypeError):
        # no positional argument given.
        iteration_utilities.return_first_arg(test=10)

    x = object()
    with pytest.raises(TypeError):
        iteration_utilities.is_even(x)
    with pytest.raises(TypeError):
        iteration_utilities.is_odd(x)

    class NoBool(object):
        def __bool__(self):
            raise ValueError('bad class')

        def __mod__(self, other):
            return self
        __nonzero__ = __bool__

    with pytest.raises(ValueError) as exc:
        iteration_utilities.is_even(NoBool())
    assert 'bad class' in str(exc)
    with pytest.raises(ValueError) as exc:
        iteration_utilities.is_odd(NoBool())
    assert 'bad class' in str(exc)

    class NoIter(object):
        def __iter__(self):
            raise ValueError('bad class')
    with pytest.raises(ValueError) as exc:
        iteration_utilities.is_iterable(NoIter())
    assert 'bad class' in str(exc)


@memory_leak_decorator(collect=True)
def test_reverse_math_ops():
    assert iteration_utilities.radd(1, 2) == 3
    assert iteration_utilities.rsub(1, 2) == 1
    assert iteration_utilities.rmul(1, 2) == 2
    assert iteration_utilities.rdiv(1, 2) == 2
    assert iteration_utilities.rfdiv(1, 2) == 2
    assert iteration_utilities.rpow(1, 2) == 2
    assert iteration_utilities.rmod(1, 2) == 0

    for rfunc in [iteration_utilities.radd, iteration_utilities.rsub,
                  iteration_utilities.rmul, iteration_utilities.rdiv,
                  iteration_utilities.rfdiv, iteration_utilities.rpow,
                  iteration_utilities.rmod]:
        with pytest.raises(TypeError):
            # Too few arguments
            rfunc(1)
        with pytest.raises(TypeError):
            # Too many arguments
            rfunc(1, 2, 3)


@pytest.mark.skipif(not iteration_utilities.GE_PY35,
                    reason="requires python3.5")
def test_c_funcs_signatures():
    # Makes sure every user-facing C function has a valid signature.
    from iteration_utilities import Iterable, chained
    from itertools import chain
    from operator import itemgetter
    from inspect import Signature

    # Gett all C functions
    it = (Iterable(chain(_iteration_utilities.__dict__.items()))
          # only include those that do not start with an underscore,
          # we only need user-facing functions/classes
          .filterfalse(lambda x: x[0].startswith(('_')))
          # only include those that have a __module__, to exclude things
          # like "return_None", "first" which do not have a signature
          .filter(lambda x: hasattr(x[1], '__module__'))
          # only include those that are really part of the package
          .filter(lambda x: x[1].__module__.startswith('iteration_utilities'))
          # remove duplicates
          .unique_everseen(itemgetter(0))
          # get the signature, fails if it can't
          .map(lambda x: (x[0], x[1], Signature.from_callable(x[1]))))
    # Just need to trigger evaluation, use sorted because it's nice for manual
    # debugging!
    it.get_sorted(key=chained(itemgetter(0), str.lower))
