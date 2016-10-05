# Built-ins
from __future__ import absolute_import, division, print_function
import itertools
import operator

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


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
    assert not memory_leak(test)

    # With pred
    def test():
        first([Test(0), Test(1), Test(2)], pred=bool)
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(1), Test(2)], pred=None)
    assert not memory_leak(test)

    def test():
        first([Test(0)]*100 + [Test(1)], pred=bool)
    assert not memory_leak(test)

    def test():
        first([[Test(0)], [Test(1), Test(2)]], pred=lambda x: len(x) > 1)
    assert not memory_leak(test)

    # pred with truthy/retpred
    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool, truthy=False)
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool, truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2), truthy=False)
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2), truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool)
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=bool, retpred=True)
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2))
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(2), Test(3)],
              pred=lambda x: x**Test(2), retpred=True)
    assert not memory_leak(test)

    # With default
    def test():
        first([], default=None) is None
    assert not memory_leak(test)

    def test():
        first([Test(0), Test(0), Test(0)], default=None, pred=bool) is None
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(TypeError):
            first([])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            first(Test(100))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            first([Test(0)], pred=bool)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            first([Test('a'), Test('b')], pred=lambda x: abs(x.value))
    assert not memory_leak(test)


def test_second():
    second = iteration_utilities.second

    assert second([1, 2, 3]) == 2
    assert second(range(10)) == 1

    # With pred
    assert second([0, 1, 2], pred=bool) == 2
    assert second([0, 1, 2], pred=None) == 2
    assert second([0]*100 + [1]*2, pred=bool) == 1
    assert second([[1], [1, 2], [1, 2]], pred=lambda x: len(x) > 1) == [1, 2]

    # pred with truthy/retpred
    assert second([0, 1, 2, 3, 0], pred=bool, truthy=False) == 0
    assert second([0, 1, 2, 3, 0], pred=bool, truthy=False,
                  retpred=True) == False
    assert second([0, 1, 2, 3, 0], pred=lambda x: x**2, truthy=False) == 0
    assert second([0, 1, 2, 3, 0],
                  pred=lambda x: x**2, truthy=False, retpred=True) == 0
    assert second([0, 1, 2, 3], pred=bool) == 2
    assert second([0, 1, 2, 3], pred=bool, retpred=True) == True
    assert second([0, 2, 3], pred=lambda x: x**2) == 3
    assert second([0, 2, 3], pred=lambda x: x**2, retpred=True) == 9

    # With default
    assert second([], default=None) is None
    assert second([0, 0, 0], default=None, pred=bool) is None

    # failures
    with pytest.raises(TypeError):
        second(100)

    with pytest.raises(TypeError):
        second([])

    with pytest.raises(TypeError):
        second([10])

    with pytest.raises(TypeError):
        second([0], pred=bool)

    with pytest.raises(TypeError):
        second([1], pred=bool)

    with pytest.raises(TypeError):
        second(['a', 'b'], pred=abs)


def test_second_memoryleak():
    second = iteration_utilities.second

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
        second([Test(1), Test(2), Test(3)])
    assert not memory_leak(test)

    # With pred
    def test():
        second([Test(0), Test(1), Test(2)], pred=bool)
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(1), Test(2)], pred=None)
    assert not memory_leak(test)

    def test():
        second([Test(0)]*100 + [Test(1)]*2, pred=bool)
    assert not memory_leak(test)

    def test():
        second([[Test(0)], [Test(1), Test(2)]]*2, pred=lambda x: len(x) > 1)
    assert not memory_leak(test)

    # pred with truthy/retpred
    def test():
        second([Test(0), Test(2), Test(3), Test(0)],
               pred=bool, truthy=False)
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(2), Test(3), Test(0)],
               pred=bool, truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(2), Test(3), Test(0)],
               pred=lambda x: x**Test(2), truthy=False)
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(2), Test(3), Test(0)],
               pred=lambda x: x**Test(2), truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(2), Test(3)],
               pred=bool)
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(2), Test(3)],
               pred=bool, retpred=True)
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(2), Test(3)],
               pred=lambda x: x**Test(2))
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(2), Test(3)],
               pred=lambda x: x**Test(2), retpred=True)
    assert not memory_leak(test)

    # With default
    def test():
        second([], default=None) is None
    assert not memory_leak(test)

    def test():
        second([Test(0), Test(0), Test(0)], default=None, pred=bool) is None
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(TypeError):
            second([])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            second([Test(0)])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            second(Test(100))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            second([Test(1)], pred=bool)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            second([Test('a'), Test('b')], pred=lambda x: abs(x.value))
    assert not memory_leak(test)


