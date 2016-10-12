# Built-ins
from __future__ import absolute_import, division, print_function
import operator

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


class T(object):
    def __init__(self, value):
        self.value = value

    def __gt__(self, other):
        return self.value > other.value

    def __lt__(self, other):
        return self.value < other.value


minmax = iteration_utilities.minmax


def test_minmax_normal1():
    assert minmax([1]) == (1, 1)

    def test():
        minmax([T(1)])
    assert not memory_leak(test)


def test_minmax_normal2():
    assert minmax([1, 2]) == (1, 2)

    def test():
        minmax([T(1), T(2)])
    assert not memory_leak(test)


def test_minmax_normal3():
    assert minmax([2, 1]) == (1, 2)

    def test():
        minmax([T(2), T(1)])
    assert not memory_leak(test)


def test_minmax_normal4():
    assert minmax([1, 2, 3]) == (1, 3)

    def test():
        minmax([T(1), T(2), T(3)])
    assert not memory_leak(test)


def test_minmax_normal5():
    assert minmax([1, 3, 2]) == (1, 3)

    def test():
        minmax([T(1), T(3), T(2)])
    assert not memory_leak(test)


def test_minmax_normal6():
    assert minmax(range(100)) == (0, 99)

    def test():
        minmax(map(T, range(100)))
    assert not memory_leak(test)


def test_minmax_normal7():
    assert minmax(range(101)) == (0, 100)

    def test():
        minmax(map(T, range(101)))
    assert not memory_leak(test)


def test_minmax_normal8():
    assert minmax({1, 2, -3}) == (-3, 2)

    def test():
        minmax({T(1), T(2), T(-3)})
    assert not memory_leak(test)


def test_minmax_normal9():
    assert minmax({1: 0, 2: 0, 3: 0}) == (1, 3)

    def test():
        minmax({T(1): T(0), T(2): T(0), T(3): T(0)})
    assert not memory_leak(test)


def test_minmax_normal10():
    assert minmax(1, 2, 3) == (1, 3)

    def test():
        minmax(T(1), T(2), T(3))
    assert not memory_leak(test)


def test_minmax_normal11():
    assert minmax(4, 3, 2, 1) == (1, 4)

    def test():
        minmax(T(4), T(3), T(2), T(1))
    assert not memory_leak(test)


def test_minmax_normal12():
    assert minmax((i for i in [4, 3, 2, 5, 3])) == (2, 5)

    def test():
        minmax((T(i) for i in [4, 3, 2, 5, 3]))
    assert not memory_leak(test)


def test_minmax_normal13():
    assert minmax((i for i in [4, 3, 2, 5, 3, 3])) == (2, 5)

    def test():
        minmax((T(i) for i in [4, 3, 2, 5, 3, 3]))
    assert not memory_leak(test)


def test_minmax_key1():
    assert minmax('a', 'b', 'c',
                  key=operator.methodcaller('upper')) == ('a', 'c')

    def test():
        minmax(T('a'), T('b'), T('c'), key=lambda x: x.value.upper())
    assert not memory_leak(test)


def test_minmax_key2():
    assert minmax((1, 2), (2, 3), (3, 1),
                  key=operator.itemgetter(1)) == ((3, 1), (2, 3))

    def test():
        minmax(T((T(1), T(2))), T((T(2), T(3))),
               T((T(3), T(1))), key=lambda x: x.value[1])
    assert not memory_leak(test)


def test_minmax_default1():
    assert minmax([], default=10) == (10, 10)

    def test():
        minmax([], default=T(10))
    assert not memory_leak(test)


def test_minmax_stability1():
    assert minmax([(1, 5)],
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))

    def test():
        minmax([T((T(1), T(5)))], key=lambda x: x.value[0])
    assert not memory_leak(test)


def test_minmax_stability2():
    assert minmax((1, 5), (1, 1),
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))

    def test():
        minmax(T((T(1), T(5))), T((T(1), T(1))),
               key=lambda x: x.value[0])
    assert not memory_leak(test)


def test_minmax_stability3():
    assert minmax((1, 5), (1, 1), (1, 2),
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))

    def test():
        minmax(T((T(1), T(5))), T((T(1), T(1))),
               T((T(1), T(2))), key=lambda x: x.value[0])
    assert not memory_leak(test)


