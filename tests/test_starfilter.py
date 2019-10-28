# Licensed under Apache License Version 2.0 - see LICENSE

import operator
import pickle

import pytest

import iteration_utilities
from iteration_utilities import starfilter

import helper_funcs as _hf
from helper_cls import T


def test_starfilter_empty1():
    assert list(starfilter(operator.eq, [])) == []


def test_starfilter_normal1():
    inp = [(T(1), T(1)), (T(2), T(2))]
    assert list(starfilter(operator.eq, inp)) == [(T(1), T(1)), (T(2), T(2))]


def test_starfilter_normal2():
    # same test as above but with lists inside.
    inp = [[T(1), T(1)], [T(2), T(2)]]
    assert list(starfilter(operator.eq, inp)) == [[T(1), T(1)], [T(2), T(2)]]


def test_starfilter_normal3():
    inp = [(T(1), T(2)), (T(2), T(1))]
    assert list(starfilter(operator.eq, inp)) == []


def test_starfilter_normal4():
    # same test as above but with lists inside.
    inp = [[T(1), T(2)], [T(2), T(1)]]
    assert list(starfilter(operator.eq, inp)) == []


def test_starfilter_attributes1():
    it = starfilter(operator.eq, [])
    assert it.pred is operator.eq


def test_starfilter_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        starfilter(operator.eq, _hf.FailIter())


def test_starfilter_failure2():
    # item not convertable to tuple
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [T(1)]))


def test_starfilter_failure3():
    # not enough arguments for function call
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [(T(1), )]))


def test_starfilter_failure4():
    # too many arguments for function call
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [(T(1), T(1), T(1))]))


def test_starfilter_failure5():
    # too many arguments for function call
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [(T(1), T(1), T(1))]))


def test_starfilter_failure6():
    # function itself fails
    def failingfunc(a, b):
        raise ValueError('bad func')
    with pytest.raises(ValueError, match='bad func'):
        next(starfilter(failingfunc, [(T(1), T(1))]))


def test_starfilter_failure7():
    # result of function has no boolean interpretation
    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        next(starfilter(lambda x, y: _hf.FailBool(), [(T(1), T(1))]))


def test_starfilter_failure8():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(starfilter(operator.ne, _hf.FailNext()))


def test_starfilter_failure9():
    # Too few arguments
    with pytest.raises(TypeError):
        starfilter()


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_starfilter_failure10():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        # won't work with return_True because then "iternext" is refreshed
        # before the failure comes.
        list(starfilter(iteration_utilities.return_False, _hf.CacheNext([1])))


def test_starfilter_copy1():
    _hf.iterator_copy(starfilter(operator.eq, [(T(1), T(1)), (T(2), T(2))]))


def test_starfilter_pickle1(protocol):
    sf = starfilter(operator.eq, [(T(1), T(1)), (T(2), T(2))])
    x = pickle.dumps(sf, protocol=protocol)
    assert list(pickle.loads(x)) == [(T(1), T(1)), (T(2), T(2))]


def test_starfilter_pickle2(protocol):
    sf = starfilter(operator.eq, [(T(1), T(1)), (T(2), T(2))])
    assert next(sf) == (T(1), T(1))
    x = pickle.dumps(sf, protocol=protocol)
    assert list(pickle.loads(x)) == [(T(2), T(2))]
