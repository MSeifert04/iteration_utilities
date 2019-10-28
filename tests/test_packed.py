# Licensed under Apache License Version 2.0 - see LICENSE

import collections
import operator
import pickle

import pytest

from iteration_utilities import packed

from helper_cls import T


def _t3(a, b, c):
    return a, b, c


def _t6(a, b, c, d, e, f):
    return a, b, c, d, e, f


def test_packed_repr1():
    x = packed(int)
    r = repr(x)
    assert 'packed' in r
    assert 'int' in r


def test_packed_attributes1():
    x = packed(int)
    assert x.func is int


@pytest.mark.parametrize("kind", [tuple, list, collections.deque])
def test_packed_normal(kind):
    eq = packed(operator.eq)
    assert eq(kind([T(1), T(1)]))
    assert not eq(kind([T(1), T(2)]))


@pytest.mark.parametrize("kind", [tuple, list, collections.deque])
def test_packed_normal_with_kwargs(kind):
    t3 = packed(_t3)
    inp = kind([T(1), T(2)])
    assert t3(inp, c=T(3)) == (T(1), T(2), T(3))


@pytest.mark.parametrize("kind", [tuple, list, collections.deque])
def test_packed_normal_more_than_6(kind):
    t6 = packed(_t6)
    inp = kind([T(1), T(2), T(3), T(4), T(5), T(6)])
    assert t6(inp) == (T(1), T(2), T(3), T(4), T(5), T(6))


@pytest.mark.parametrize("kind", [tuple, list, collections.deque])
def test_packed_normal_more_than_6_with_kwargs(kind):
    t6 = packed(_t6)
    inp = kind([T(1), T(2), T(3), T(4)])
    assert t6(inp, e=T(5), f=T(6)) == (T(1), T(2), T(3), T(4), T(5), T(6))


def test_packed_failure1():
    # too many arguments when creating an instance
    with pytest.raises(TypeError):
        packed(1, 2)


def test_packed_failure2():
    # too few arguments when creating an instance
    with pytest.raises(TypeError):
        packed()


def test_packed_failure3():
    # too few arguments when calling the instance
    with pytest.raises(TypeError):
        packed(operator.eq)()


def test_packed_failure4():
    # too many arguments when calling the instance
    with pytest.raises(TypeError):
        packed(operator.eq)(1, 2)


def test_packed_failure5():
    # arguments for calling the instance are not convertible to tuple
    with pytest.raises(TypeError):
        packed(operator.eq)(1)


def test_packed_failure6():
    # function raised an Exception
    def failingfunc(a, b):
        raise ValueError('bad func')

    with pytest.raises(ValueError, match='bad func'):
        packed(failingfunc)((1, 2))


def test_packed_pickle1(protocol):
    eq = packed(operator.eq)
    x = pickle.dumps(eq, protocol=protocol)
    assert pickle.loads(x)((T(1), T(1)))
    assert not pickle.loads(x)((T(1), T(2)))
