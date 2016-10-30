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
from helper_cls import T


merge = iteration_utilities.merge


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
    for seq in itertools.permutations([[1, 3], [2], [4]]):
        assert list(merge(*seq)) == [1, 2, 3, 4]

    for seq in itertools.permutations([[T(1), T(3)],
                                       [T(2)], [T(4)]]):
        def test():
            list(merge(*seq))
        assert not memory_leak(test)


def test_merge_normal4():
    for seq in itertools.permutations([[1, 3], [0, 2], [4]]):
        assert list(merge(*seq)) == [0, 1, 2, 3, 4]

    for seq in itertools.permutations([[T(1), T(3)],
                                       [T(0), T(2)],
                                       [T(4)]]):
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
    # No memory leak test on purpose since T throws an Excepetion if the types
    # differ.


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
    for seq in itertools.permutations([[3, 1], [2], [4]]):
        assert list(merge(*seq, reverse=True)) == [4, 3, 2, 1]

    for seq in itertools.permutations([[T(3), T(1)],
                                       [T(2)],
                                       [T(4)]]):
        def test():
            list(merge(*seq, reverse=True))
        assert not memory_leak(test)


def test_merge_reverse4():
    for seq in itertools.permutations([[3, 1], [2, 0], [4]]):
        assert list(merge(*seq, reverse=True)) == [4, 3, 2, 1, 0]

    for seq in itertools.permutations([[T(3), T(1)],
                                       [T(2), T(0)],
                                       [T(4)]]):
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
        with pytest.raises(TypeError):
            merge(T(10))
    assert not memory_leak(test)


def test_merge_failure2():
    # One iterable is not iterable
    with pytest.raises(TypeError):
        merge([10, 20], 10)

    def test():
        with pytest.raises(TypeError):
            merge([T(10), T(20)], T(10))
    assert not memory_leak(test)


def test_merge_failure3():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], reverse=True, key=abs, wrongkwd=True)

    def test():
        with pytest.raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  reverse=True, key=abs, wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure4():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], reverse=True, wrongkwd=True)

    def test():
        with pytest.raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  reverse=True, wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure5():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], key=abs, wrongkwd=True)

    def test():
        with pytest.raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  key=abs, wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure6():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([10, 20], [20, 30], wrongkwd=True)

    def test():
        with pytest.raises(TypeError):
            merge([T(10), T(20)], [T(20), T(30)],
                  wrongkwd=True)
    assert not memory_leak(test)


def test_merge_failure7():
    # Key function fails
    with pytest.raises(TypeError):
        list(merge([2, (2, 0)], [(1, 2), (1, 3)], key=operator.itemgetter(0)))

    def test():
        with pytest.raises(TypeError):
            list(merge([T(2), (T(2), T(0))],
                       [(T(1), T(2)), (T(1), T(3))],
                       key=operator.itemgetter(0)))
    assert not memory_leak(test)


def test_merge_failure8():
    # Key function fails
    with pytest.raises(TypeError):
        list(merge([(2, 0), 2], [(1, 2), (1, 3)], key=operator.itemgetter(0)))

    def test():
        with pytest.raises(TypeError):
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
        with pytest.raises(TypeError):
            list(merge([T('a'), T('b')], [T(2), T(3)]))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2, reason='cmp works on Python 2')
def test_merge_failure10():
    # comparison fails
    with pytest.raises(TypeError):
        list(merge([1, 'b'], [2, 3]))

    def test():
        with pytest.raises(TypeError):
            list(merge([T(1), T('b')], [T(2), T(3)]))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_merge_pickle1():
    mge = merge([0], [1, 2], [2])
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, 2]

    def test():
        mge = merge([T(0)], [T(1), T(2)], [T(2)])
        next(mge)
        x = pickle.dumps(mge)
        list(pickle.loads(x))
    memory_leak(test)
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_merge_pickle2():
    mge = merge([1, 2], [0], [-2], key=abs)
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, -2]

    def test():
        mge = merge([T(1), T(2)], [T(0)], [T(-2)], key=abs)
        next(mge)
        x = pickle.dumps(mge)
        list(pickle.loads(x))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='pickle does not work on Python 2')
def test_merge_pickle3():
    mge = merge([2, 1], [0], [3], reverse=True)
    assert next(mge) == 3
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [2, 1, 0]

    def test():
        mge = merge([T(2), T(1)], [T(0)], [T(3)], reverse=True)
        next(mge)
        x = pickle.dumps(mge)
        list(pickle.loads(x))
    assert not memory_leak(test)
