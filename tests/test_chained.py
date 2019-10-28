# Licensed under Apache License Version 2.0 - see LICENSE

import pickle

import pytest

import iteration_utilities
from iteration_utilities import chained

import helper_funcs
from helper_cls import T as Original_T


class T(Original_T):
    def __add__(self, other):
        return self.__class__(self.value + other)

    def __mul__(self, other):
        return self.__class__(self.value * other)

    def __rtruediv__(self, other):
        return self.__class__(other / self.value)

    def __pow__(self, other):
        return self.__class__(self.value ** other)


class ChainedSubclass(chained):
    pass


def test_chained_repr1():
    x = chained(int, float)
    r = repr(x)
    assert 'chained' in r
    assert 'int' in r
    assert 'float' in r
    assert 'all=False' in r


def test_chained_repr2():
    x = chained(int, float, complex, str, reverse=True, all=True)
    r = repr(x)
    assert 'chained' in r
    assert 'int' in r
    assert 'float' in r
    assert 'complex' in r
    assert 'str' in r
    assert 'all=True' in r


def test_chained_from_chained1():
    # Verify that unwrapping works
    x = chained(int, chained(float, complex), str)
    funcs = x.__reduce__()[1]
    assert funcs[0] is int
    assert funcs[1] is float
    assert funcs[2] is complex
    assert funcs[3] is str


def test_chained_from_chained2():
    # Verify that unwrapping works (chained from chained)
    x = chained(chained(float, complex))
    funcs = x.__reduce__()[1]
    assert funcs[0] is float
    assert funcs[1] is complex


def test_chained_from_chained3():
    # Verify that unwrapping works
    x = chained(int, chained(float, complex), str, reverse=True)
    funcs = x.__reduce__()[1]
    assert funcs[0] is str
    assert funcs[1] is float  # still second!
    assert funcs[2] is complex  # still third!
    assert funcs[3] is int


def test_chained_from_chained4():
    # Verify that unwrapping works with multiple chained
    x = chained(chained(int, list),
                chained(float, complex),
                chained(str, tuple, set))
    funcs = x.__reduce__()[1]
    assert funcs[0] is int
    assert funcs[1] is list
    assert funcs[2] is float
    assert funcs[3] is complex
    assert funcs[4] is str
    assert funcs[5] is tuple
    assert funcs[6] is set


def test_chained_from_chained5():
    # Verify that unwrapping does NOT work if the inner chained has all
    x = chained(int, chained(float, complex, all=True), str)
    funcs = x.__reduce__()[1]
    assert funcs[0] is int
    assert type(funcs[1]) is chained  # no unwrapping
    assert funcs[2] is str


def test_chained_from_chained6():
    # Verify that unwrapping does NOT work if the outer chained has all
    x = chained(int, chained(float, complex), str, all=True)
    funcs = x.__reduce__()[1]
    assert funcs[0] is int
    assert type(funcs[1]) is chained  # no unwrapping
    assert funcs[2] is str


def test_chained_normal1():
    double_increment = chained(lambda x: x*2, lambda x: x+1)
    assert double_increment(T(10)) == T(21)
    assert double_increment(T(2)) == T(5)


def test_chained_reverse1():
    double_increment = chained(lambda x: x*2, lambda x: x+1, reverse=True)
    assert double_increment(T(10)) == T(22)
    assert double_increment(T(2)) == T(6)


def test_chained_all1():
    double_increment = chained(lambda x: x*2, lambda x: x+1, all=True)
    assert double_increment(T(10)) == (T(20), T(11))
    assert double_increment(T(2)) == (T(4), T(3))


def test_chained_attributes1():
    chd = chained(bool, int)
    assert chd.funcs == (bool, int)
    assert not chd.all


def test_chained_failure1():
    with pytest.raises(TypeError):  # at least one func must be present
        chained()


def test_chained_failure2():
    with pytest.raises(TypeError):  # kwarg not accepted
        chained(lambda x: x+1, invalidkwarg=lambda x: x*2)


def test_chained_failure3():
    with pytest.raises(TypeError):  # func fails
        chained(lambda x: x+1)(T('a'))


def test_chained_failure4():
    with pytest.raises(TypeError):  # second func fails
        chained(lambda x: x*2, lambda x: x+1)(T('a'))


def test_chained_failure5():
    with pytest.raises(TypeError):  # second func fails
        chained(lambda x: x*2, lambda x: x+1, all=True)(T('a'))


def test_chained_failure_setstate1():
    helper_funcs.iterator_setstate_list_fail(chained(lambda x: x))


def test_chained_failure_setstate2():
    helper_funcs.iterator_setstate_empty_fail(chained(lambda x: x))


def test_chained_pickle1(protocol):
    cmp = chained(iteration_utilities.square, iteration_utilities.reciprocal)
    x = pickle.dumps(cmp, protocol=protocol)
    assert pickle.loads(x)(T(10)) == T(1/100)
    assert pickle.loads(x)(T(2)) == T(1/4)


def test_chained_pickle2(protocol):
    cmp = chained(iteration_utilities.square, iteration_utilities.double,
                  reverse=True)
    x = pickle.dumps(cmp, protocol=protocol)
    assert pickle.loads(x)(T(10)) == T(400)
    assert pickle.loads(x)(T(3)) == T(36)


def test_chained_pickle3(protocol):
    cmp = chained(iteration_utilities.square, iteration_utilities.double,
                  all=True)
    x = pickle.dumps(cmp, protocol=protocol)
    assert pickle.loads(x)(T(10)) == (T(100), T(20))
    assert pickle.loads(x)(T(3)) == (T(9), T(6))
