# Built-ins
from __future__ import absolute_import, division, print_function
import itertools
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


merge = iteration_utilities.merge


class T(object):
    def __init__(self, value):
        self.value = value

    def __gt__(self, other):
        return self.value > other.value

    def __lt__(self, other):
        return self.value < other.value


def test_merge_empty1():
    assert list(merge()) == []

    def test():
        list(merge())
    assert not memory_leak(test)


def test_merge_empty2():
    assert list(merge([])) == []

    def test():
        list(merge([]))
    assert not memory_leak(test)


def test_merge_empty3():
    assert list(merge([], (), {})) == []

    def test():
        list(merge([], (), {}))
    assert not memory_leak(test)


def test_merge_normal1():
    for seq in itertools.permutations([[1], [2], [3]]):
        assert list(merge(*seq)) == [1, 2, 3]

    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)


def test_merge_normal2():
    for seq in itertools.permutations([[1], [2], [3], []]):
        assert list(merge(*seq)) == [1, 2, 3]

    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)], []]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)


def test_merge_normal3():
    for seq in itertools.permutations([[1, 2.5], [2], [3]]):
        assert list(merge(*seq)) == [1, 2, 2.5, 3]

    for seq in itertools.permutations([[T(1), T(2.5)],
                                       [T(2)], [T(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)


def test_merge_normal4():
    for seq in itertools.permutations([[1, 2.5], [0.5, 2], [3]]):
        assert list(merge(*seq)) == [0.5, 1, 2, 2.5, 3]

    for seq in itertools.permutations([[T(1), T(2.5)],
                                       [T(0.5), T(2)],
                                       [T(3)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)


def test_merge_normal5():
    for seq in itertools.permutations([range(5), range(3), range(4, 7)]):
        assert list(merge(*seq)) == [0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6]
    # TODO: Missing memoryleak

def test_merge_stable1():
    # Stability tests
    it = merge([1], [1.])
    item1 = next(it)
    assert isinstance(item1, int)
    item2 = next(it)
    assert isinstance(item2, float)

    def test():
        list(merge([T(1)], [T(1.)]))
    assert not memory_leak(test)


def test_merge_key1():
    # Key function tests
    seq = ([(1, 0), (2, 0)], [(1, -1), (2, -1)])
    assert (list(merge(*seq, key=operator.itemgetter(0))) ==
            [(1, 0), (1, -1), (2, 0), (2, -1)])

    def test():
        seq = ([(T(1), T(0)), (T(2), T(0))],
               [(T(1), T(-1)), (T(2), T(-1))])
        list(merge(*seq, key=operator.itemgetter(0)))
    assert not memory_leak(test)


def test_merge_reverse1():
    # Reverse test
    for seq in itertools.permutations([[1], [2], [3]]):
        assert list(merge(*seq, reverse=True)) == [3, 2, 1]

    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)


def test_merge_reverse2():
    for seq in itertools.permutations([[1], [2], [3], []]):
        assert list(merge(*seq, reverse=True)) == [3, 2, 1]

    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)], []]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)


def test_merge_reverse3():
    for seq in itertools.permutations([[2.5, 1], [2], [3]]):
        assert list(merge(*seq, reverse=True)) == [3, 2.5, 2, 1]

    for seq in itertools.permutations([[T(2.5), T(1)],
                                       [T(2)],
                                       [T(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)


def test_merge_reverse4():
    for seq in itertools.permutations([[2.5, 1], [2, 0.5], [3]]):
        assert list(merge(*seq, reverse=True)) == [3, 2.5, 2, 1, 0.5]

    for seq in itertools.permutations([[T(2.5), T(1)],
                                       [T(2), T(0.5)],
                                       [T(3)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)


def test_merge_keyreverse1():
    # Key+reverse function tests
    seq = ([(2, 0), (1, 0)], [(2, -1), (1, -1)])
    assert (list(merge(*seq, reverse=True, key=operator.itemgetter(0))) ==
            [(2, 0), (2, -1), (1, 0), (1, -1)])

    def test():
        seq = ([(T(2), T(0)), (T(1), T(0))],
               [(T(2), T(-1)), (T(1), T(-1))])
        list(merge(*seq, reverse=True, key=operator.itemgetter(0)))
    assert not memory_leak(test)


def test_merge_failure1():
    # One iterable is not iterable
    with pytest.raises(TypeError):
        merge(10)

    def test():
        with pytest_raises(TypeError):
            merge(T(10))
    assert not memory_leak(test)


def test_merge_failure2():
    # One iterable is not iterable
    with pytest.raises(TypeError):
        merge([10, 20], 10)

    def test():
        with pytest_raises(TypeError):
            merge([T(10), T(20)], T(10))
    assert not memory_leak(test)


def test_merge_failure3():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], reverse=True, key=abs, wrongkwd=True)

    def test():
        with pytest_raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  reverse=True, key=abs, wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure4():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], reverse=True, wrongkwd=True)

    def test():
        with pytest_raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  reverse=True, wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure5():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], key=abs, wrongkwd=True)

    def test():
        with pytest_raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  key=abs, wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure6():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], wrongkwd=True)

    def test():
        with pytest_raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure7():
    # Key function fails
    with pytest.raises(TypeError):
        list(merge([2, (2, 0)], [(1, 2), (1, 3)], key=operator.itemgetter(0)))

    def test():
        with pytest_raises(TypeError):
            list(merge([T(2), (T(2), T(0))],
                       [(T(1), T(2)), (T(1), T(3))],
                       key=operator.itemgetter(0)))
    assert not memory_leak(test)


def test_merge_failure8():
    # Key function fails
    with pytest.raises(TypeError):
        list(merge([(2, 0), 2], [(1, 2), (1, 3)], key=operator.itemgetter(0)))

    def test():
        with pytest_raises(TypeError):
            list(merge([(T(2), T(0)), T(2)],
                       [(T(1), T(2)), (T(1), T(3))],
                       key=operator.itemgetter(0)))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_merge_failure9():
    # comparison fails
    with pytest.raises(TypeError):
        list(merge(['a', 'b'], [2, 3]))

    def test():
        with pytest_raises(TypeError):
            list(merge([T('a'), T('b')], [T(2), T(3)]))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_merge_failure10():
    # comparison fails
    with pytest.raises(TypeError):
        list(merge([1, 'b'], [2, 3]))

    def test():
        with pytest_raises(TypeError):
            list(merge([T(1), T('b')], [T(2), T(3)]))
    assert not memory_leak(test)
