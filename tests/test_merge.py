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
from helper_leak import memory_leak_decorator
from helper_cls import T, toT, failingTIterator


merge = iteration_utilities.merge


@memory_leak_decorator()
def test_merge_empty1():
    assert list(merge()) == []


@memory_leak_decorator()
def test_merge_empty2():
    assert list(merge([])) == []


@memory_leak_decorator()
def test_merge_empty3():
    assert list(merge([], (), {})) == []


@memory_leak_decorator()
def test_merge_empty4():
    # generator, one ends immediatly the other only after two items
    assert list(merge((i for i in []),
                      (i for i in (T(1), T(2))),
                      {})) == [T(1), T(2)]


@memory_leak_decorator()
def test_merge_empty5():
    # generator
    assert list(merge((), {}, (i for i in []))) == []


@memory_leak_decorator()
def test_merge_normal1():
    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)]]):
        assert list(merge(*seq)) == toT([1, 2, 3])


@memory_leak_decorator()
def test_merge_normal2():
    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)], []]):
        assert list(merge(*seq)) == toT([1, 2, 3])


@memory_leak_decorator()
def test_merge_normal3():
    for seq in itertools.permutations([[T(1), T(3)], [T(2)], [T(4)]]):
        assert list(merge(*seq)) == toT([1, 2, 3, 4])


@memory_leak_decorator()
def test_merge_normal4():
    for seq in itertools.permutations([[T(1), T(3)], [T(0), T(2)], [T(4)]]):
        assert list(merge(*seq)) == toT([0, 1, 2, 3, 4])


@memory_leak_decorator()
def test_merge_normal5():
    perms = itertools.permutations([toT(range(5)),
                                    toT(range(3)),
                                    toT(range(4, 7))])
    for seq in perms:
        assert list(merge(*seq)) == toT([0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6])


@memory_leak_decorator()
def test_merge_stable1():
    # Stability tests (no use of T on purpose!)
    it = merge([1], [1.])
    item1 = next(it)
    assert isinstance(item1, int)
    item2 = next(it)
    assert isinstance(item2, float)


@memory_leak_decorator()
def test_merge_key1():
    # Key function tests
    seq = ([(T(1), T(0)), (T(2), T(0))], [(T(1), T(-1)), (T(2), T(-1))])
    assert (list(merge(*seq, key=operator.itemgetter(0))) ==
            [(T(1), T(0)), (T(1), T(-1)), (T(2), T(0)), (T(2), T(-1))])


@memory_leak_decorator()
def test_merge_reverse1():
    # Reverse test
    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)]]):
        assert list(merge(*seq, reverse=True)) == toT([3, 2, 1])


@memory_leak_decorator()
def test_merge_reverse2():
    for seq in itertools.permutations([[T(1)], [T(2)], [T(3)], []]):
        assert list(merge(*seq, reverse=True)) == toT([3, 2, 1])


@memory_leak_decorator()
def test_merge_reverse3():
    for seq in itertools.permutations([[T(3), T(1)], [T(2)], [T(4)]]):
        assert list(merge(*seq, reverse=True)) == toT([4, 3, 2, 1])


@memory_leak_decorator()
def test_merge_reverse4():
    for seq in itertools.permutations([[T(3), T(1)], [T(2), T(0)], [T(4)]]):
        assert list(merge(*seq, reverse=True)) == toT([4, 3, 2, 1, 0])


@memory_leak_decorator()
def test_merge_keyreverse1():
    # Key+reverse function tests
    seq = ([(T(2), T(0)), (T(1), T(0))], [(T(2), T(-1)), (T(1), T(-1))])
    assert (list(merge(*seq, reverse=True, key=operator.itemgetter(0))) ==
            [(T(2), T(0)), (T(2), T(-1)), (T(1), T(0)), (T(1), T(-1))])


@memory_leak_decorator(collect=True)
def test_merge_failure1():
    # One iterable is not iterable
    with pytest.raises(TypeError):
        merge(T(10))


@memory_leak_decorator(collect=True)
def test_merge_failure2():
    # One iterable is not iterable
    with pytest.raises(TypeError):
        merge([T(10), T(20)], T(10))


@memory_leak_decorator(collect=True)
def test_merge_failure3():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([T(10), T(20)], [T(20)], reverse=True, key=abs, wrongkwd=True)


@memory_leak_decorator(collect=True)
def test_merge_failure4():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([T(10), T(20)], [T(20), T(30)], reverse=True, wrongkwd=True)


@memory_leak_decorator(collect=True)
def test_merge_failure5():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([T(10), T(20)], [T(20), T(30)], key=abs, wrongkwd=True)


@memory_leak_decorator(collect=True)
def test_merge_failure6():
    # Unexpected keyword argument
    with pytest.raises(TypeError):
        merge([T(10), T(20)], [T(20), T(30)], wrongkwd=True)


@memory_leak_decorator(collect=True)
def test_merge_failure7():
    # Key function fails
    with pytest.raises(TypeError):
        list(merge([T(2), (T(2), T(0))], [(T(1), T(2)), (T(1), T(3))],
                   key=operator.itemgetter(0)))


@memory_leak_decorator(collect=True)
def test_merge_failure8():
    # Key function fails
    with pytest.raises(TypeError):
        list(merge([(T(2), T(0)), T(2)], [(T(1), T(2)), (T(1), T(3))],
                   key=operator.itemgetter(0)))


@memory_leak_decorator(collect=True)
def test_merge_failure9():
    # comparison fails
    with pytest.raises(TypeError):
        list(merge([T('a'), T('b')], [T(2), T(3)]))


@memory_leak_decorator(collect=True)
def test_merge_failure10():
    # comparison fails
    with pytest.raises(TypeError):
        list(merge([T(1), T('b')], [T(2), T(3)]))


@memory_leak_decorator(collect=True)
def test_merge_failure11():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(merge(failingTIterator()))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_merge_failure12():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(merge([T(1), T(1)], failingTIterator()))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_merge_failure13():
    # Test that a failing iterator doesn't raise a SystemError
    mge = merge(failingTIterator(offset=2, repeats=10))
    assert next(mge) == T(1)
    with pytest.raises(TypeError) as exc:
        next(mge)
    assert 'eq expected 2 arguments, got 1' in str(exc)


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle1():
    mge = merge([T(0)], [T(1), T(2)], [T(2)])
    assert next(mge) == T(0)
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([1, 2, 2])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle2():
    mge = merge([T(1), T(2)], [T(0)], [T(-2)], key=abs)
    assert next(mge) == T(0)
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([1, 2, -2])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle3():
    mge = merge([T(2), T(1)], [T(0)], [T(3)], reverse=True)
    assert next(mge) == T(3)
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([2, 1, 0])


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator()
def test_merge_lengthhint1():
    it = merge([0], [1, 2, 3], [1])
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
