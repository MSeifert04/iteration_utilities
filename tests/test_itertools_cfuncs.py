# Built-ins
from __future__ import absolute_import, division, print_function
import itertools
import operator
import weakref

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_doctest import doctest_module_no_failure
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


kwargs_memoryleak = {'exclude_object': weakref.ref}


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
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax([Test(1), Test(2)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax([Test(2), Test(1)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax([Test(1), Test(2), Test(3)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax([Test(1), Test(3), Test(2)])
    assert not memory_leak(test, **kwargs_memoryleak)

    # Other input types
    def test():
        minmax(map(Test, range(100)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(map(Test, range(101)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax({Test(1), Test(2), Test(-3)})
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax({Test(1): Test(0), Test(2): Test(0), Test(3): Test(0)})
    assert not memory_leak(test, **kwargs_memoryleak)

    # Test multiple args instead of one sequence
    def test():
        minmax(Test(1), Test(2), Test(3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test(4), Test(3), Test(2), Test(1))
    assert not memory_leak(test, **kwargs_memoryleak)

    # Test key-function
    def test():
        minmax(Test('a'), Test('b'), Test('c'), key=lambda x: x.value.upper())
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test((Test(1), Test(2))), Test((Test(2), Test(3))),
               Test((Test(3), Test(1))), key=lambda x: x.value[1])
    assert not memory_leak(test, **kwargs_memoryleak)

    # Test default value
    def test():
        minmax([], default=Test(10))
    assert not memory_leak(test, **kwargs_memoryleak)

    # Test stablility
    def test():
        minmax([Test((Test(1), Test(5)))], key=lambda x: x.value[0])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test((Test(1), Test(5))), Test((Test(1), Test(1))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test((Test(1), Test(5))), Test((Test(1), Test(1))),
               Test((Test(1), Test(2))), key=lambda x: x.value[0])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test((Test(1), Test(5))), Test((Test(1), Test(1))),
               Test((Test(1), Test(2))), Test((Test(1), Test(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test((Test(5), Test(5))), Test((Test(1), Test(5))),
               Test((Test(1), Test(2))), Test((Test(1), Test(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test((Test(5), Test(5))), Test((Test(3), Test(5))),
               Test((Test(1), Test(5))), Test((Test(1), Test(3))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        minmax(Test((Test(5), Test(5))), Test((Test(3), Test(5))),
               Test((Test(4), Test(5))), Test((Test(1), Test(5))),
               key=lambda x: x.value[0])
    assert not memory_leak(test, **kwargs_memoryleak)

    # Test exceptions
    def test():
        with pytest_raises(TypeError):  # No args
            minmax()
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(ValueError):  # empty sequence no default
            minmax([])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # invalid kwarg
            minmax(Test(1), Test(2), invalid_kw='a')
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # default with multiple args
            minmax(Test(1), Test(2), default=Test(10))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # arg is not iterable
            minmax(Test(100))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # func fails on odd numbered arg
            minmax(Test(100), Test('a'), key=lambda x: x.value + '')
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # func fails on even numbered arg
            minmax(Test('a'), Test(100), key=lambda x: x.value + '')
    assert not memory_leak(test, **kwargs_memoryleak)

    if not iteration_utilities.PY2:
        def test():
            with pytest_raises(TypeError):  # unable to compare 1 and 2
                minmax(Test(100), Test('a'))
        assert not memory_leak(test, **kwargs_memoryleak)

        def test():
            with pytest_raises(TypeError):  # unable to compare 3 and 4
                minmax(Test(100), Test(20), Test(100), Test('a'))
        assert not memory_leak(test, **kwargs_memoryleak)

        def test():
            with pytest_raises(TypeError):  # unable to compare 1 and 3
                minmax(Test(1), Test(20), Test('a'), Test('c'))
        assert not memory_leak(test, **kwargs_memoryleak)

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
        assert not memory_leak(test, **kwargs_memoryleak)


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

    # failures
    with pytest.raises(TypeError):
        list(accumulate(None, [1, 2, 3], 'a'))

    with pytest.raises(TypeError):
        list(accumulate(operator.add, [1, 2, 3], 'a'))

    with pytest.raises(TypeError):
        list(accumulate(['a', 2, 3]))


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
        list(accumulate([]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(accumulate([Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(accumulate(None, []))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(accumulate(None, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(accumulate(operator.add, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(accumulate(operator.mul, [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(accumulate(operator.add, [Test(1), Test(2), Test(3)], Test(10)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            list(accumulate(operator.add,
                            [Test(1), Test(2), Test(3)],
                            Test('a')))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            list(accumulate(None,
                            [Test(1), Test(2), Test(3)],
                            Test('a')))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            list(accumulate([Test('a'), Test(2), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)


def test_partition():
    partition = iteration_utilities.partition
    # One argument form
    assert partition([]) == ([], [])
    assert partition([0, 1, 2]) == ([0], [1, 2])
    assert partition([3, 1, 0]) == ([0], [3, 1])
    assert partition([0, 0, 0]) == ([0, 0, 0], [])
    assert partition([1, 1, 1]) == ([], [1, 1, 1])

    # With predicate function
    assert partition([0, 1, 2], lambda x: x > 1) == ([0, 1], [2])
    assert partition([0, 1, 2], lambda x: x < 1) == ([1, 2], [0])

    # not-iterable
    with pytest.raises(TypeError):
        partition(10)
    with pytest.raises(TypeError):
        partition([1, 2, 3, 4, 'a'], lambda x: x + 3)
    with pytest.raises(TypeError):
        partition([1, 2, 3, 4, 'a'], lambda x: x - 3)
    with pytest.raises(TypeError):
        partition([1, 2, 3, 4, 'a'], lambda x: x + 'a')


def test_partition_memoryleak():
    partition = iteration_utilities.partition

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __bool__(self):
            return bool(self.value)

        def __nonzero__(self):
            return bool(self.value)

    # One argument form
    def test():
        partition([])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        partition([Test(0), Test(1), Test(2)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        partition([Test(3), Test(1), Test(0)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        partition([Test(0), Test(0), Test(0)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        partition([Test(1), Test(1), Test(1)])
    assert not memory_leak(test, **kwargs_memoryleak)

    # With predicate function
    def test():
        partition([Test(0), Test(1), Test(2)], lambda x: x.value > 1)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        partition([Test(0), Test(1), Test(2)], lambda x: x.value < 1)
    assert not memory_leak(test, **kwargs_memoryleak)

    # not-iterable
    def test():
        with pytest_raises(TypeError):
            partition(Test(10))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            partition([Test(1), Test('a')], lambda x: x.value + 3)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            partition([Test(1), Test('a')], lambda x: x.value - 1)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            partition([Test(1), Test('a')], lambda x: x.value + 'a')
    assert not memory_leak(test, **kwargs_memoryleak)


def test_unique_everseen():
    unique_everseen = iteration_utilities.unique_everseen

    assert list(unique_everseen([])) == []
    assert list(unique_everseen([1, 2, 1])) == [1, 2]
    assert list(unique_everseen([1, 2, 1], abs)) == [1, 2]
    assert list(unique_everseen([1, 1, -1], abs)) == [1]

    # unhashable types
    assert list(unique_everseen([{1: 1}, {2: 2}, {1: 1}])) == [{1: 1}, {2: 2}]
    assert list(unique_everseen([[1], [2], [1]])) == [[1], [2]]
    assert list(unique_everseen([[1, 1], [1, 2], [1, 3]],
                                operator.itemgetter(0))) == [[1, 1]]

    # failures
    with pytest.raises(TypeError):
        list(unique_everseen(10))

    with pytest.raises(TypeError):
        list(unique_everseen([1, 2, 3, 'a'], abs))


def test_unique_everseen_memoryleak():
    unique_everseen = iteration_utilities.unique_everseen

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __hash__(self):
            return hash(self.value)

        def __eq__(self, other):
            return self.value == other.value

    def test():
        list(unique_everseen([]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_everseen([Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_everseen([Test(1), Test(2), Test(1)],
                             lambda x: abs(x.value)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_everseen([Test(1), Test(1), Test(-1)],
                             lambda x: abs(x.value)))
    assert not memory_leak(test, **kwargs_memoryleak)

    # unhashable types
    def test():
        list(unique_everseen([{Test(1): Test(1)}, {Test(2): Test(2)},
                              {Test(1): Test(1)}]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_everseen([[Test(1)], [Test(2)], [Test(1)]]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_everseen([[Test(1), Test(1)], [Test(1), Test(2)],
                              [Test(1), Test(3)]], operator.itemgetter(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    # failures
    def test():
        with pytest_raises(TypeError):
            list(unique_everseen(Test(10)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            list(unique_everseen([Test(1), Test(2), Test(3), Test('a')],
                                 lambda x: abs(x.value)))
    assert not memory_leak(test, **kwargs_memoryleak)


def test_first():
    first = iteration_utilities.first

    assert first([1, 2, 3]) == 1
    assert first(range(10)) == 0

    # With pred
    assert first([0, 1, 2], pred=bool) == 1
    assert first([0, 1, 2], pred=None) == 1
    assert first([0]*100 + [1], pred=bool) == 1
    assert first([[1], [1, 2]], pred=lambda x: len(x) > 1) == [1, 2]

    # pred with truthy/retpred
    assert first([0, 1, 2, 3], pred=bool, truthy=False) == 0
    assert first([0, 1, 2, 3], pred=bool, truthy=False, retpred=True) == False
    assert first([0, 1, 2, 3], pred=lambda x: x**2, truthy=False) == 0
    assert first([0, 1, 2, 3],
                 pred=lambda x: x**2, truthy=False, retpred=True) == 0
    assert first([0, 1, 2, 3], pred=bool) == 1
    assert first([0, 1, 2, 3], pred=bool, retpred=True) == True
    assert first([0, 2, 3], pred=lambda x: x**2) == 2
    assert first([0, 2, 3], pred=lambda x: x**2, retpred=True) == 4

    # With default
    assert first([], default=None) is None
    assert first([0, 0, 0], default=None, pred=bool) is None

    # failures
    with pytest.raises(TypeError):
        first(100)

    with pytest.raises(TypeError):
        first([])

    with pytest.raises(TypeError):
        first([0], pred=bool)

    with pytest.raises(TypeError):
        first(['a', 'b'], pred=abs)


def test_first_memoryleak():
    first = iteration_utilities.first

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __bool__(self):
            return bool(self.value)

        def __nonzero__(self):
            return bool(self.value)

        def __pow__(self, other):
            return self.__class__(self.value ** other.value)

    def test():
        first([Test(1), Test(2), Test(3)])
    assert not memory_leak(test, **kwargs_memoryleak)

    # With pred
    def test():
        first([Test(0), Test(1), Test(2)], pred=bool)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(1), Test(2)], pred=None)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0)]*100 + [Test(1)], pred=bool)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([[Test(0)], [Test(1), Test(2)]], pred=lambda x: len(x) > 1)
    assert not memory_leak(test, **kwargs_memoryleak)

    # pred with truthy/retpred
    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool, truthy=False)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool, truthy=False, retpred=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2), truthy=False)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2), truthy=False, retpred=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool, retpred=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2), retpred=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    # With default
    def test():
        first([], default=None) is None
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        first([Test(0), Test(0), Test(0)], default=None, pred=bool) is None
    assert not memory_leak(test, **kwargs_memoryleak)

    # failures
    def test():
        with pytest_raises(TypeError):
            first([])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            first(Test(100))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            first([Test(0)], pred=bool)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            first([Test('a'), Test('b')], pred=lambda x: abs(x.value))
    assert not memory_leak(test, **kwargs_memoryleak)


def test_applyfunc():
    applyfunc = iteration_utilities.applyfunc
    take = iteration_utilities.take

    assert take(applyfunc(lambda x: x**2, 2), 3) == [4, 16, 256]
    assert take(applyfunc(lambda x: x, 2), 3) == [2, 2, 2]

    with pytest.raises(TypeError):
        take(applyfunc(lambda x: x**2, 'a'), 3)


def test_applyfunc_memoryleak():
    applyfunc = iteration_utilities.applyfunc
    take = iteration_utilities.take

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __pow__(self, other):
            return self.__class__(self.value**other.value)

    def test():
        take(applyfunc(lambda x: x**Test(2), Test(2)), 3)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        take(applyfunc(lambda x: x, Test(2)), 3)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            take(applyfunc(lambda x: x**Test(2), Test('a')), 3)
    assert not memory_leak(test, **kwargs_memoryleak)


def test_successive():
    successive = iteration_utilities.successive

    assert list(successive([])) == []
    assert list(successive([1])) == []
    assert list(successive([], times=10)) == []
    assert list(successive([1, 2, 3, 4, 5], times=10)) == []

    assert list(successive(range(4))) == [(0, 1), (1, 2), (2, 3)]
    assert list(successive(range(4), times=3)) == [(0, 1, 2), (1, 2, 3)]
    assert list(successive(range(4), times=4)) == [(0, 1, 2, 3)]

    with pytest.raises(TypeError):
        successive(10)

    with pytest.raises(ValueError):  # times must be > 0
        successive([1, 2, 3], 0)


def test_successive_memoryleak():
    successive = iteration_utilities.successive

    class Test(object):
        def __init__(self, value):
            self.value = value

    def test():
        list(successive([]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(successive([Test(1)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(successive([Test(1), Test(2), Test(3)], times=10))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(successive([Test(1), Test(2), Test(3), Test(4)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(successive([Test(1), Test(2), Test(3), Test(4)], times=3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(successive([Test(1), Test(2), Test(3), Test(4)], times=4))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            successive(Test(1))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(ValueError):  # times must be > 0
            successive([Test(1), Test(2), Test(3)], 0)
    assert not memory_leak(test, **kwargs_memoryleak)


def test_roundrobin():
    roundrobin = iteration_utilities.roundrobin

    assert list(roundrobin()) == []
    assert list(roundrobin([])) == []
    assert list(roundrobin([], (), {})) == []

    assert list(roundrobin([1], [1, 2], [1, 2, 3])) == [1, 1, 1, 2, 2, 3]
    assert list(roundrobin([1, 2, 3], [1], [1, 2])) == [1, 1, 1, 2, 2, 3]
    assert list(roundrobin([1, 2], [1, 2, 3], [1])) == [1, 1, 1, 2, 2, 3]

    with pytest.raises(TypeError):
        list(roundrobin(10))

    with pytest.raises(TypeError):
        list(roundrobin([10], 100))


def test_roundrobin_memoryleak():
    roundrobin = iteration_utilities.roundrobin

    class Test(object):
        def __init__(self, value):
            self.value = value

    def test():
        list(roundrobin())
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(roundrobin([]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(roundrobin([], (), {}))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(roundrobin([Test(1)],
                        [Test(1), Test(2)],
                        [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(roundrobin([Test(1), Test(2), Test(3)],
                        [Test(1)],
                        [Test(1), Test(2)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(roundrobin([Test(1), Test(2)],
                        [Test(1), Test(2), Test(3)],
                        [Test(1)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            list(roundrobin(Test(1)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            list(roundrobin([Test(1)], Test(1)))
    assert not memory_leak(test, **kwargs_memoryleak)


def test_merge():
    merge = iteration_utilities.merge

    assert list(merge()) == []
    assert list(merge([])) == []
    assert list(merge([], (), {})) == []

    for seq in itertools.permutations([[1], [2], [3]]):
        assert list(merge(*seq)) == [1, 2, 3]

    for seq in itertools.permutations([[1], [2], [3], []]):
        assert list(merge(*seq)) == [1, 2, 3]

    for seq in itertools.permutations([[1, 2.5], [2], [3]]):
        assert list(merge(*seq)) == [1, 2, 2.5, 3]

    for seq in itertools.permutations([[1, 2.5], [0.5, 2], [3]]):
        assert list(merge(*seq)) == [0.5, 1, 2, 2.5, 3]

    for seq in itertools.permutations([range(5), range(3), range(4, 7)]):
        assert list(merge(*seq)) == [0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6]

    # Stability tests
    it = merge([1], [1.])
    item1 = next(it)
    assert isinstance(item1, int)
    item2 = next(it)
    assert isinstance(item2, float)

    # Key function tests
    seq = ([(1, 0), (2, 0)], [(1, -1), (2, -1)])
    assert (list(merge(*seq, key=operator.itemgetter(0))) ==
            [(1, 0), (1, -1), (2, 0), (2, -1)])

    # Reverse test
    for seq in itertools.permutations([[1], [2], [3]]):
        assert list(merge(*seq, reverse=True)) == [3, 2, 1]

    for seq in itertools.permutations([[1], [2], [3], []]):
        assert list(merge(*seq, reverse=True)) == [3, 2, 1]

    for seq in itertools.permutations([[2.5, 1], [2], [3]]):
        assert list(merge(*seq, reverse=True)) == [3, 2.5, 2, 1]

    for seq in itertools.permutations([[2.5, 1], [2, 0.5], [3]]):
        assert list(merge(*seq, reverse=True)) == [3, 2.5, 2, 1, 0.5]

    # Key+reverse function tests
    seq = ([(2, 0), (1, 0)], [(2, -1), (1, -1)])
    assert (list(merge(*seq, reverse=True, key=operator.itemgetter(0))) ==
            [(2, 0), (2, -1), (1, 0), (1, -1)])

    # One iterable is not iterable
    with pytest.raises(TypeError):
        merge(10)

    with pytest.raises(TypeError):
        merge([10, 20], 10)

    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], reverse=True, key=abs, wrongkwd=True)

    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], reverse=True, wrongkwd=True)

    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], key=abs, wrongkwd=True)

    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], wrongkwd=True)

    # Key function fails
    with pytest.raises(TypeError):
        list(merge([2, (2, 0)], [(1, 2), (1, 3)], key=operator.itemgetter(0)))

    with pytest.raises(TypeError):
        list(merge([(2, 0), 2], [(1, 2), (1, 3)], key=operator.itemgetter(0)))

    # Comparison fails
    if not iteration_utilities.PY2:
        with pytest.raises(TypeError):
            list(merge(['a', 'b'], [2, 3]))

        with pytest.raises(TypeError):
            list(merge([1, 'b'], [2, 3]))


def test_merge_memoryleak():
    merge = iteration_utilities.merge

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __gt__(self, other):
            return self.value > other.value

        def __lt__(self, other):
            return self.value < other.value

    def test():
        list(merge())
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(merge([]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(merge([], (), {}))
    assert not memory_leak(test, **kwargs_memoryleak)

    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test, **kwargs_memoryleak)

    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)], []]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test, **kwargs_memoryleak)

    for seq in itertools.permutations([[Test(1), Test(2.5)],
                                       [Test(2)], [Test(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test, **kwargs_memoryleak)

    for seq in itertools.permutations([[Test(1), Test(2.5)],
                                       [Test(0.5), Test(2)],
                                       [Test(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test, **kwargs_memoryleak)

    # Stability tests
    def test():
        list(merge([Test(1)], [Test(1.)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    # Key function tests
    def test():
        seq = ([(Test(1), Test(0)), (Test(2), Test(0))],
               [(Test(1), Test(-1)), (Test(2), Test(-1))])
        list(merge(*seq, key=operator.itemgetter(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    # Reverse test
    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test, **kwargs_memoryleak)

    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)], []]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test, **kwargs_memoryleak)

    for seq in itertools.permutations([[Test(2.5), Test(1)],
                                       [Test(2)],
                                       [Test(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test, **kwargs_memoryleak)

    for seq in itertools.permutations([[Test(2.5), Test(1)],
                                       [Test(2), Test(0.5)],
                                       [Test(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test, **kwargs_memoryleak)

    # Key+reverse function tests
    def test():
        seq = ([(Test(2), Test(0)), (Test(1), Test(0))],
               [(Test(2), Test(-1)), (Test(1), Test(-1))])
        list(merge(*seq, reverse=True, key=operator.itemgetter(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    # One iterable is not iterable
    def test():
        with pytest_raises(TypeError):
            merge(Test(10))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], Test(10))
    assert not memory_leak(test, **kwargs_memoryleak)

    # Unexpected keyword argument
    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  reverse=True, key=abs, wrongkwd=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  reverse=True, wrongkwd=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  key=abs, wrongkwd=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  wrongkwd=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    # Key function fails
    def test():
        with pytest_raises(TypeError):
            list(merge([Test(2), (Test(2), Test(0))],
                       [(Test(1), Test(2)), (Test(1), Test(3))],
                       key=operator.itemgetter(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            list(merge([(Test(2), Test(0)), Test(2)],
                       [(Test(1), Test(2)), (Test(1), Test(3))],
                       key=operator.itemgetter(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    # Comparison fails
    if not iteration_utilities.PY2:
        def test():
            with pytest_raises(TypeError):
                list(merge([Test('a'), Test('b')], [Test(2), Test(3)]))
        assert not memory_leak(test, **kwargs_memoryleak)

        def test():
            with pytest_raises(TypeError):
                list(merge([Test(1), Test('b')], [Test(2), Test(3)]))
        assert not memory_leak(test, **kwargs_memoryleak)


def test_grouper():
    grouper = iteration_utilities.grouper
    # Empty iterable
    assert list(grouper([], 2)) == []

    # no fillvalue + truncate
    assert list(grouper([1], 3)) == [(1, )]
    assert list(grouper([1, 2], 3)) == [(1, 2)]
    assert list(grouper([1, 2, 3], 3)) == [(1, 2, 3)]
    assert list(grouper([1, 2, 3, 4], 3)) == [(1, 2, 3), (4, )]
    assert list(grouper([1, 2, 3, 4, 5], 3)) == [(1, 2, 3), (4, 5)]
    assert list(grouper([1, 2, 3, 4, 5, 6], 3)) == [(1, 2, 3), (4, 5, 6)]

    # with fillvalue
    assert list(grouper([1], 3,
                        fillvalue=0)) == [(1, 0, 0)]
    assert list(grouper([1, 2], 3,
                        fillvalue=0)) == [(1, 2, 0)]
    assert list(grouper([1, 2, 3], 3,
                        fillvalue=0)) == [(1, 2, 3)]
    assert list(grouper([1, 2, 3, 4], 3,
                        fillvalue=0)) == [(1, 2, 3), (4, 0, 0)]
    assert list(grouper([1, 2, 3, 4, 5], 3,
                        fillvalue=0)) == [(1, 2, 3), (4, 5, 0)]
    assert list(grouper([1, 2, 3, 4, 5, 6], 3,
                        fillvalue=0)) == [(1, 2, 3), (4, 5, 6)]

    # with truncate
    assert list(grouper([1], 3,
                        truncate=True)) == []
    assert list(grouper([1, 2], 3,
                        truncate=True)) == []
    assert list(grouper([1, 2, 3], 3,
                        truncate=True)) == [(1, 2, 3)]
    assert list(grouper([1, 2, 3, 4], 3,
                        truncate=True)) == [(1, 2, 3)]
    assert list(grouper([1, 2, 3, 4, 5], 3,
                        truncate=True)) == [(1, 2, 3)]
    assert list(grouper([1, 2, 3, 4, 5, 6], 3,
                        truncate=True)) == [(1, 2, 3), (4, 5, 6)]

    # failures
    with pytest.raises(TypeError):  # fillvalue + truncate is forbidden
        grouper([1, 2, 3], 2, fillvalue=None, truncate=True)

    with pytest.raises(ValueError):  # n must be > 0
        grouper([1, 2, 3], 0)

    with pytest.raises(TypeError):  # iterable must be iterable
        grouper(1, 2)


def test_grouper_memoryleak():
    grouper = iteration_utilities.grouper

    class Test(object):
        def __init__(self, value):
            self.value = value

    # Empty iterable
    def test():
        list(grouper([], 2))
    assert not memory_leak(test, **kwargs_memoryleak)

    # no fillvalue + truncate
    def test():
        list(grouper([Test(1)], 3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2)], 3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3)], 3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4)], 3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5)], 3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5), Test(6)],
                     3))
    assert not memory_leak(test, **kwargs_memoryleak)

    # with fillvalue
    def test():
        list(grouper([Test(1)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5), Test(6)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    # with truncate
    def test():
        list(grouper([Test(1)], 3,
                     truncate=True))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2)], 3,
                     truncate=True))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3)], 3,
                     truncate=True))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4)], 3,
                     truncate=True))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5)], 3,
                     truncate=True))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5), Test(6)], 3,
                     truncate=True))
    assert not memory_leak(test, **kwargs_memoryleak)

    # failures
    def test():
        with pytest_raises(TypeError):  # fillvalue + truncate is forbidden
            grouper([Test(1), Test(2), Test(3)], 2,
                    fillvalue=Test(0), truncate=True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(ValueError):  # n must be > 0
            grouper([Test(1), Test(2), Test(3)], 0)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # iterable must be iterable
            grouper(Test(1), 2)
    assert not memory_leak(test, **kwargs_memoryleak)


def test_ilen():
    ilen = iteration_utilities.ilen

    assert ilen([]) == 0
    assert ilen(range(10)) == 10
    assert ilen([1, 2, 3, 4, 5]) == 5

    generator = (i for i in [1, 2, 3, 4, 5])
    assert ilen(generator) == 5
    assert ilen(generator) == 0

    with pytest.raises(TypeError):
        ilen(100)


def test_ilen_memoryleak():
    ilen = iteration_utilities.ilen

    class Test(object):
        def __init__(self, value):
            self.value = value

    def test():
        ilen([])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        ilen(range(10))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        ilen([Test(1), Test(2), Test(3), Test(4), Test(5)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        generator = (i for i in [Test(1), Test(2), Test(3), Test(4), Test(5)])
        ilen(generator) == 5
        ilen(generator) == 0
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            ilen(100)
    assert not memory_leak(test, **kwargs_memoryleak)


def test_intersperse():
    intersperse = iteration_utilities.intersperse

    assert list(intersperse([], 0)) == []
    assert list(intersperse([1], 0)) == [1]
    assert list(intersperse([1, 2], 0)) == [1, 0, 2]

    with pytest.raises(TypeError):
        intersperse(100, 0)


def test_intersperse_memoryleak():
    intersperse = iteration_utilities.intersperse

    class Test(object):
        def __init__(self, value):
            self.value = value

    def test():
        list(intersperse([], Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(intersperse([Test(1)], Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(intersperse([Test(1), Test(2)], Test(0)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            intersperse(Test(100), Test(0))
    assert not memory_leak(test, **kwargs_memoryleak)


def test_complement():
    complement = iteration_utilities.complement

    assert not complement(lambda x: x is True)(True)
    assert complement(lambda x: x is True)(False)

    assert complement(lambda x: x is False)(True)
    assert not complement(lambda x: x is False)(False)

    assert not complement(iteration_utilities.is_None)(None)
    assert complement(iteration_utilities.is_None)(False)
    assert complement(iteration_utilities.is_None)(True)


def test_complement_memoryleak():
    complement = iteration_utilities.complement

    def test():
        complement(lambda x: x is True)(True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        complement(lambda x: x is True)(False)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        complement(lambda x: x is False)(True)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        complement(lambda x: x is False)(False)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        complement(iteration_utilities.is_None)(None)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        complement(iteration_utilities.is_None)(False)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        complement(iteration_utilities.is_None)(True)
    assert not memory_leak(test, **kwargs_memoryleak)


def test_one():
    one = iteration_utilities.one

    assert one([0]) == 0
    assert one('a') == 'a'
    assert one({'o': 10}) == 'o'

    with pytest.raises(TypeError):
        one(10)

    with pytest.raises(ValueError):  # empty iterable
        one([])

    with pytest.raises(ValueError):  # more than 1 element
        one([1, 2])


def test_one_memoryleak():
    one = iteration_utilities.one

    class Test(object):
        def __init__(self, value):
            self.value = value

    def test():
        one([Test(0)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        one('a')
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        one({Test(0): 10})
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            one(Test(0))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(ValueError):  # empty iterable
            one([])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(ValueError):  # more than 1 element
            one([1, 2])
    assert not memory_leak(test, **kwargs_memoryleak)


def test_nth():
    nth = iteration_utilities.nth

    assert nth([], 10, None) is None
    assert nth([1, 2, 3], 0) == 1
    assert nth([1, 2, 3], 1) == 2
    assert nth([1, 2, 3], 2) == 3

    with pytest.raises(TypeError):  # not iterable
        nth(1, 10)

    with pytest.raises(IndexError):  # nth not in iterable
        nth([], 10)


def test_nth_memoryleak():
    nth = iteration_utilities.nth

    class Test(object):
        def __init__(self, value):
            self.value = value

    def test():
        nth([], 10, Test(1))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        nth([Test(1), Test(2), Test(3)], 0)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        nth([Test(1), Test(2), Test(3)], 1)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        nth([Test(1), Test(2), Test(3)], 2)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # not iterable
            nth(Test(1), 10)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(IndexError):  # nth not in iterable
            nth([], 10)
    assert not memory_leak(test, **kwargs_memoryleak)


def test_quantify():
    quantify = iteration_utilities.quantify

    assert quantify([]) == 0
    assert quantify([0, 0]) == 0
    assert quantify([0, 0, 1]) == 1
    assert quantify([0, 0, 1, 1], None) == 2

    assert quantify([], lambda x: x) == 0

    assert quantify([1, 2, 3], lambda x: x > 2) == 1
    assert quantify([1, 2, 3], lambda x: x < 3) == 2

    with pytest.raises(TypeError):
        quantify(1)

    with pytest.raises(TypeError):
        quantify([1], 1)


def test_quantify_memoryleak():
    quantify = iteration_utilities.quantify

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __lt__(self, other):
            return self.value < other.value

        def __gt__(self, other):
            return self.value > other.value

        def __bool__(self):
            return bool(self.value)

        def __nonzero__(self):
            return bool(self.value)

    def test():
        quantify([])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        quantify([Test(0), Test(0)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        quantify([Test(0), Test(0), Test(1)])
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        quantify([Test(0), Test(0), Test(1), Test(1)], None)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        quantify([], iteration_utilities.return_first_positional_argument)
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        quantify([Test(1), Test(2), Test(3)], lambda x: x > Test(2))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        quantify([Test(1), Test(2), Test(3)], lambda x: x < Test(3))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            quantify(Test(1))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):
            quantify([Test(1)], Test(1))
    assert not memory_leak(test, **kwargs_memoryleak)


def test_unique_justseen():
    unique_justseen = iteration_utilities.unique_justseen

    assert list(unique_justseen([])) == []
    assert list(unique_justseen([1, 1, 2, 3, 3])) == [1, 2, 3]
    assert list(unique_justseen('aAabBb')) == ['a', 'A', 'a', 'b', 'B', 'b']
    assert list(unique_justseen('aAabBb', key=str.lower)) == ['a', 'b']

    with pytest.raises(TypeError):  # not iterable
        unique_justseen(10)

    with pytest.raises(TypeError):  # function call fails
        list(unique_justseen([1, 2, 3], key=lambda x: x + 'a'))

    class Test2(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            raise TypeError()

    with pytest.raises(TypeError):  # objects do not support eq or ne
        list(unique_justseen([Test2(1), Test2(2)]))

    class Test3(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            return self.value != other.value

    res = list(unique_justseen([Test3(1), Test3(1)]))
    assert len(res) == 1
    assert isinstance(res[0], Test3)
    assert res[0].value == 1


def test_unique_justseen_memoryleak():
    unique_justseen = iteration_utilities.unique_justseen

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            return self.value == other.value

    class Test2(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            raise TypeError()

    class Test3(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            return self.value != other.value

    def test():
        list(unique_justseen([])) == []
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_justseen([Test(1), Test(1), Test(2), Test(3), Test(3)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_justseen([Test(1), Test(-1), Test(1),
                              Test(2), Test(-2), Test(2)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_justseen([Test(1), Test(-1), Test(1),
                              Test(2), Test(-2), Test(2)],
                             key=lambda x: abs(x.value)))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # not iterable
            unique_justseen(Test(1))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # function call fails
            list(unique_justseen([Test(1), Test(2), Test(3)],
                                 key=lambda x: x + 'a'))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        with pytest_raises(TypeError):  # objects do not support eq or ne
            list(unique_justseen([Test2(1), Test2(2)]))
    assert not memory_leak(test, **kwargs_memoryleak)

    def test():
        list(unique_justseen([Test3(1), Test3(1)]))
    assert not memory_leak(test, **kwargs_memoryleak)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='Python 2 does not support this way of pickling.')
def test_cfuncs_pickle():
    import pickle

    accumulate = iteration_utilities.accumulate
    applyfunc = iteration_utilities.applyfunc
    grouper = iteration_utilities.grouper
    intersperse = iteration_utilities.intersperse
    merge = iteration_utilities.merge
    unique_everseen = iteration_utilities.unique_everseen
    unique_justseen = iteration_utilities.unique_justseen
    successive = iteration_utilities.successive
    roundrobin = iteration_utilities.roundrobin
    complement = iteration_utilities.complement

    # IMPORTANT: methoddescriptors like "str.lower" as key functions can not
    #            be pickled before python 3.4

    # ----- Accumulate
    acc = accumulate([1, 2, 3, 4])
    assert next(acc) == 1
    x = pickle.dumps(acc)
    assert list(pickle.loads(x)) == [3, 6, 10]

    # ----- Applyfunc
    apf = applyfunc(iteration_utilities.square, 2)
    assert next(apf) == 4
    x = pickle.dumps(apf)
    assert next(pickle.loads(x)) == 16

    # ----- Unique Everseen
    uqe = unique_everseen([1, 2, 1, 2])
    assert next(uqe) == 1
    x = pickle.dumps(uqe)
    assert list(pickle.loads(x)) == [2]

    # ----- Successive
    suc = successive([1, 2, 3, 4])
    assert next(suc) == (1, 2)
    x = pickle.dumps(suc)
    assert list(pickle.loads(x)) == [(2, 3), (3, 4)]

    # ----- Roundrobin
    rr = roundrobin([1, 2, 3], [1, 2, 3])
    assert next(rr) == 1
    x = pickle.dumps(rr)
    assert list(pickle.loads(x)) == [1, 2, 2, 3, 3]

    rr2 = roundrobin([1], [1, 2, 3])
    assert next(rr2) == 1
    assert next(rr2) == 1
    assert next(rr2) == 2
    x = pickle.dumps(rr2)
    assert list(pickle.loads(x)) == [3]

    # ----- Merge
    mge = merge([0], [1, 2], [2])
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, 2]

    mge = merge([1, 2], [0], [-2], key=abs)
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, -2]

    mge = merge([2, 1], [0], [3], reverse=True)
    assert next(mge) == 3
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [2, 1, 0]

    # ----- Grouper
    grp = grouper(range(10), 3)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9,)]

    grp = grouper(range(10), 3, fillvalue=0)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9, 0, 0)]

    grp = grouper(range(10), 3, truncate=True)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8)]

    # ----- Intersperse
    its = intersperse([1, 2, 3], 0)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [1, 0, 2, 0, 3]

    its = intersperse([1, 2, 3], 0)  # start value must be set!
    assert next(its) == 1
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 2, 0, 3]

    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [2, 0, 3]

    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    assert next(its) == 2
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 3]

    # ----- Complement
    x = pickle.dumps(complement(iteration_utilities.is_None))
    assert pickle.loads(x)(False)
    assert pickle.loads(x)(True)
    assert not pickle.loads(x)(None)

    # ----- Unique_justseen
    ujs = unique_justseen([1, 2, 3])
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [1, 2, 3]

    ujs = unique_justseen([1, 2, 3])
    assert next(ujs) == 1
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [2, 3]

    # Pickling a method descriptor is not possible for Python 3.3 and before
    # Also operator.methodcaller loses it's methodname when pickled for Python
    #   3.4 and lower...
    if iteration_utilities.PY34:
        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        x = pickle.dumps(ujs)
        assert list(pickle.loads(x)) == ['a']

        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        assert next(ujs) == 'a'
        x = pickle.dumps(ujs)
        assert list(pickle.loads(x)) == []


def test_callbacks():
    assert iteration_utilities.return_True()
    assert not iteration_utilities.return_False()
    assert iteration_utilities.return_None() is None
    assert iteration_utilities.return_identity(1) == 1
    assert iteration_utilities.return_first_positional_argument(1, 2, 3) == 1
    assert iteration_utilities.return_called(int) == 0

    assert iteration_utilities.square(2) == 4
    assert iteration_utilities.one_over(2) == 0.5

    assert iteration_utilities.is_None(None)
    assert not iteration_utilities.is_None(False)

    assert not iteration_utilities.is_not_None(None)
    assert iteration_utilities.is_not_None(False)
