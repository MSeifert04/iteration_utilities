# Licensed under Apache License Version 2.0 - see LICENSE

import collections
import pickle

import pytest

from iteration_utilities import deepflatten

import helper_funcs as _hf
from helper_cls import T, toT


def test_deepflatten_empty1():
    assert list(deepflatten([])) == []


def test_deepflatten_attributes1():
    it = deepflatten([[T(1)], T(2)])
    assert it.depth == -1
    assert it.currentdepth == 0
    assert it.ignore is None
    assert it.types is None

    assert next(it) == T(1)

    assert it.currentdepth == 1


def test_deepflatten_normal1():
    assert list(deepflatten([T(1), T(2), T(3)])) == [T(1), T(2), T(3)]


def test_deepflatten_normal2():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]])) == toT([1, 2, 3])


def test_deepflatten_normal3():
    # really deeply nested thingy
    assert list(deepflatten([[[[[[[[[[[T(5), T(4), T(3), T(2), T(1), T(0)]]]]],
                               map(T, range(3))]]],
                            (T(i) for i in range(5))]]])
                ) == toT([5, 4, 3, 2, 1, 0, 0, 1, 2, 0, 1, 2, 3, 4])


def test_deepflatten_normal4():
    # really deeply nested thingy with types
    assert list(deepflatten([[[[[[[[[[[T(5), T(4), T(3), T(2), T(1), T(0)]]]]],
                               [T(0), T(1), T(2)]]]],
                            [T(0), T(1), T(2), T(3), T(4)]]]], types=list)
                ) == toT([5, 4, 3, 2, 1, 0, 0, 1, 2, 0, 1, 2, 3, 4])


def test_deepflatten_containing_strings1():
    # no endless recursion even if we have strings in the iterable
    assert list(deepflatten(["abc", "def"])) == ['a', 'b', 'c', 'd', 'e', 'f']


def test_deepflatten_containing_strings2():
    # no endless recursion even if we have strings in the iterable and gave
    # strings as types
    assert list(deepflatten(["abc", "def"],
                            types=str)) == ['a', 'b', 'c', 'd', 'e', 'f']


def test_deepflatten_containing_strings3():
    # mixed with strings
    assert list(deepflatten(["abc", ("def",), "g", [[{'h'}], 'i']],
                            )) == ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i']


def test_deepflatten_depth1():
    assert list(deepflatten([T(1), T(2), T(3)], 1)) == toT([1, 2, 3])


def test_deepflatten_depth2():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]],
                            1)) == [T(1), T(2), [T(3)]]


def test_deepflatten_types1():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]],
                            types=list)) == toT([1, 2, 3])


def test_deepflatten_types2():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]],
                            types=tuple)) == [[T(1)], T(2), [[T(3)]]]


def test_deepflatten_types3():
    assert list(deepflatten([[T(1)], T(2), ([T(3)], )],
                            types=(list, tuple))) == toT([1, 2, 3])


def test_deepflatten_ignore1():
    assert list(deepflatten([[T(1)], T(2), [[T(3), 'abc']]],
                            ignore=str)) == [T(1), T(2), T(3), 'abc']


def test_deepflatten_ignore2():
    assert list(deepflatten([[T(1)], T(2), ([T(3), 'abc'], )],
                            ignore=(tuple, str))
                ) == [T(1), T(2), ([T(3), 'abc'], )]


def test_deepflatten_failure1():
    with pytest.raises(TypeError):
        list(deepflatten([T(1), T(2), T(3)], None, T('a')))


def test_deepflatten_failure2():
    # recursively iterable data structures like strings that return another
    # string in their iter.
    with pytest.raises(RecursionError):
        list(deepflatten([collections.UserString('abc')]))


def test_deepflatten_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(deepflatten(_hf.FailNext()))


def test_deepflatten_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(_hf.FailNext.EXC_TYP, match=_hf.FailNext.EXC_MSG):
        next(deepflatten([[_hf.FailNext()], 2]))


def test_deepflatten_failure5():
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        deepflatten(_hf.FailIter())


def test_deepflatten_failure6():
    # specified not iterable type as types
    with pytest.raises(TypeError):
        list(deepflatten([T(1), 2., T(3), T(4)], types=float))


