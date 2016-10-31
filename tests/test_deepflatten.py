# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT

string_types = basestring if iteration_utilities.PY2 else str


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


@pytest.mark.xfail(iteration_utilities.PY2, reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_deepflatten_pickle1():
    dpflt = deepflatten([[T(1)], [T(2)], [T(3)], [T(4)]])
    assert next(dpflt) == T(1)
    x = pickle.dumps(dpflt)
    assert list(pickle.loads(x)) == toT([2, 3, 4])
