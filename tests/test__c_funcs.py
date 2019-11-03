# Licensed under Apache License Version 2.0 - see LICENSE

import gc

import pytest

import iteration_utilities
from iteration_utilities import _iteration_utilities

import helper_funcs as _hf


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

    class NoBoolWithMod(_hf.FailBool):
        def __mod__(self, other):
            return self

    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        iteration_utilities.is_even(NoBoolWithMod())
    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        iteration_utilities.is_odd(NoBoolWithMod())
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        iteration_utilities.is_iterable(_hf.FailIter())


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


def test_traverse():
    """To test the traverse implementation we call gc.collect() while instances
    of all the C objects are still valid."""
    acc = iteration_utilities.accumulate([])
    app = iteration_utilities.applyfunc(lambda x: x, 1)
    cha = iteration_utilities.chained(int, float)
    cla = iteration_utilities.clamp([], 0, 1)
    com = iteration_utilities.complement(int)
    con = iteration_utilities.constant(1)
    dee = iteration_utilities.deepflatten([])
    dup = iteration_utilities.duplicates([])
    fli = iteration_utilities.flip(int)
    gro = iteration_utilities.grouper([], 2)
    ine = iteration_utilities.intersperse([], 1)
    iik = iteration_utilities.ItemIdxKey(10, 2)
    ite = iteration_utilities.iter_except(int, TypeError)
    mer = iteration_utilities.merge([])
    nth = iteration_utilities.nth(1)
    pac = iteration_utilities.packed(int)
    par = iteration_utilities.partial(int, 10)
    rep = iteration_utilities.replicate([], 3)
    rou = iteration_utilities.roundrobin([])
    see = iteration_utilities.Seen()
    sid = iteration_utilities.sideeffects([], lambda x: x)
    spl = iteration_utilities.split([], lambda x: True)
    sta = iteration_utilities.starfilter(lambda x: True, [])
    suc = iteration_utilities.successive([])
    tab = iteration_utilities.tabulate(int)
    une = iteration_utilities.unique_everseen([])
    unj = iteration_utilities.unique_justseen([])
    gc.collect()


@_hf.skip_on_pypy_not_investigated_why
def test_c_funcs_signatures():
    # Makes sure every user-facing C function has a valid signature.
    from iteration_utilities import Iterable, chained
    from itertools import chain
    from operator import itemgetter
    from inspect import Signature

    # Get all C functions
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