def test_last():
    last = iteration_utilities.last

    assert last([1, 2, 3]) == 3
    assert last(range(10)) == 9

    # With pred
    assert last([0, 1, 2], pred=bool) == 2
    assert last([0, 1, 2], pred=None) == 2
    assert last([0]*100 + [1], pred=bool) == 1
    assert last([[1], [1, 2], [1, 2]], pred=lambda x: len(x) > 1) == [1, 2]

    # pred with truthy/retpred
    assert last([0, 1, 2, 3, 0], pred=bool, truthy=False) == 0
    assert last([0, 1, 2, 3, 0], pred=bool, truthy=False,
                retpred=True) == False
    assert last([0, 1, 2, 3, 0], pred=lambda x: x**2, truthy=False) == 0
    assert last([0, 1, 2, 3, 0],
                pred=lambda x: x**2, truthy=False, retpred=True) == 0
    assert last([0, 1, 2, 3], pred=bool) == 3
    assert last([0, 1, 2, 3], pred=bool, retpred=True) == True
    assert last([0, 2, 3, 4], pred=lambda x: x**2) == 4
    assert last([0, 2, 3, 4], pred=lambda x: x**2, retpred=True) == 16

    # With default
    assert last([], default=None) is None
    assert last([0, 0, 0], default=None, pred=bool) is None

    # failures
    with pytest.raises(TypeError):
        last(100)

    with pytest.raises(TypeError):
        last([])

    with pytest.raises(TypeError):
        last([0], pred=bool)

    with pytest.raises(TypeError):
        last(['a', 'b'], pred=abs)


