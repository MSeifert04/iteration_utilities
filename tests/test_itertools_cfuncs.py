# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='Python 2 does not support this way of pickling.')
def test_cfuncs_pickle():

    grouper = iteration_utilities.grouper
    intersperse = iteration_utilities.intersperse
    merge = iteration_utilities.merge
    unique_justseen = iteration_utilities.unique_justseen
    roundrobin = iteration_utilities.roundrobin
    complement = iteration_utilities.complement
    compose = iteration_utilities.compose

    # IMPORTANT: methoddescriptors like "str.lower" as key functions can not
    #            be pickled before python 3.4

    # ----- Roundrobin
    rr = roundrobin([1, 2, 3], [1, 2, 3])
    assert next(rr) == 1
    x = pickle.dumps(rr)
    assert list(pickle.loads(x)) == [1, 2, 2, 3, 3]

    rr2 = roundrobin([1], [1, 2, 3])
    assert next(rr2) == 1
    assert next(rr2) == 1
    assert next(rr2) == 2
    x = pickle.dumps(rr2)
    assert list(pickle.loads(x)) == [3]

    # ----- Merge
    mge = merge([0], [1, 2], [2])
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, 2]

    mge = merge([1, 2], [0], [-2], key=abs)
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, -2]

    mge = merge([2, 1], [0], [3], reverse=True)
    assert next(mge) == 3
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [2, 1, 0]

    # ----- Grouper
    grp = grouper(range(10), 3)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9,)]

    grp = grouper(range(10), 3, fillvalue=0)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9, 0, 0)]

    grp = grouper(range(10), 3, truncate=True)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8)]

    # ----- Intersperse
    its = intersperse([1, 2, 3], 0)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [1, 0, 2, 0, 3]

    its = intersperse([1, 2, 3], 0)  # start value must be set!
    assert next(its) == 1
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 2, 0, 3]

    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [2, 0, 3]

    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    assert next(its) == 2
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 3]

    # ----- Complement
    x = pickle.dumps(complement(iteration_utilities.is_None))
    assert pickle.loads(x)(False)
    assert pickle.loads(x)(True)
    assert not pickle.loads(x)(None)

    # ----- Unique_justseen
    ujs = unique_justseen([1, 2, 3])
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [1, 2, 3]

    ujs = unique_justseen([1, 2, 3])
    assert next(ujs) == 1
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [2, 3]

    # Pickling a method descriptor is not possible for Python 3.3 and before
    # Also operator.methodcaller loses it's methodname when pickled for Python
    #   3.4 and lower...
    if iteration_utilities.PY34:
        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        x = pickle.dumps(ujs)
        assert list(pickle.loads(x)) == ['a']

        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        assert next(ujs) == 'a'
        x = pickle.dumps(ujs)
        assert list(pickle.loads(x)) == []

    # ----- Compose

    cmp = compose(iteration_utilities.square, iteration_utilities.one_over)
    x = pickle.dumps(cmp)
    assert pickle.loads(x)(10) == 1/100
    assert pickle.loads(x)(2) == 1/4


def test_callbacks():
    assert iteration_utilities.return_True()
    assert not iteration_utilities.return_False()
    assert iteration_utilities.return_None() is None
    assert iteration_utilities.return_identity(1) == 1
    assert iteration_utilities.return_first_positional_argument(1, 2, 3) == 1
    assert iteration_utilities.return_called(int) == 0

    assert iteration_utilities.square(2) == 4
    assert iteration_utilities.one_over(2) == 0.5

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