def test_minmax_stability4():
    assert minmax((1, 5), (1, 1), (1, 2), (1, 3),
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))

    def test():
        minmax(T((T(1), T(5))), T((T(1), T(1))),
               T((T(1), T(2))), T((T(1), T(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test)


def test_minmax_stability5():
    assert minmax((5, 5), (1, 5), (1, 2), (1, 3),
                  key=operator.itemgetter(0)) == ((1, 5), (5, 5))

    def test():
        minmax(T((T(5), T(5))), T((T(1), T(5))),
               T((T(1), T(2))), T((T(1), T(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test)


def test_minmax_stability6():
    assert minmax((5, 5), (3, 5), (1, 5), (1, 3),
                  key=operator.itemgetter(0)) == ((1, 5), (5, 5))

    def test():
        minmax(T((T(5), T(5))), T((T(3), T(5))),
               T((T(1), T(5))), T((T(1), T(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test)


def test_minmax_stability7():
    assert minmax((5, 5), (3, 5), (4, 5), (1, 5),
                  key=operator.itemgetter(0)) == ((1, 5), (5, 5))

    def test():
        minmax(T((T(5), T(5))), T((T(3), T(5))),
               T((T(4), T(5))), T((T(1), T(5))),
               key=lambda x: x.value[0])
    assert not memory_leak(test)


def test_minmax_failure1():
    # No args
    with pytest.raises(TypeError):
        minmax()

    def test():
        with pytest_raises(TypeError):
            minmax()
    assert not memory_leak(test)


def test_minmax_failure2():
    # empty sequence no default
    with pytest.raises(ValueError):
        minmax([])

    def test():
        with pytest_raises(ValueError):
            minmax([])
    assert not memory_leak(test)


def test_minmax_failure3():
    # invalid kwarg
    with pytest.raises(TypeError):
        minmax(1, 2, invalid_kw='a')

    def test():
        with pytest_raises(TypeError):
            minmax(T(1), T(2), invalid_kw='a')
    assert not memory_leak(test)


def test_minmax_failure4():
    # default with multiple args
    with pytest.raises(TypeError):
        minmax(1, 2, default=10)

    def test():
        with pytest_raises(TypeError):
            minmax(T(1), T(2), default=T(10))
    assert not memory_leak(test)


def test_minmax_failure5():
    # arg is not iterable
    with pytest.raises(TypeError):
        minmax(100)

    def test():
        with pytest_raises(TypeError):
            minmax(T(100))
    assert not memory_leak(test)


def test_minmax_failure6():
    # func fails on odd numbered arg
    with pytest.raises(TypeError):
        minmax(100, 'a', key=lambda x: x + '')

    def test():
        with pytest_raises(TypeError):
            minmax(T(100), T('a'), key=lambda x: x.value + '')
    assert not memory_leak(test)


def test_minmax_failure7():
    # func fails on even numbered arg
    with pytest.raises(TypeError):
        minmax('a', 100, key=lambda x: x + '')

    def test():
        with pytest_raises(TypeError):
            minmax(T('a'), T(100), key=lambda x: x.value + '')
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_minmax_failure8():
    # unable to compare first and second
    with pytest.raises(TypeError):
        minmax(100, 'a')

    def test():
        with pytest_raises(TypeError):
            minmax(T(100), T('a'))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_minmax_failure9():
    # unable to compare third and fourth
    with pytest.raises(TypeError):
        minmax(100, 20, 100, 'a')

    def test():
        with pytest_raises(TypeError):  # unable to compare 3 and 4
            minmax(T(100), T(20), T(100), T('a'))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_minmax_failure10():
    # unable to compare first and third
    with pytest.raises(TypeError):
        minmax(1, 20, 'a', 'c')

    def test():
        with pytest_raises(TypeError):  # unable to compare 1 and 3
            minmax(T(1), T(20), T('a'), T('c'))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_minmax_failure11():
    # unable to compare second and fourth

    # This is tricky. The elements are explicitly chosen so that
    # 1 compares with 2 without error: 1 current min, 2 current max
    # 3 compares with 4: 3 < 4
    # 3 compares with 1: 1 still current minimum
    # 4 compares not with 2 because the first element is equal and then
    # the comparison with the second element throws and error because
    # str and int are not compareable.
    with pytest.raises(TypeError):
        minmax((100, 'a'), (200, 10), (150, 'b'), (200, 'd'))

    def test():
        with pytest_raises(TypeError):
            minmax(T((100, 'a')), T((200, 10)),
                   T((150, 'b')), T((200, 'd')))
    assert not memory_leak(test)
