# Licensed under Apache License Version 2.0 - see LICENSE

import operator

import pytest

from iteration_utilities import Iterable
from iteration_utilities._utils import _default


def test_sentinelfactory():
    as_str = str(_default)
    as_repr = repr(_default)
    assert as_str == as_repr
    assert as_str == "<default>"


def test_cls_length_hint():
    assert operator.length_hint(Iterable([1, 2, 3])) == 3
    assert operator.length_hint(Iterable([1, 2, 3]).accumulate()) == 3


def test_islice_no_arguments():
    # This previously raised an IndexError, this makes sure the correct
    # exception is raised (#250).
    with pytest.raises(TypeError):
        Iterable([1]).islice()


def test_cls_exception():
    with pytest.raises(TypeError):
        Iterable.from_count().pad(ntail=None)

    # __getitem__ : negative idx
    with pytest.raises(ValueError):
        Iterable(range(10))[-2]

    # __getitem__ : negative step
    with pytest.raises(ValueError):
        Iterable(range(10))[::-2]

    # __getitem__ : positive start with negative stop
    with pytest.raises(ValueError):
        Iterable(range(10))[2:-1]

    # __getitem__ : negative start/stop with infinite iterable.
    with pytest.raises(TypeError):
        Iterable.from_count()[-5:-3]

    # __getitem__ : not int, not slice
    with pytest.raises(TypeError):
        Iterable.from_count()['bad']
