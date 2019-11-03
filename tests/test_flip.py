# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

from iteration_utilities import flip

from helper_cls import T


class FlipSubclass(flip):
    pass


def _one_arg(a):
    return a,


def _two_args(a, b):
    return a, b


def _three_args(a, b, c):
    return a, b, c


def _six_args(a, b, c, d, e, f):
    return a, b, c, d, e, f


def test_flip_repr1():
    x = flip(int)
    r = repr(x)
    assert 'flip' in r
    assert 'int' in r


def test_flip_attributes1():
    x = flip(int)
    assert x.func is int


def test_flip_double_flip1():
    x = flip(int)
    y = flip(x)
    # Simply returned the original function instead of flipping the
    # arguments twice.
    assert y is int


def test_flip_double_flip2():
    # A subclass should prevent the behavior that it simply returns the
    # original function when flipped.
    assert FlipSubclass(flip(int)) is not int
    assert flip(FlipSubclass(int)) is not int


def test_flip_normal1():
    assert not flip(isinstance)(float, 10)


def test_flip_normal2():
    assert flip(isinstance)(int, 10)


def test_flip_args0():
    def func():
        return ()
    assert flip(func)() == ()


def test_flip_args1():
    assert flip(_one_arg)(T(10)) == (T(10),)


def test_flip_args1_only_kwargs():
    assert flip(_one_arg)(a=T(10)) == (T(10),)


def test_flip_args2():
    assert flip(_two_args)(T(1), T(2)) == (T(2), T(1))


def test_flip_args2_with_kwargs():
    assert flip(_two_args)(T(1), b=T(2)) == (T(1), T(2))


def test_flip_args2_only_kwargs():
    assert flip(_two_args)(a=T(1), b=T(2)) == (T(1), T(2))


def test_flip_args3():
    assert flip(_three_args)(T(1), T(2), T(3)) == (T(3), T(2), T(1))


def test_flip_args3_with_kwargs():
    assert flip(_three_args)(T(1), T(2), c=T(3)) == (T(2), T(1), T(3))


def test_flip_args3_with_2_kwargs():
    assert flip(_three_args)(T(1), b=T(2), c=T(3)) == (T(1), T(2), T(3))


def test_flip_args3_only_kwargs():
    assert flip(_three_args)(a=T(1), b=T(2), c=T(3)) == (T(1), T(2), T(3))


def test_flip_args6():
    expected = T(6), T(5), T(4), T(3), T(2), T(1)
    assert flip(_six_args)(T(1), T(2), T(3), T(4), T(5), T(6)) == expected


def test_flip_args6_with_kwargs():
    expected = T(3), T(2), T(1), T(4), T(5), T(6)
    assert flip(_six_args)(T(1), T(2), T(3), d=T(4), e=T(5), f=T(6)) == expected


def test_flip_args6_with_5_kwargs():
    expected = T(1), T(2), T(3), T(4), T(5), T(6)
    assert flip(_six_args)(T(1), b=T(2), c=T(3), d=T(4), e=T(5), f=T(6)) == expected


def test_flip_args6_only_kwargs():
    expected = T(1), T(2), T(3), T(4), T(5), T(6)
    assert flip(_six_args)(a=T(1), b=T(2), c=T(3), d=T(4), e=T(5), f=T(6)) == expected


def test_flip_failure1():
    with pytest.raises(TypeError):
        flip(isinstance)(10, float)


def test_flip_failure2():
    # Too few arguments
    with pytest.raises(TypeError):
        flip()


def test_flip_failure3():
    # Too many arguments
    with pytest.raises(TypeError):
        flip(isinstance, bool)


def test_flip_pickle1(protocol):
    x = pickle.dumps(flip(isinstance), protocol=protocol)
    assert pickle.loads(x)(float, 10.)
    assert pickle.loads(x)(int, 10)
    assert not pickle.loads(x)(float, 10)
