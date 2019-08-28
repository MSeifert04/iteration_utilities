# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
from helper_cls import T
from helper_leak import memory_leak_decorator


starfilter = iteration_utilities.starfilter


@memory_leak_decorator()
def test_starfilter_empty1():
    assert list(starfilter(operator.eq, [])) == []


@memory_leak_decorator()
def test_starfilter_normal1():
    inp = [(T(1), T(1)), (T(2), T(2))]
    assert list(starfilter(operator.eq, inp)) == [(T(1), T(1)), (T(2), T(2))]


@memory_leak_decorator()
def test_starfilter_normal2():
    # same test as above but with lists inside.
    inp = [[T(1), T(1)], [T(2), T(2)]]
    assert list(starfilter(operator.eq, inp)) == [[T(1), T(1)], [T(2), T(2)]]


@memory_leak_decorator()
def test_starfilter_normal3():
    inp = [(T(1), T(2)), (T(2), T(1))]
    assert list(starfilter(operator.eq, inp)) == []


@memory_leak_decorator()
def test_starfilter_normal4():
    # same test as above but with lists inside.
    inp = [[T(1), T(2)], [T(2), T(1)]]
    assert list(starfilter(operator.eq, inp)) == []


@memory_leak_decorator()
def test_starfilter_attributes1():
    it = starfilter(operator.eq, [])
    assert it.pred is operator.eq


@memory_leak_decorator(collect=True)
def test_starfilter_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        starfilter(operator.eq, _hf.FailIter())


@memory_leak_decorator(collect=True)
def test_starfilter_failure2():
    # item not convertable to tuple
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [T(1)]))


@memory_leak_decorator(collect=True)
def test_starfilter_failure3():
    # not enough arguments for function call
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [(T(1), )]))


@memory_leak_decorator(collect=True)
def test_starfilter_failure4():
    # too many arguments for function call
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [(T(1), T(1), T(1))]))


@memory_leak_decorator(collect=True)
def test_starfilter_failure5():
    # too many arguments for function call
    with pytest.raises(TypeError):
        next(starfilter(operator.eq, [(T(1), T(1), T(1))]))


@memory_leak_decorator(collect=True)
def test_starfilter_failure6():
    # function itself fails
    def failingfunc(a, b):
        raise ValueError('bad func')
    with pytest.raises(ValueError, match='bad func'):
        next(starfilter(failingfunc, [(T(1), T(1))]))


@memory_leak_decorator(collect=True)
def test_starfilter_failure7():
    # result of function has no boolean interpretation
    class NoBool(object):
        def __bool__(self):
            raise ValueError('no bool')

        __nonzero__ = __bool__

    def failingfunc(a, b):
        return NoBool()

    with pytest.raises(ValueError, match='no bool'):
        next(starfilter(failingfunc, [(T(1), T(1))]))


@memory_leak_decorator(collect=True)
def test_starfilter_failure8():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(starfilter(operator.ne, _hf.FailNext()))


@memory_leak_decorator(collect=True)
def test_starfilter_failure9():
    # Too few arguments
    with pytest.raises(TypeError):
        starfilter()


@memory_leak_decorator(collect=True, offset=1)
def test_starfilter_failure10():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        # won't work with return_True because then "iternext" is refreshed
        # before the failure comes.
        list(starfilter(iteration_utilities.return_False, _hf.CacheNext([1])))


@memory_leak_decorator(collect=True)
def test_starfilter_copy1():
    _hf.iterator_copy(starfilter(operator.eq, [(T(1), T(1)), (T(2), T(2))]))


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_starfilter_pickle1():
    sf = starfilter(operator.eq, [(T(1), T(1)), (T(2), T(2))])
    x = pickle.dumps(sf)
    assert list(pickle.loads(x)) == [(T(1), T(1)), (T(2), T(2))]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_starfilter_pickle2():
    sf = starfilter(operator.eq, [(T(1), T(1)), (T(2), T(2))])
    assert next(sf) == (T(1), T(1))
    x = pickle.dumps(sf)
    assert list(pickle.loads(x)) == [(T(2), T(2))]
