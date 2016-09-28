# Built-ins
from __future__ import absolute_import, division, print_function
import operator

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_doctest import doctest_module_no_failure
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


def test_doctests():
    # classes are added to the main module code. :-)
    doctest_module_no_failure(iteration_utilities)
    doctest_module_no_failure(iteration_utilities._cfuncs)


def test_minmax():
    minmax = iteration_utilities.minmax
    # Test some inputs
    assert minmax([1]) == (1, 1)
    assert minmax([1, 2]) == (1, 2)
    assert minmax([2, 1]) == (1, 2)
    assert minmax([1, 2, 3]) == (1, 3)
    assert minmax([1, 3, 2]) == (1, 3)
    # Other input types
    assert minmax(range(100)) == (0, 99)
    assert minmax(range(101)) == (0, 100)
    assert minmax({1, 2, -3}) == (-3, 2)
    assert minmax({1: 0, 2: 0, 3: 0}) == (1, 3)
    # Test multiple args instead of one sequence
    assert minmax(1, 2, 3) == (1, 3)
    assert minmax(4, 3, 2, 1) == (1, 4)
    # Test key-function
    assert minmax('a', 'b', 'c',
                  key=operator.methodcaller('upper')) == ('a', 'c')
    assert minmax((1, 2), (2, 3), (3, 1),
                  key=operator.itemgetter(1)) == ((3, 1), (2, 3))
    # Test default value
    assert minmax([], default=10) == (10, 10)

    # Test stablility
    assert minmax([(1, 5)],
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))
    assert minmax((1, 5), (1, 1),
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))
    assert minmax((1, 5), (1, 1), (1, 2),
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))
    assert minmax((1, 5), (1, 1), (1, 2), (1, 3),
                  key=operator.itemgetter(0)) == ((1, 5), (1, 5))
    assert minmax((5, 5), (1, 5), (1, 2), (1, 3),
                  key=operator.itemgetter(0)) == ((1, 5), (5, 5))
    assert minmax((5, 5), (3, 5), (1, 5), (1, 3),
                  key=operator.itemgetter(0)) == ((1, 5), (5, 5))
    assert minmax((5, 5), (3, 5), (4, 5), (1, 5),
                  key=operator.itemgetter(0)) == ((1, 5), (5, 5))

    # Test exceptions
    with pytest.raises(TypeError):  # No args
        minmax()
    with pytest.raises(ValueError):  # empty sequence no default
        minmax([])
    with pytest.raises(TypeError):  # invalid kwarg
        minmax(1, 2, invalid_kw='a')
    with pytest.raises(TypeError):  # default with multiple args
        minmax(1, 2, default=10)
    with pytest.raises(TypeError):  # arg is not iterable
        minmax(100)
    with pytest.raises(TypeError):  # func fails on odd numbered arg
        minmax(100, 'a', key=lambda x: x + '')
    with pytest.raises(TypeError):  # func fails on even numbered arg
        minmax('a', 100, key=lambda x: x + '')
    if not iteration_utilities.PY2:
        with pytest.raises(TypeError):  # unable to compare first and second
            minmax(100, 'a')
        with pytest.raises(TypeError):  # unable to compare third and fourth
            minmax(100, 20, 100, 'a')
        with pytest.raises(TypeError):  # unable to compare first and third
            minmax(1, 20, 'a', 'c')
        with pytest.raises(TypeError):  # unable to compare second and fourth
            # This is tricky. The elements are explicitly chosen so that
            # 1 compares with 2 without error: 1 current min, 2 current max
            # 3 compares with 4: 3 < 4
            # 3 compares with 1: 1 still current minimum
            # 4 compares not with 2 because the first element is equal and then
            # the comparison with the second element throws and error because
            # str and int are not compareable.
            minmax((100, 'a'), (200, 10), (150, 'b'), (200, 'd'))


