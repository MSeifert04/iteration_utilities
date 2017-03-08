# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT, failingTIterator


if iteration_utilities.EQ_PY2:
    from itertools import imap as map
    from UserString import UserString
    string_types = basestring
else:
    from collections import UserString
    string_types = str

if not iteration_utilities.GE_PY35:
    RecursionError = RuntimeError


deepflatten = iteration_utilities.deepflatten


@memory_leak_decorator()
def test_deepflatten_empty1():
    assert list(deepflatten([])) == []


@memory_leak_decorator()
def test_deepflatten_normal1():
    assert list(deepflatten([T(1), T(2), T(3)])) == [T(1), T(2), T(3)]


@memory_leak_decorator()
def test_deepflatten_normal2():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]])) == toT([1, 2, 3])


@memory_leak_decorator()
def test_deepflatten_normal3():
    # really deeply nested thingy
    assert list(deepflatten([[[[[[[[[[[T(5), T(4), T(3), T(2), T(1), T(0)]]]]],
                               map(T, range(3))]]],
                            (T(i) for i in range(5))]]])
                ) == toT([5, 4, 3, 2, 1, 0, 0, 1, 2, 0, 1, 2, 3, 4])


@memory_leak_decorator()
def test_deepflatten_normal4():
    # really deeply nested thingy with types
    assert list(deepflatten([[[[[[[[[[[T(5), T(4), T(3), T(2), T(1), T(0)]]]]],
                               [T(0), T(1), T(2)]]]],
                            [T(0), T(1), T(2), T(3), T(4)]]]], types=list)
                ) == toT([5, 4, 3, 2, 1, 0, 0, 1, 2, 0, 1, 2, 3, 4])


@memory_leak_decorator()
def test_deepflatten_containing_strings1():
    # no endless recursion even if we have strings in the iterable
    assert list(deepflatten(["abc", "def"])) == ['a', 'b', 'c', 'd', 'e', 'f']


@memory_leak_decorator()
def test_deepflatten_containing_strings2():
    # no endless recursion even if we have strings in the iterable and gave
    # strings as types
    assert list(deepflatten(["abc", "def"],
                            types=str)) == ['a', 'b', 'c', 'd', 'e', 'f']


@memory_leak_decorator()
def test_deepflatten_containing_strings3():
    # mixed with strings
    assert list(deepflatten(["abc", ("def",), "g", [[{'h'}], 'i']],
                            )) == ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i']


@memory_leak_decorator()
def test_deepflatten_depth1():
    assert list(deepflatten([T(1), T(2), T(3)], 1)) == toT([1, 2, 3])


@memory_leak_decorator()
def test_deepflatten_depth2():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]],
                            1)) == [T(1), T(2), [T(3)]]


@memory_leak_decorator()
def test_deepflatten_types1():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]],
                            types=list)) == toT([1, 2, 3])


@memory_leak_decorator()
def test_deepflatten_types2():
    assert list(deepflatten([[T(1)], T(2), [[T(3)]]],
                            types=tuple)) == [[T(1)], T(2), [[T(3)]]]


@memory_leak_decorator()
def test_deepflatten_types3():
    assert list(deepflatten([[T(1)], T(2), ([T(3)], )],
                            types=(list, tuple))) == toT([1, 2, 3])


@memory_leak_decorator()
def test_deepflatten_ignore1():
    assert list(deepflatten([[T(1)], T(2), [[T(3), 'abc']]],
                            ignore=string_types)) == [T(1), T(2), T(3), 'abc']


@memory_leak_decorator()
def test_deepflatten_ignore2():
    assert list(deepflatten([[T(1)], T(2), ([T(3), 'abc'], )],
                            ignore=(tuple, string_types))
                ) == [T(1), T(2), ([T(3), 'abc'], )]


@memory_leak_decorator(collect=True)
def test_deepflatten_failure1():
    with pytest.raises(TypeError):
        list(deepflatten([T(1), T(2), T(3)], None, T('a')))


@memory_leak_decorator(collect=True)
def test_deepflatten_failure2():
    # recursivly iterable data structures like strings that return another
    # string in their iter.
    with pytest.raises(RecursionError):
        list(deepflatten([UserString('abc')]))


@memory_leak_decorator(collect=True)
def test_deepflatten_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(deepflatten(failingTIterator()))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_deepflatten_failure4():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(deepflatten([[failingTIterator()], 2]))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_deepflatten_failure5():
    # not iterable
    with pytest.raises(TypeError):
        deepflatten(T(1))


@memory_leak_decorator(collect=True)
def test_deepflatten_failure6():
    # specified not iterable type as types
    with pytest.raises(TypeError):
        list(deepflatten([T(1), 2., T(3), T(4)], types=float))


@memory_leak_decorator(collect=True)
def test_deepflatten_failure7():
    # object that raises something else than TypeError when not iterable
    class NotIterable(object):
        def __iter__(self):
            raise ValueError('bad class')

    with pytest.raises(ValueError) as exc:
        list(deepflatten([T(1), NotIterable(), T(3), T(4)]))
    assert 'bad class' in str(exc)


@memory_leak_decorator(collect=True)
def test_deepflatten_failure8():
    # accessing iterator after exhausting the iterable

    df = deepflatten(toT([1, 2, 3, 4]))

    assert list(df) == toT([1, 2, 3, 4])

    nothing = object()
    assert next(df, nothing) is nothing


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_deepflatten_pickle1():
    dpflt = deepflatten([[T(1)], [T(2)], [T(3)], [T(4)]])
    assert next(dpflt) == T(1)
    x = pickle.dumps(dpflt)
    assert list(pickle.loads(x)) == toT([2, 3, 4])


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_deepflatten_pickle2():
    dpflt = deepflatten([['abc', T(1)], [T(2)], [T(3)], [T(4)]])
    assert next(dpflt) == 'a'
    x = pickle.dumps(dpflt)
    assert list(pickle.loads(x)) == ['b', 'c'] + toT([1, 2, 3, 4])