def test_deepflatten_failure7():
    # object that raises something else than TypeError when not iterable
    with pytest.raises(_hf.FailIter.EXC_TYP, match=_hf.FailIter.EXC_MSG):
        list(deepflatten([T(1), _hf.FailIter(), T(3), T(4)]))


def test_deepflatten_failure8():
    # accessing iterator after exhausting the iterable
    df = deepflatten(toT([1, 2, 3, 4]))
    assert list(df) == toT([1, 2, 3, 4])
    nothing = object()
    assert next(df, nothing) is nothing


def test_deepflatten_failure9():
    # Check that everyting is working even if isinstance fails
    df = deepflatten(toT([1, 2, 3, 4]), types=_hf.FailingIsinstanceClass)
    with pytest.raises(_hf.FailingIsinstanceClass.EXC_TYP, match=_hf.FailingIsinstanceClass.EXC_MSG):
        list(df)


def test_deepflatten_failure10():
    # Check that everyting is working even if isinstance fails
    df = deepflatten(toT([1, 2, 3, 4]), ignore=_hf.FailingIsinstanceClass)
    with pytest.raises(_hf.FailingIsinstanceClass.EXC_TYP, match=_hf.FailingIsinstanceClass.EXC_MSG):
        list(df)


def test_deepflatten_copy1():
    _hf.iterator_copy(deepflatten(toT([1, 2, 3, 4])))


def test_deepflatten_failure_setstate1():
    # using __setstate__ to pass in an invalid iteratorlist
    df = deepflatten(toT([1, 2, 3, 4]))
    with pytest.raises(TypeError):
        df.__setstate__(({'a', 'b', 'c'}, 0, 0))


def test_deepflatten_failure_setstate2():
    # using __setstate__ to pass in an invalid iteratorlist (not iterator
    # inside)
    df = deepflatten(toT([1, 2, 3, 4]))
    with pytest.raises(TypeError):
        df.__setstate__(([set(toT([1, 2, 3, 4]))], 0, 0))


def test_deepflatten_failure_setstate3():
    # using __setstate__ to pass in an invalid currentdepth (too low)
    df = deepflatten(toT([1, 2, 3, 4]))
    with pytest.raises(ValueError):
        df.__setstate__(([iter(toT([1, 2, 3, 4]))], -3, 0))


def test_deepflatten_failure_setstate4():
    # using __setstate__ to pass in an invalid currentdepth (too high)
    df = deepflatten(toT([1, 2, 3, 4]))
    with pytest.raises(ValueError):
        df.__setstate__(([iter(toT([1, 2, 3, 4]))], 5, 0))


def test_deepflatten_failure_setstate5():
    _hf.iterator_setstate_list_fail(deepflatten(toT([1, 2, 3, 4])))


def test_deepflatten_failure_setstate6():
    _hf.iterator_setstate_empty_fail(deepflatten(toT([1, 2, 3, 4])))


def test_deepflatten_reduce1():
    # Earlier we were able to modify the iteratorlist (including deleting
    # parts of it). That could lead to segmentation faults.
    df = deepflatten(toT([1, 2, 3, 4, 5, 6]))
    next(df)
    # Clear the iteratorlist from all items.
    df.__reduce__()[2][0][:] = []
    next(df)


def test_deepflatten_setstate1():
    # We could keep a reference to the iteratorlist passed to setstate and
    # mutate it (leading to incorrect behavior and segfaults).
    df = deepflatten(toT([1, 2, 3, 4, 5, 6]))
    next(df)
    # Easiest way is to roundtrip the state but keep the state as variable so
    # we can modify it!
    state = df.__reduce__()[2]
    df.__setstate__(state)
    state[0][:] = []
    next(df)


def test_deepflatten_pickle1(protocol):
    dpflt = deepflatten([[T(1)], [T(2)], [T(3)], [T(4)]])
    assert next(dpflt) == T(1)
    x = pickle.dumps(dpflt, protocol=protocol)
    assert list(pickle.loads(x)) == toT([2, 3, 4])


def test_deepflatten_pickle2(protocol):
    dpflt = deepflatten([['abc', T(1)], [T(2)], [T(3)], [T(4)]])
    assert next(dpflt) == 'a'
    x = pickle.dumps(dpflt, protocol=protocol)
    assert list(pickle.loads(x)) == ['b', 'c'] + toT([1, 2, 3, 4])