def test_minmax_memoryleak():
    minmax = iteration_utilities.minmax

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __gt__(self, other):
            return self.value > other.value

        def __lt__(self, other):
            return self.value < other.value

    # Test some inputs
    def test():
        minmax([Test(1)])
    assert not memory_leak(test, Test)

    def test():
        minmax([Test(1), Test(2)])
    assert not memory_leak(test, Test)

    def test():
        minmax([Test(2), Test(1)])
    assert not memory_leak(test, Test)

    def test():
        minmax([Test(1), Test(2), Test(3)])
    assert not memory_leak(test, Test)

    def test():
        minmax([Test(1), Test(3), Test(2)])
    assert not memory_leak(test, Test)

    # Other input types
    def test():
        minmax(map(Test, range(100)))
    assert not memory_leak(test, Test)

    def test():
        minmax(map(Test, range(101)))
    assert not memory_leak(test, Test)

    def test():
        minmax({Test(1), Test(2), Test(-3)})
    assert not memory_leak(test, Test)

    def test():
        minmax({Test(1): Test(0), Test(2): Test(0), Test(3): Test(0)})
    assert not memory_leak(test, Test)

    # Test multiple args instead of one sequence
    def test():
        minmax(Test(1), Test(2), Test(3))
    assert not memory_leak(test, Test)

    def test():
        minmax(Test(4), Test(3), Test(2), Test(1))
    assert not memory_leak(test, Test)

    # Test key-function
    def test():
        minmax(Test('a'), Test('b'), Test('c'), key=lambda x: x.value.upper())
    assert not memory_leak(test, Test)

    def test():
        minmax(Test((Test(1), Test(2))), Test((Test(2), Test(3))),
               Test((Test(3), Test(1))), key=lambda x: x.value[1])
    assert not memory_leak(test, Test)

    # Test default value
    def test():
        minmax([], default=Test(10))
    assert not memory_leak(test, Test)

    # Test stablility
    def test():
        minmax([Test((Test(1), Test(5)))], key=lambda x: x.value[0])
    assert not memory_leak(test, Test)

    def test():
        minmax(Test((Test(1), Test(5))), Test((Test(1), Test(1))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, Test)

    def test():
        minmax(Test((Test(1), Test(5))), Test((Test(1), Test(1))),
               Test((Test(1), Test(2))), key=lambda x: x.value[0])
    assert not memory_leak(test, Test)

    def test():
        minmax(Test((Test(1), Test(5))), Test((Test(1), Test(1))),
               Test((Test(1), Test(2))), Test((Test(1), Test(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, Test)

    def test():
        minmax(Test((Test(5), Test(5))), Test((Test(1), Test(5))),
               Test((Test(1), Test(2))), Test((Test(1), Test(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, Test)

    def test():
        minmax(Test((Test(5), Test(5))), Test((Test(3), Test(5))),
               Test((Test(1), Test(5))), Test((Test(1), Test(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, Test)

    def test():
        minmax(Test((Test(5), Test(5))), Test((Test(3), Test(5))),
               Test((Test(4), Test(5))), Test((Test(1), Test(5))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, Test)

    # Test exceptions
    def test():
        with pytest.raises(TypeError):  # No args
            minmax()
    assert not memory_leak(test, Test)

    def test():
        with pytest.raises(ValueError):  # empty sequence no default
            minmax([])
    assert not memory_leak(test, Test)

    def test():
        with pytest.raises(TypeError):  # invalid kwarg
            minmax(Test(1), Test(2), invalid_kw='a')
    assert not memory_leak(test, Test)

    def test():
        with pytest.raises(TypeError):  # default with multiple args
            minmax(Test(1), Test(2), default=Test(10))
    assert not memory_leak(test, Test)

    def test():
        with pytest.raises(TypeError):  # arg is not iterable
            minmax(Test(100))
    assert not memory_leak(test, Test)

    def test():
        with pytest_raises(TypeError):  # func fails on odd numbered arg
            minmax(Test(100), Test('a'), key=lambda x: x.value + '')
    assert not memory_leak(test, Test)

    def test():
        with pytest_raises(TypeError):  # func fails on even numbered arg
            minmax(Test('a'), Test(100), key=lambda x: x.value + '')
    assert not memory_leak(test, Test)

    if not iteration_utilities.PY2:
        def test():
            with pytest_raises(TypeError):  # unable to compare 1 and 2
                minmax(Test(100), Test('a'))
        assert not memory_leak(test, Test)

        def test():
            with pytest_raises(TypeError):  # unable to compare 3 and 4
                minmax(Test(100), Test(20), Test(100), Test('a'))
        assert not memory_leak(test, Test)

        def test():
            with pytest_raises(TypeError):  # unable to compare 1 and 3
                minmax(Test(1), Test(20), Test('a'), Test('c'))
        assert not memory_leak(test, Test)

        def test():
            with pytest_raises(TypeError):  # unable to compare 2 and 4
                # This is tricky. The elements are explicitly chosen so that
                # 1 compares with 2 without error: 1 current min, 2 current max
                # 3 compares with 4: 3 < 4
                # 3 compares with 1: 1 still current minimum
                # 4 compares not with 2 because the first element is equal and
                # then the comparison with the second element throws and error
                # because str and int are not compareable.
                minmax(Test((100, 'a')), Test((200, 10)),
                       Test((150, 'b')), Test((200, 'd')))
        assert not memory_leak(test, Test)


def test_accumulate():
    accumulate = iteration_utilities.accumulate

    # Test one-argument form
    assert list(accumulate([])) == []
    assert list(accumulate([1, 2, 3])) == [1, 3, 6]

    # Test multiple accumulators
    assert list(accumulate(None, [])) == []
    assert list(accumulate(None, [1, 2, 3, 4])) == [1, 3, 6, 10]
    assert list(accumulate(operator.add, [1, 2, 3, 4])) == [1, 3, 6, 10]
    assert list(accumulate(operator.mul, [1, 2, 3, 4])) == [1, 2, 6, 24]

    # Start value
    assert list(accumulate(None, [1, 2, 3], 10)) == [11, 13, 16]


def test_accumulate_memoryleak():
    accumulate = iteration_utilities.accumulate

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __add__(self, other):
            return self.__class__(self.value + other.value)

        def __mul__(self, other):
            return self.__class__(self.value * other.value)

    def test():
        list(accumulate([Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, Test)

    def test():
        list(accumulate(None, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, Test)

    def test():
        list(accumulate(operator.add, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, Test)

    def test():
        list(accumulate(operator.mul, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, Test)

    def test():
        list(accumulate(operator.add, [Test(1), Test(2), Test(3)], Test(10)))
    assert not memory_leak(test, Test)

    def test():
        with pytest_raises(TypeError):
            list(accumulate(operator.add,
                            [Test(1), Test(2), Test(3)],
                            Test('a')))
    assert not memory_leak(test, Test)

    def test():
        with pytest_raises(TypeError):
            list(accumulate(None,
                            [Test(1), Test(2), Test(3)],
                            Test('a')))
    assert not memory_leak(test, Test)

    def test():
        with pytest_raises(TypeError):
            list(accumulate([Test('a'), Test(2), Test(3)]))
    assert not memory_leak(test, Test)


def test_partition():
    assert iteration_utilities.partition([]) == ([], [])
    assert iteration_utilities.partition([], None) == ([], [])


def test_callbacks():
    assert iteration_utilities.return_True()
    assert not iteration_utilities.return_False()
    assert iteration_utilities.return_None() is None
    assert iteration_utilities.return_first_positional_argument(1, 2, 3) == 1

    assert iteration_utilities.square(2) == 4

    assert iteration_utilities.is_None(None)
    assert not iteration_utilities.is_None(False)
