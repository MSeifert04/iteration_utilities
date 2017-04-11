# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function
import itertools
import operator
import pickle
import sys

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
import helper_funcs as _hf
from helper_cls import T, toT
from helper_leak import memory_leak_decorator


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
def test_merge_normal6():
    # key=None is identical to no key
    assert list(merge([T(1)], [T(2)], key=None)) == [T(1), T(2)]


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


@memory_leak_decorator()
def test_merge_attributes1():
    # Key+reverse function tests
    it = merge(toT(range(5)), toT(range(5)))
    assert not it.reverse
    assert not it.key


@memory_leak_decorator(collect=True)
def test_merge_failure1():
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        merge(_hf.FailIter())
    assert _hf.FailIter.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_merge_failure2():
    with pytest.raises(_hf.FailIter.EXC_TYP) as exc:
        merge([T(10), T(20)], _hf.FailIter())
    assert _hf.FailIter.EXC_MSG in str(exc)


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
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        next(merge(_hf.FailNext()))
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_merge_failure12():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        next(merge([T(1), T(1)], _hf.FailNext()))
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_merge_failure13():
    # Test that a failing iterator doesn't raise a SystemError
    mge = merge(_hf.FailNext(offset=2, repeats=10))
    assert next(mge) == T(1)
    with pytest.raises(_hf.FailNext.EXC_TYP) as exc:
        next(mge)
    assert _hf.FailNext.EXC_MSG in str(exc)


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate1():
    # __setstate__ with numactive < 0 fails
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(ValueError):
        mg.__setstate__((None, 0, None, -1))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate2():
    # __setstate__ with numactive > len(iterators) fails
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(ValueError):
        mg.__setstate__((None, 0, None, 3))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate3():
    # __setstate__ with type(current) != tuple
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(TypeError):
        mg.__setstate__((None, 0, [], 2))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate4():
    # __setstate__ with len(current) != len(iteratortuple)
    from iteration_utilities import ItemIdxKey as IIK
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(ValueError):
        mg.__setstate__((None, 0, (IIK(-2, 0), ), 2))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate5():
    # __setstate__ with current containing non-itemidxkey instances
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(TypeError):
        mg.__setstate__((None, 0, (1, 2), 2))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate6():
    # __setstate__ with current containing itemidxkey with key even though
    # no key function is given
    from iteration_utilities import ItemIdxKey as IIK
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(TypeError):
        mg.__setstate__((None, 0, (IIK(-2, 0), IIK(-1, 1, 2)), 2))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate7():
    # __setstate__ with current containing itemidxkey without key even though
    # a key function is given
    from iteration_utilities import ItemIdxKey as IIK
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(TypeError):
        mg.__setstate__((lambda x: x, 0, (IIK(-2, 0), IIK(-1, 1, 2)), 2))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate8():
    # __setstate__ with current containing itemidxkey with index that is out
    # of bounds
    from iteration_utilities import ItemIdxKey as IIK
    mg = merge(toT(range(5)), toT(range(3, 10, 2)))
    with pytest.raises(ValueError):
        mg.__setstate__((None, 0, (IIK(-2, 0), IIK(-1, 20)), 2))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate9():
    _hf.iterator_setstate_list_fail(merge(toT(range(5)), toT(range(3, 10, 2))))


@memory_leak_decorator(collect=True)
def test_merge_failure_setstate10():
    _hf.iterator_setstate_empty_fail(
        merge(toT(range(5)), toT(range(3, 10, 2))))


@memory_leak_decorator(collect=True)
def test_merge_copy1():
    _hf.iterator_copy(merge([T(0)], [T(1), T(2)], [T(2)]))


@memory_leak_decorator(collect=True)
def test_merge_reduce1():
    # We shouldn't be able to alter the ItemIdxKey instances in the "current"
    # tuple that is returned from reduce. We could remove or add the key
    # attribute which would break the comparisons
    df = merge([T(1), T(2), T(3)], [T(1), T(2), T(3)])
    next(df)
    # add a key even though we have no key function
    df.__reduce__()[2][2][0].key = 10
    list(df)


@memory_leak_decorator(collect=True)
def test_merge_setstate1():
    # We shouldn't be able to alter the ItemIdxKey instances in the "current"
    # tuple that is used to setstate. We could remove or add the key
    # attribute which would break the comparisons
    df = merge([T(1), T(2), T(3)], [T(1), T(2), T(3)])
    next(df)
    # we roundtrip the state but keep a reference so we can later add a key
    # even though we have no key function
    state = df.__reduce__()[2]
    df.__setstate__(state)
    state[2][0].key = 10
    list(df)


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle1():
    # normal
    mge = merge([T(0)], [T(1), T(2)], [T(2)])
    assert next(mge) == T(0)
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([1, 2, 2])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle2():
    # with key
    mge = merge([T(1), T(2)], [T(0)], [T(-2)], key=abs)
    assert next(mge) == T(0)
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([1, 2, -2])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle3():
    # reverse
    mge = merge([T(2), T(1)], [T(0)], [T(3)], reverse=True)
    assert next(mge) == T(3)
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([2, 1, 0])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle4():
    # pickle unstarted merge instance
    mge = merge([T(0)], [T(1), T(2)], [T(2)])
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([0, 1, 2, 2])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_merge_pickle5():
    # pickle merge with no exhausted iterable
    mge = merge([T(0), T(1)], [T(1), T(2)])
    assert next(mge) == T(0)
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == toT([1, 1, 2])


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


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_merge_lengthhint_failure1():
    f_it = _hf.FailLengthHint(toT([1, 2, 3]))
    it = merge(f_it)
    with pytest.raises(_hf.FailLengthHint.EXC_TYP) as exc:
        operator.length_hint(it)
    assert _hf.FailLengthHint.EXC_MSG in str(exc)

    with pytest.raises(_hf.FailLengthHint.EXC_TYP) as exc:
        list(it)
    assert _hf.FailLengthHint.EXC_MSG in str(exc)


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_merge_lengthhint_failure2():
    # This is the easy way to overflow the length_hint: If the iterable itself
    # has a length_hint > sys.maxsize
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize + 1)
    it = merge(of_it)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_merge_lengthhint_failure3():
    # Like the test case above but this time we take one item because
    # internally an unstarted "merge" and started "merge" are treated
    # differently
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize + 1)
    it = merge(of_it)
    next(it)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_merge_lengthhint_failure4():
    # Overflow could also happen when adding length_hints that individually are
    # below the sys.maxsize
    # In this case we have 3 + sys.maxsize
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize)
    it = merge(toT([1, 2, 3]), of_it)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)


@pytest.mark.xfail(not iteration_utilities.GE_PY34,
                   reason='length does not work before Python 3.4')
@memory_leak_decorator(collect=True)
def test_merge_lengthhint_failure5():
    # Like the test above but this time with a "started" merge
    of_it = _hf.OverflowLengthHint(toT([1, 2, 3]), sys.maxsize)
    it = merge(toT([1, 2, 3]), of_it)
    next(it)
    with pytest.raises(OverflowError):
        operator.length_hint(it)

    with pytest.raises(OverflowError):
        list(it)
