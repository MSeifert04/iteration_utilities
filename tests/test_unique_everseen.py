# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, failingTIterator


unique_everseen = iteration_utilities.unique_everseen
Seen = iteration_utilities.Seen


@memory_leak_decorator()
def test_uniqueeverseen_empty1():
    assert list(unique_everseen([])) == []


@memory_leak_decorator()
def test_uniqueeverseen_normal1():
    assert list(unique_everseen([T(1), T(2), T(1)])) == [T(1), T(2)]


@memory_leak_decorator()
def test_uniqueeverseen_key1():
    assert list(unique_everseen([T(1), T(2), T(1)], abs)) == [T(1), T(2)]


@memory_leak_decorator()
def test_uniqueeverseen_key2():
    assert list(unique_everseen([T(1), T(1), T(-1)], abs)) == [T(1)]


@memory_leak_decorator()
def test_uniqueeverseen_unhashable1():
    assert list(unique_everseen([{T(1): T(1)}, {T(2): T(2)},
                                 {T(1): T(1)}])) == [{T(1): T(1)},
                                                     {T(2): T(2)}]


@memory_leak_decorator()
def test_uniqueeverseen_unhashable2():
    assert list(unique_everseen([[T(1)], [T(2)], [T(1)]])) == [[T(1)], [T(2)]]


@memory_leak_decorator()
def test_uniqueeverseen_unhashable3():
    assert list(unique_everseen([[T(1), T(1)], [T(1), T(2)], [T(1), T(3)]],
                                operator.itemgetter(0))) == [[T(1), T(1)]]


@memory_leak_decorator()
def test_uniqueeverseen_getter1():
    t = unique_everseen([T(1), T([0, 0]), T(3)])
    assert not t.seen
    assert t.key is None
    assert next(t) == T(1)
    assert t.seen == Seen({T(1)})
    assert t.key is None
    assert next(t) == T([0, 0])
    assert T(1) in t.seen
    assert T([0, 0]) in t.seen
    assert t.key is None
    assert next(t) == T(3)
    assert t.seen == Seen({T(1), T(3)}, [T([0, 0])])
    assert t.key is None


@memory_leak_decorator()
def test_uniqueeverseen_getter2():
    t = unique_everseen([T(1), T([0, 0]), T(3)],
                        iteration_utilities.return_identity)
    assert not t.seen
    assert t.key is iteration_utilities.return_identity
    assert next(t) == T(1)
    assert t.seen == Seen({T(1)})
    assert t.key is iteration_utilities.return_identity
    assert next(t) == T([0, 0])
    assert T(1) in t.seen
    assert T([0, 0]) in t.seen
    assert t.key is iteration_utilities.return_identity
    assert next(t) == T(3)
    assert t.seen == Seen({T(1), T(3)}, [T([0, 0])])
    assert t.key is iteration_utilities.return_identity


@memory_leak_decorator(collect=True)
def test_uniqueeverseen_failure1():
    with pytest.raises(TypeError):
        list(unique_everseen(T(10)))


@memory_leak_decorator(collect=True)
def test_uniqueeverseen_failure2():
    with pytest.raises(TypeError):
        list(unique_everseen([T(1), T(2), T(3), T('a')], abs))


@memory_leak_decorator(collect=True)
def test_uniqueeverseen_failure3():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        next(unique_everseen(failingTIterator()))
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_uniqueeverseen_failure4():
    # Too few arguments
    with pytest.raises(TypeError):
        unique_everseen()


@pytest.mark.xfail(iteration_utilities.EQ_PY2,
                   reason='pickle does not work on Python 2')
@memory_leak_decorator(offset=1)
def test_uniqueeverseen_pickle1():
    uqe = unique_everseen([T(1), T(2), T(1), T(2)])
    assert next(uqe) == T(1)
    x = pickle.dumps(uqe)
    assert list(pickle.loads(x)) == [T(2)]
