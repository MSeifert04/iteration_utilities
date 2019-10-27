# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

import iteration_utilities
from iteration_utilities import nth

import helper_funcs as _hf
from helper_cls import T, toT


def test_nth_repr1():
    x = nth(2)
    r = repr(x)
    assert 'nth' in r
    assert '2' in r


def test_nth_attributes1():
    assert iteration_utilities.first.n == 0
    assert iteration_utilities.second.n == 1
    assert iteration_utilities.third.n == 2
    assert iteration_utilities.last.n == -1
    assert nth(10).n == 10


def test_nth_normal1():
    assert nth(1)([T(1), T(2), T(3)]) == T(2)


def test_nth_normal2():
    assert nth(2)(map(T, range(10))) == T(2)


def test_nth_nopred_retpred1():
    assert nth(2)(toT(range(10)), retpred=True) == T(2)


def test_nth_retidx1():
    assert nth(2)(toT(range(10)), retidx=True) == 2


def test_nth_retidx2():
    assert nth(2)(toT(range(10)), pred=bool, retidx=True) == 3


def test_nth_pred1():
    # With pred
    assert nth(1)([T(0), T(1), T(2)], pred=bool) == T(2)


def test_nth_pred2():
    assert nth(1)([T(0), T(1), T(2)], pred=None) == T(2)


def test_nth_pred3():
    assert nth(0)([T(0)]*100 + [T(1)], pred=bool) == T(1)


def test_nth_pred4():
    assert nth(1)([[T(0)], [T(1), T(2)]]*2,
                  pred=lambda x: len(x) > 1) == [T(1), T(2)]


def test_nth_predtruthyretpred1():
    # pred with truthy/retpred
    assert nth(1)([T(0), T(2), T(3), T(0)], pred=bool, truthy=False) == T(0)


def test_nth_predtruthyretpred2():
    assert not nth(1)([T(0), T(1), T(2), T(3), T(0)], pred=bool, truthy=False,
                      retpred=True)


def test_nth_predtruthyretpred3():
    assert nth(1)([T(0), T(2), T(3), T(0)], pred=lambda x: x**T(2),
                  truthy=False) == T(0)


def test_nth_predtruthyretpred4():
    assert nth(1)(toT([0, 1, 2, 3, 0]),
                  pred=lambda x: x**T(2), truthy=False, retpred=True) == T(0)


def test_nth_predtruthyretpred5():
    assert nth(2)([T(0), T(1), T(2), T(3)], pred=bool) == T(3)


def test_nth_predtruthyretpred6():
    assert nth(2)([T(0), T(1), T(2), T(3)], pred=bool, retpred=True)


def test_nth_predtruthyretpred7():
    assert nth(2)([T(0), T(1), T(2), T(3)], pred=lambda x: x**T(2)) == T(3)


def test_nth_predtruthyretpred8():
    assert nth(2)([T(0), T(2), T(3), T(4)], pred=lambda x: x**T(2),
                  retpred=True) == T(16)


def test_nth_default1():
    # With default
    assert nth(2)([], default=None) is None


def test_nth_default2():
    assert nth(1)([T(0), T(0), T(0)], default=None, pred=bool) is None


def test_nth_default3():
    # generator
    assert nth(1)((i for i in [T(0), T(0), T(0)]),
                  default=None, pred=bool) is None


def test_nth_regressiontest():
    # This segfaulted in earlier versions because the "val" intermediate
    # variable was decref'd for each item in the iterable.
    lst = [1] + [0]*10000 + [2]*20
    assert nth(1)(lst, pred=bool, retpred=True)


def test_nth_failures1():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        nth(10)(_hf.FailIter())


def test_nth_failures2():
    with pytest.raises(IndexError):
        nth(10)([])


def test_nth_failures3():
    with pytest.raises(IndexError):
        nth(1)([T(0)], pred=bool)


def test_nth_failures4():
    with pytest.raises(TypeError):
        nth(1)([T('a'), T('b')], pred=lambda x: abs(x.value))


def test_nth_failures5():
    # item not an integer
    with pytest.raises(TypeError):
        nth('a')


def test_nth_failures6():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        nth(1)(_hf.FailNext())


def test_nth_failures7():
    # too few arguments for __call__
    with pytest.raises(TypeError):
        nth(1)()


def test_nth_failures8():
    # too few arguments for __call__
    with pytest.raises(TypeError):
        nth(1)()


def test_nth_failures9():
    # too few arguments for __call__
    with pytest.raises(ValueError, match='`retpred` or `retidx`'):
        nth(1)([T(0), T(1), T(2)], retpred=True, retidx=True)


def test_nth_failures10():
    # indexerror with generator
    with pytest.raises(IndexError):
        nth(1)((i for i in [T(0)]), pred=bool)


def test_nth_failures11():
    # evaluating as boolean fails
    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        nth(1)([T(0)], pred=lambda x: _hf.FailBool())


def test_nth_failures12():
    # evaluating as boolean fails
    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        nth(1)([T(0)], pred=lambda x: _hf.FailBool(), retpred=True)


def test_nth_failures13():
    # evaluating as boolean fails
    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        nth(1)([T(0)], pred=lambda x: _hf.FailBool(), retidx=True)


def test_nth_failures14():
    # evaluating as boolean fails
    with pytest.raises(_hf.FailBool.EXC_TYP, match=_hf.FailBool.EXC_MSG):
        nth(1)([T(0)], pred=lambda x: _hf.FailBool(), truthy=False)


@_hf.skip_on_pypy_because_cache_next_works_differently
def test_nth_failure15():
    # Changing next method
    with pytest.raises(_hf.CacheNext.EXC_TYP, match=_hf.CacheNext.EXC_MSG):
        nth(2)(_hf.CacheNext(1))


def test_nth_pickle1(protocol):
    x = pickle.dumps(nth(2), protocol=protocol)
    assert pickle.loads(x)([T(1), T(2), T(3), T(4)]) == T(3)