def test_last_memoryleak():
    last = iteration_utilities.last

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
        last([Test(1), Test(2), Test(3)])
    assert not memory_leak(test)

    # With pred
    def test():
        last([Test(0), Test(1), Test(2)], pred=bool)
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(1), Test(2)], pred=None)
    assert not memory_leak(test)

    def test():
        last([Test(0)]*100 + [Test(1)]*2, pred=bool)
    assert not memory_leak(test)

    def test():
        last([[Test(0)], [Test(1), Test(2)]]*2, pred=lambda x: len(x) > 1)
    assert not memory_leak(test)

    # pred with truthy/retpred
    def test():
        last([Test(0), Test(2), Test(3), Test(0)],
             pred=bool, truthy=False)
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(2), Test(3), Test(0)],
             pred=bool, truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(2), Test(3), Test(0)],
             pred=lambda x: x**Test(2), truthy=False)
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(2), Test(3), Test(0)],
             pred=lambda x: x**Test(2), truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(2), Test(3)], pred=bool)
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(2), Test(3)], pred=bool, retpred=True)
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(2), Test(3)], pred=lambda x: x**Test(2))
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(2), Test(3)],
             pred=lambda x: x**Test(2), retpred=True)
    assert not memory_leak(test)

    # With default
    def test():
        last([], default=None) is None
    assert not memory_leak(test)

    def test():
        last([Test(0), Test(0), Test(0)], default=None, pred=bool) is None
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(TypeError):
            last([])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            last(Test(100))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            last([Test('a'), Test('b')], pred=lambda x: abs(x.value))
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        take(applyfunc(lambda x: x, Test(2)), 3)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            take(applyfunc(lambda x: x**Test(2), Test('a')), 3)
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        list(successive([Test(1)]))
    assert not memory_leak(test)

    def test():
        list(successive([Test(1), Test(2), Test(3)], times=10))
    assert not memory_leak(test)

    def test():
        list(successive([Test(1), Test(2), Test(3), Test(4)]))
    assert not memory_leak(test)

    def test():
        list(successive([Test(1), Test(2), Test(3), Test(4)], times=3))
    assert not memory_leak(test)

    def test():
        list(successive([Test(1), Test(2), Test(3), Test(4)], times=4))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            successive(Test(1))
    assert not memory_leak(test)

    def test():
        with pytest_raises(ValueError):  # times must be > 0
            successive([Test(1), Test(2), Test(3)], 0)
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        list(roundrobin([]))
    assert not memory_leak(test)

    def test():
        list(roundrobin([], (), {}))
    assert not memory_leak(test)

    def test():
        list(roundrobin([Test(1)],
                        [Test(1), Test(2)],
                        [Test(1), Test(2), Test(3)]))
    assert not memory_leak(test)

    def test():
        list(roundrobin([Test(1), Test(2), Test(3)],
                        [Test(1)],
                        [Test(1), Test(2)]))
    assert not memory_leak(test)

    def test():
        list(roundrobin([Test(1), Test(2)],
                        [Test(1), Test(2), Test(3)],
                        [Test(1)]))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            list(roundrobin(Test(1)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            list(roundrobin([Test(1)], Test(1)))
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        list(merge([]))
    assert not memory_leak(test)

    def test():
        list(merge([], (), {}))
    assert not memory_leak(test)

    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)

    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)], []]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)

    for seq in itertools.permutations([[Test(1), Test(2.5)],
                                       [Test(2)], [Test(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)

    for seq in itertools.permutations([[Test(1), Test(2.5)],
                                       [Test(0.5), Test(2)],
                                       [Test(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)

    # Stability tests
    def test():
        list(merge([Test(1)], [Test(1.)]))
    assert not memory_leak(test)

    # Key function tests
    def test():
        seq = ([(Test(1), Test(0)), (Test(2), Test(0))],
               [(Test(1), Test(-1)), (Test(2), Test(-1))])
        list(merge(*seq, key=operator.itemgetter(0)))
    assert not memory_leak(test)

    # Reverse test
    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)

    for seq in itertools.permutations([[Test(1)], [Test(2)], [Test(3)], []]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)

    for seq in itertools.permutations([[Test(2.5), Test(1)],
                                       [Test(2)],
                                       [Test(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)

    for seq in itertools.permutations([[Test(2.5), Test(1)],
                                       [Test(2), Test(0.5)],
                                       [Test(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)

    # Key+reverse function tests
    def test():
        seq = ([(Test(2), Test(0)), (Test(1), Test(0))],
               [(Test(2), Test(-1)), (Test(1), Test(-1))])
        list(merge(*seq, reverse=True, key=operator.itemgetter(0)))
    assert not memory_leak(test)

    # One iterable is not iterable
    def test():
        with pytest_raises(TypeError):
            merge(Test(10))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], Test(10))
    assert not memory_leak(test)

    # Unexpected keyword argument
    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  reverse=True, key=abs, wrongkwd=True)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  reverse=True, wrongkwd=True)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  key=abs, wrongkwd=True)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            merge([Test(10), Test(20)], [Test(20), Test(30)],
                  wrongkwd=True)
    assert not memory_leak(test)

    # Key function fails
    def test():
        with pytest_raises(TypeError):
            list(merge([Test(2), (Test(2), Test(0))],
                       [(Test(1), Test(2)), (Test(1), Test(3))],
                       key=operator.itemgetter(0)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            list(merge([(Test(2), Test(0)), Test(2)],
                       [(Test(1), Test(2)), (Test(1), Test(3))],
                       key=operator.itemgetter(0)))
    assert not memory_leak(test)

    # Comparison fails
    if not iteration_utilities.PY2:
        def test():
            with pytest_raises(TypeError):
                list(merge([Test('a'), Test('b')], [Test(2), Test(3)]))
        assert not memory_leak(test)

        def test():
            with pytest_raises(TypeError):
                list(merge([Test(1), Test('b')], [Test(2), Test(3)]))
        assert not memory_leak(test)


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
    assert not memory_leak(test)

    # no fillvalue + truncate
    def test():
        list(grouper([Test(1)], 3))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2)], 3))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3)], 3))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4)], 3))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5)], 3))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5), Test(6)],
                     3))
    assert not memory_leak(test)

    # with fillvalue
    def test():
        list(grouper([Test(1)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5), Test(6)], 3,
                     fillvalue=Test(0)))
    assert not memory_leak(test)

    # with truncate
    def test():
        list(grouper([Test(1)], 3,
                     truncate=True))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2)], 3,
                     truncate=True))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3)], 3,
                     truncate=True))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4)], 3,
                     truncate=True))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5)], 3,
                     truncate=True))
    assert not memory_leak(test)

    def test():
        list(grouper([Test(1), Test(2), Test(3), Test(4), Test(5), Test(6)], 3,
                     truncate=True))
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(TypeError):  # fillvalue + truncate is forbidden
            grouper([Test(1), Test(2), Test(3)], 2,
                    fillvalue=Test(0), truncate=True)
    assert not memory_leak(test)

    def test():
        with pytest_raises(ValueError):  # n must be > 0
            grouper([Test(1), Test(2), Test(3)], 0)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # iterable must be iterable
            grouper(Test(1), 2)
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        ilen(range(10))
    assert not memory_leak(test)

    def test():
        ilen([Test(1), Test(2), Test(3), Test(4), Test(5)])
    assert not memory_leak(test)

    def test():
        generator = (i for i in [Test(1), Test(2), Test(3), Test(4), Test(5)])
        ilen(generator) == 5
        ilen(generator) == 0
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            ilen(100)
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        list(intersperse([Test(1)], Test(0)))
    assert not memory_leak(test)

    def test():
        list(intersperse([Test(1), Test(2)], Test(0)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            intersperse(Test(100), Test(0))
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        complement(lambda x: x is True)(False)
    assert not memory_leak(test)

    def test():
        complement(lambda x: x is False)(True)
    assert not memory_leak(test)

    def test():
        complement(lambda x: x is False)(False)
    assert not memory_leak(test)

    def test():
        complement(iteration_utilities.is_None)(None)
    assert not memory_leak(test)

    def test():
        complement(iteration_utilities.is_None)(False)
    assert not memory_leak(test)

    def test():
        complement(iteration_utilities.is_None)(True)
    assert not memory_leak(test)


def test_compose():
    compose = iteration_utilities.compose

    double_increment = compose(lambda x: x*2, lambda x: x+1)
    assert double_increment(10) == 21
    assert double_increment(2.) == 5

    with pytest.raises(TypeError):  # at least one func must be present
        compose()

    with pytest.raises(TypeError):  # kwarg not accepted
        compose(lambda x: x+1, invalidkwarg=lambda x: x*2)

    with pytest.raises(TypeError):  # func fails
        compose(lambda x: x+1)('a')

    with pytest.raises(TypeError):  # second func fails
        compose(lambda x: x*2, lambda x: x+1)('a')


def test_compose_memoryleak():
    compose = iteration_utilities.compose

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __add__(self, other):
            return self.__class__(self.value + other)

        def __mul__(self, other):
            return self.__class__(self.value + other)

    def test():
        compose(lambda x: x*2, lambda x: x+1)(Test(10))
        compose(lambda x: x*2, lambda x: x+1)(Test(2))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # at least one func must be present
            compose()
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # kwarg not accepted
            compose(lambda x: x+1, invalidkwarg=lambda x: x*2)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # func fails
            compose(lambda x: x+1)(Test('a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # second func fails
            compose(lambda x: x*2, lambda x: x+1)(Test('a'))
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        one('a')
    assert not memory_leak(test)

    def test():
        one({Test(0): 10})
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            one(Test(0))
    assert not memory_leak(test)

    def test():
        with pytest_raises(ValueError):  # empty iterable
            one([])
    assert not memory_leak(test)

    def test():
        with pytest_raises(ValueError):  # more than 1 element
            one([1, 2])
    assert not memory_leak(test)


def test_nth():
    nth = iteration_utilities.nth

    assert nth([1, 2, 3], 1) == 2
    assert nth(range(10), 2) == 2

    # With pred
    assert nth([0, 1, 2], 1, pred=bool) == 2
    assert nth([0, 1, 2], 1, pred=None) == 2
    assert nth([0]*100 + [1], 0, pred=bool) == 1
    assert nth([[1], [1, 2], [1, 2]], 1, pred=lambda x: len(x) > 1) == [1, 2]

    # pred with truthy/retpred
    assert nth([0, 1, 2, 3, 0], 1, pred=bool, truthy=False) == 0
    assert nth([0, 1, 2, 3, 0], 1, pred=bool, truthy=False,
                retpred=True) == False
    assert nth([0, 1, 2, 3, 0], 1, pred=lambda x: x**2, truthy=False) == 0
    assert nth([0, 1, 2, 3, 0], 1,
               pred=lambda x: x**2, truthy=False, retpred=True) == 0
    assert nth([0, 1, 2, 3], 2, pred=bool) == 3
    assert nth([0, 1, 2, 3], 2, pred=bool, retpred=True) == True
    assert nth([0, 2, 3, 4], 2, pred=lambda x: x**2) == 4
    assert nth([0, 2, 3, 4], 2, pred=lambda x: x**2, retpred=True) == 16

    # With default
    assert nth([], 2, default=None) is None
    assert nth([0, 0, 0], 1, default=None, pred=bool) is None

    # failures
    with pytest.raises(TypeError):
        nth(100, 10)

    with pytest.raises(IndexError):
        nth([], 10)

    with pytest.raises(IndexError):
        nth([0], 1, pred=bool)

    with pytest.raises(TypeError):
        nth(['a', 'b'], 1, pred=abs)


def test_nth_memoryleak():
    nth = iteration_utilities.nth

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
        nth([Test(1), Test(2), Test(3)], 1)
    assert not memory_leak(test)

    # With pred
    def test():
        nth([Test(0), Test(1), Test(2)], 1, pred=bool)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(1), Test(2)], 1, pred=None)
    assert not memory_leak(test)

    def test():
        nth([Test(0)]*100 + [Test(1)]*2, 1, pred=bool)
    assert not memory_leak(test)

    def test():
        nth([[Test(0)], [Test(1), Test(2)]]*2, 1, pred=lambda x: len(x) > 1)
    assert not memory_leak(test)

    # pred with truthy/retpred
    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 1,
            pred=bool, truthy=False)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 1,
            pred=bool, truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 1,
            pred=lambda x: x**Test(2), truthy=False)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 0,
            pred=lambda x: x**Test(2), truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1, pred=bool)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1, pred=bool, retpred=True)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1, pred=lambda x: x**Test(2))
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1,
            pred=lambda x: x**Test(2), retpred=True)
    assert not memory_leak(test)

    # With default
    def test():
        nth([], 2, default=None) is None
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(0), Test(0)], 1, default=None, pred=bool) is None
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(IndexError):
            nth([], 10)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            nth(Test(100), 10)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            nth([Test('a'), Test('b')], 1, pred=lambda x: abs(x.value))
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        quantify([Test(0), Test(0)])
    assert not memory_leak(test)

    def test():
        quantify([Test(0), Test(0), Test(1)])
    assert not memory_leak(test)

    def test():
        quantify([Test(0), Test(0), Test(1), Test(1)], None)
    assert not memory_leak(test)

    def test():
        quantify([], iteration_utilities.return_first_positional_argument)
    assert not memory_leak(test)

    def test():
        quantify([Test(1), Test(2), Test(3)], lambda x: x > Test(2))
    assert not memory_leak(test)

    def test():
        quantify([Test(1), Test(2), Test(3)], lambda x: x < Test(3))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            quantify(Test(1))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            quantify([Test(1)], Test(1))
    assert not memory_leak(test)


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
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test(1), Test(1), Test(2), Test(3), Test(3)]))
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test(1), Test(-1), Test(1),
                              Test(2), Test(-2), Test(2)]))
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test(1), Test(-1), Test(1),
                              Test(2), Test(-2), Test(2)],
                             key=lambda x: abs(x.value)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # not iterable
            unique_justseen(Test(1))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # function call fails
            list(unique_justseen([Test(1), Test(2), Test(3)],
                                 key=lambda x: x + 'a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # objects do not support eq or ne
            list(unique_justseen([Test2(1), Test2(2)]))
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test3(1), Test3(1)]))
    assert not memory_leak(test)


