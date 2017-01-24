# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


sideeffects = iteration_utilities.sideeffects
return_None = iteration_utilities.return_None


def raise_error_when_below10(val):
    if isinstance(val, tuple):
        if val[0].value < 10:
            raise ValueError()
    else:
        if val.value < 10:
            raise ValueError()


@memory_leak_decorator()
def test_sideeffects_empty1():
    assert list(sideeffects([], return_None)) == []


@memory_leak_decorator()
def test_sideeffects_empty2():
    assert list(sideeffects([], return_None, 0)) == []


@memory_leak_decorator()
def test_sideeffects_empty3():
    assert list(sideeffects([], return_None, 1)) == []


@memory_leak_decorator()
def test_sideeffects_empty4():
    assert list(sideeffects([], return_None, 10)) == []


@memory_leak_decorator()
def test_sideeffects_normal1():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append)) == [T(1), T(2)]
    assert l == [T(1), T(2)]


@memory_leak_decorator()
def test_sideeffects_normal2():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 0)) == [T(1), T(2)]
    assert l == [T(1), T(2)]


@memory_leak_decorator()
def test_sideeffects_normal3():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 1)) == [T(1), T(2)]
    assert l == [(T(1), ), (T(2), )]


@memory_leak_decorator()
def test_sideeffects_normal4():
    l = []
    assert list(sideeffects([T(1), T(2)], l.append, 2)) == [T(1), T(2)]
    assert l == [(T(1), T(2))]


@memory_leak_decorator()
def test_sideeffects_normal5():
    l = []
    assert list(sideeffects([T(1), T(2), T(3)], l.append,
                            times=2)) == [T(1), T(2), T(3)]
    assert l == [(T(1), T(2)), (T(3), )]


@memory_leak_decorator(collect=True)
def test_sideeffects_failure1():
    l = []
    with pytest.raises(TypeError):
        sideeffects(T(1), l.append)


@memory_leak_decorator(collect=True)
def test_sideeffects_failure2():
    l = []
    with pytest.raises(TypeError):
        sideeffects(T(1), l.append, 1)


@memory_leak_decorator(collect=True)
def test_sideeffects_failure3():
    with pytest.raises(ValueError):
        list(sideeffects([T(1), T(2), T(3)], raise_error_when_below10))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure4():
    with pytest.raises(ValueError):
        list(sideeffects([T(11), T(12), T(3)], raise_error_when_below10))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure5():
    with pytest.raises(ValueError):
        list(sideeffects([T(11), T(12), T(3)], raise_error_when_below10, 2))


@memory_leak_decorator(collect=True)
def test_sideeffects_failure6():
    with pytest.raises(ValueError):
        list(sideeffects([T(3), T(12), T(11)], raise_error_when_below10, 2))


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle1():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    assert next(suc) == T(1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle2():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(1), T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle3():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(1), T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle4():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 1)
    assert next(suc) == T(1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle5():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    assert next(suc) == T(1)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(2), T(3), T(4)]


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_sideeffects_pickle6():
    suc = sideeffects([T(1), T(2), T(3), T(4)], return_None, 2)
    assert next(suc) == T(1)
    assert next(suc) == T(2)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [T(3), T(4)]


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_sideeffects_lengthhint1():
    it = sideeffects([1, 2, 3, 4, 5, 6], lambda x: None)
    assert operator.length_hint(it) == 6
    next(it)
    assert operator.length_hint(it) == 5
    next(it)
    assert operator.length_hint(it) == 4
    next(it)
    assert operator.length_hint(it) == 3
    next(it)
    assert operator.length_hint(it) == 2
    next(it)
    assert operator.length_hint(it) == 1
    next(it)
    assert operator.length_hint(it) == 0
