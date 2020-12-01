# Licensed under Apache License Version 2.0 - see LICENSE

from itertools import tee

import pytest

import iteration_utilities


def test_ipartition_with_none_predicate():
    f, t = iteration_utilities.ipartition([0, 1, 0, 1], pred=None)
    assert list(f) == [0, 0]
    assert list(t) == [1, 1]


def test_exceptions():
    # Random product doesn't work with empty iterables
    with pytest.raises(IndexError):
        iteration_utilities.random_product([])

    # There is no element 10 in the tee object so this will raise the
    # Exception.
    t1, t2 = tee([1, 2, 3, 4, 5])
    with pytest.raises(IndexError):
        iteration_utilities.tee_lookahead(t1, 10)

    # Missing idx or start/stop in replace/remove/getitem
    with pytest.raises(TypeError):
        iteration_utilities.replace([1, 2, 3], 5)
    with pytest.raises(TypeError):
        iteration_utilities.remove([1, 2, 3])
    with pytest.raises(TypeError):
        iteration_utilities.getitem([1, 2, 3])
    # Stop smaller than start in replace/remove
    with pytest.raises(ValueError):
        iteration_utilities.replace(range(10), 5, start=7, stop=5)
    with pytest.raises(ValueError):
        iteration_utilities.remove(range(10), start=7, stop=5)
    # idx smaller than -1 in getitem
    with pytest.raises(ValueError):
        iteration_utilities.getitem(range(10), (4, 2, -3, 9))

    with pytest.raises(ValueError):
        iteration_utilities.nth_combination([1], r=-1, index=0)
    with pytest.raises(ValueError):
        iteration_utilities.nth_combination([1], r=2, index=0)
    with pytest.raises(IndexError):
        iteration_utilities.nth_combination([1, 2, 3, 4], r=2, index=-20)
    with pytest.raises(IndexError):
        iteration_utilities.nth_combination([1, 2, 3, 4], r=2, index=20)


def test_empty_input():
    empty = []

    assert list(iteration_utilities
                .combinations_from_relations({}, 1)) == []

    assert list(iteration_utilities
                .combinations_from_relations({'a': [1, 2, 3]}, 2)) == []

    assert iteration_utilities.consume(empty, 2) is None

    assert list(iteration_utilities.flatten(empty)) == []

    assert list(iteration_utilities.getitem(
        range(10), empty)) == []

    x, y = iteration_utilities.ipartition(empty, lambda x: x)
    assert list(x) == [] and list(y) == []

    # no need to test iter_subclasses here

    assert list(iteration_utilities.ncycles(empty, 10)) == []

    assert list(iteration_utilities.powerset(empty)) == [()]

    assert iteration_utilities.random_combination(empty, 0) == ()
    assert iteration_utilities.random_combination(empty, 0, True) == ()

    assert iteration_utilities.random_permutation(empty, 0) == ()

    assert list(iteration_utilities.remove(
        range(10), empty)) == list(range(10))

    assert list(iteration_utilities.replace(
        range(10), 20, empty)) == list(range(10))

    # no need to test repeatfunc here

    # no need to test tabulate here

    assert list(iteration_utilities.tail(empty, 2)) == []

    # no need to test tee_lookahead here