def test_groupby2():
    groupby2 = iteration_utilities.groupby2

    assert groupby2([], key=lambda x: x) == {}
    assert groupby2(['a', 'ab', 'abc'],
                    key=operator.itemgetter(0)) == {'a': ['a', 'ab', 'abc']}
    assert groupby2(['a', 'ba', 'ab', 'abc', 'b'],
                    key=operator.itemgetter(0)) == {'a': ['a', 'ab', 'abc'],
                                                    'b': ['ba', 'b']}
    assert groupby2(['a', 'ba', 'ab', 'abc', 'b'],
                    key=operator.itemgetter(0),
                    keepkey=len) == {'a': [1, 2, 3], 'b': [2, 1]}

    with pytest.raises(TypeError):  # not iterable
        groupby2(1, key=len)

    with pytest.raises(TypeError):  # key func fails
        groupby2([1, 2, 3], key=lambda x: x + 'a')

    with pytest.raises(TypeError):  # keepkey func fails
        groupby2([1, 2, 3], key=lambda x: x, keepkey=lambda x: x + 'a')

    with pytest.raises(TypeError):  # unhashable
        groupby2([{'a': 10}], key=lambda x: x)


def test_groupby2_memoryleak():
    groupby2 = iteration_utilities.groupby2

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            return self.value == other.value

        def __hash__(self):
            return hash(self.value)

    def test():
        groupby2([], key=lambda x: x)
    assert not memory_leak(test)

    def test():
        groupby2([Test('a'), Test('ab'), Test('abc')],
                 key=lambda x: Test(x.value[0]))
    assert not memory_leak(test)

    def test():
        groupby2([Test('a'), Test('ba'), Test('ab'), Test('abc'), Test('b')],
                 key=lambda x: Test(x.value[0]))
    assert not memory_leak(test)

    def test():
        groupby2([Test('a'), Test('ba'), Test('ab'), Test('abc'), Test('b')],
                 key=lambda x: Test(x.value[0]),
                 keepkey=lambda x: Test(len(x.value)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # not iterable
            groupby2(Test(1), key=lambda x: Test(len(x.value)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # key func fails
            groupby2([Test(1), Test(2), Test(3)],
                     key=lambda x: Test(x.value + 'a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # keepkey func fails
            groupby2([Test(1), Test(2), Test(3)],
                     key=lambda x: x, keepkey=lambda x: Test(x.value + 'a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # unhashable
            groupby2([{Test('a'): Test(10)}], key=lambda x: x)
    assert not memory_leak(test)


def test_alldistinct():
    all_distinct = iteration_utilities.all_distinct

    assert all_distinct([1, 2, 3])
    assert not all_distinct([1, 1, 1])

    assert all_distinct([{'a': 1}, {'a': 2}])
    assert not all_distinct([{'a': 1}, {'a': 1}])

    with pytest.raises(TypeError):  # iterable is not iterable
        all_distinct(1)


def test_alldistinct_memoryleak():
    all_distinct = iteration_utilities.all_distinct

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            return self.value == other.value

        def __hash__(self):
            return hash(self.value)

    def test():
        all_distinct([Test(1), Test(2), Test(3)])
    assert not memory_leak(test)

    def test():
        all_distinct([Test(1), Test(1), Test(1)])
    assert not memory_leak(test)

    def test():
        all_distinct([{Test('a'): Test(1)}, {Test('a'): Test(2)}])
    assert not memory_leak(test)

    def test():
        all_distinct([{Test('a'): Test(1)}, {Test('a'): Test(1)}])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # iterable is not iterable
            all_distinct(Test(1))
    assert not memory_leak(test)


def test_all_equal():
    all_equal = iteration_utilities.all_equal

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    assert all_equal([])
    assert all_equal([1, 1, 1])
    assert not all_equal([1, 1, 2])

    with pytest.raises(TypeError):  # not iterable
        all_equal(1)

    if not iteration_utilities.PY2:
        with pytest.raises(TypeError):  # comparison fail
            all_equal([Test(1), Test('a')])


def test_all_equal_memoryleak():
    all_equal = iteration_utilities.all_equal

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    def test():
        all_equal([])
    assert not memory_leak(test)

    def test():
        all_equal([Test(1), Test(1), Test(1)])
    assert not memory_leak(test)

    def test():
        all_equal([Test(1), Test(1), Test(2)])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # not iterable
            all_equal(Test(1))
    assert not memory_leak(test)

    if not iteration_utilities.PY2:
        def test():
            with pytest_raises(TypeError):  # comparison fail
                all_equal([Test(1), Test('a')])
        assert not memory_leak(test)


def test_split():
    split = iteration_utilities.split

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    assert list(split([], lambda x: False)) == []

    assert list(split([1, 2, 3], lambda x: x == 2)) == [[1], [3]]
    assert list(split([1, 2, 3], lambda x: x == 3)) == [[1, 2]]

    # keep
    assert list(split([1, 2, 3], lambda x: x == 2,
                      keep=True)) == [[1], [2], [3]]
    assert list(split([1, 2, 3], lambda x: x == 3,
                      keep=True)) == [[1, 2], [3]]

    # maxsplit
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=1)) == [[1], [3, 4, 5]]
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=2)) == [[1], [3], [5]]

    # equality
    assert list(split([1, 2, 3, 2, 5], 2,
                      eq=True)) == [[1], [3], [5]]

    # failures
    with pytest.raises(TypeError):  # not iterable
        split(1, lambda x: False)
    with pytest.raises(TypeError):  # func fails
        list(split([1, 2, 3], lambda x: x + 'a'))
    with pytest.raises(TypeError):  # cmp fails
        list(split([Test(1), Test(2), Test(3)], Test('a'), eq=True))


def test_split_memoryleak():
    split = iteration_utilities.split

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    def test():
        list(split([], lambda x: False)) == []
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 2))
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 3))
    assert not memory_leak(test)

    # keep
    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 2,
                   keep=True))
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 3,
                   keep=True))
    assert not memory_leak(test)

    # maxsplit
    def test():
        list(split([Test(1), Test(2), Test(3), Test(4), Test(5)],
                   lambda x: x.value % 2 == 0, maxsplit=1))
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3), Test(4), Test(5)],
                   lambda x: x.value % 2 == 0, maxsplit=2))
    assert not memory_leak(test)

    # equality
    def test():
        list(split([Test(1), Test(2), Test(3), Test(2), Test(5)], Test(2),
                   eq=True))
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(TypeError):  # not iterable
            split(Test(1), lambda x: False)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # func fails
            list(split([Test(1), Test(2), Test(3)],
                       lambda x: Test(x.value + 'a')))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # cmp fails
            list(split([Test(1), Test(2), Test(3)], Test('a'), eq=True))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='Python 2 does not support this way of pickling.')
def test_cfuncs_pickle():
    import pickle

    applyfunc = iteration_utilities.applyfunc
    grouper = iteration_utilities.grouper
    intersperse = iteration_utilities.intersperse
    merge = iteration_utilities.merge
    unique_everseen = iteration_utilities.unique_everseen
    unique_justseen = iteration_utilities.unique_justseen
    successive = iteration_utilities.successive
    roundrobin = iteration_utilities.roundrobin
    complement = iteration_utilities.complement
    compose = iteration_utilities.compose

    # IMPORTANT: methoddescriptors like "str.lower" as key functions can not
    #            be pickled before python 3.4

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

    # ----- Compose

    cmp = compose(iteration_utilities.square, iteration_utilities.one_over)
    x = pickle.dumps(cmp)
    assert pickle.loads(x)(10) == 1/100
    assert pickle.loads(x)(2) == 1/4


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

    assert iteration_utilities.is_even(2)
    assert not iteration_utilities.is_even(1)

    assert iteration_utilities.is_odd(1)
    assert not iteration_utilities.is_odd(2)

    assert not iteration_utilities.is_iterable(1)
    assert iteration_utilities.is_iterable([1])
