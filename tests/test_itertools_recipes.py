# Built-ins
from __future__ import absolute_import, division, print_function
from itertools import tee

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_doctest import doctest_module_no_failure


def test_doctests():
    doctest_module_no_failure(iteration_utilities._recipes._core)
    doctest_module_no_failure(iteration_utilities._recipes._additional)


def test_exceptions():

    # old-style classes don't have the subclasses special member.
    if iteration_utilities.PY2:
        class A:
            pass
        with pytest.raises(TypeError):
            list(iteration_utilities.itersubclasses(A))

    # Random product doesn't work with empty iterables
    with pytest.raises(IndexError):
        iteration_utilities.random_product([])

    # There is no element 10 in the tee object so this will raise the
    # Exception.
    t1, t2 = tee([1, 2, 3, 4, 5])
    with pytest.raises(IndexError):
        iteration_utilities.tee_lookahead(t1, 10)


def test_empty_input():
    empty = []
    assert iteration_utilities.all_equal(empty)
    assert iteration_utilities.consume(empty, 2) is None
    assert list(iteration_utilities.deepflatten(empty)) == []
    assert iteration_utilities.dotproduct(empty, empty) == 0
    assert list(iteration_utilities.flatten(empty)) == []
    assert list(iteration_utilities.grouper(empty, 2)) == []
    x, y = iteration_utilities.ipartition(empty, lambda x: x)
    assert list(x) == [] and list(y) == []
    # no need to test iter_except here
    # no need to test iter_subclasses here
    assert not iteration_utilities.last_true(empty)
    assert list(iteration_utilities.merge(empty)) == []
    assert list(iteration_utilities.ncycles(empty, 10)) == []
    assert iteration_utilities.nth(empty, 10) is None
    assert list(iteration_utilities.take(
        iteration_utilities.padnone(empty), 2)) == [None, None]
    assert list(iteration_utilities.powerset(empty)) == [()]
    assert iteration_utilities.quantify(empty, lambda x: x) == 0
    assert iteration_utilities.random_combination(empty, 0) == ()
    assert iteration_utilities.random_combination(empty, 0, True) == ()
    assert iteration_utilities.random_permutation(empty, 0) == ()
    # no need to test repeatfunc here
    # no need to test tabulate here
    assert list(iteration_utilities.tail(empty, 2)) == []
    assert iteration_utilities.take(empty, 2) == []
    # no need to test tee_lookahead here
    assert list(iteration_utilities.unique_justseen(empty)) == []
