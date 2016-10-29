# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak


class T(object):
    def __init__(self, value):
        self.value = value

    def __hash__(self):
        return hash(self.value)

    def __eq__(self, other):
        return self.value == other.value


Seen = iteration_utilities.Seen


def test_seen_equality0():
    assert Seen() == Seen()

    def test():
        Seen() == Seen()
    assert not memory_leak(test)


def test_seen_equality1():
    assert Seen({1, 2}) == Seen({1, 2})

    def test():
        Seen({T(1), T(2)}) == Seen({T(1), T(2)})
    assert not memory_leak(test)


def test_seen_equality2():
    assert not Seen({1, 2, 3}) == Seen({1, 2})

    def test():
        Seen({T(1), T(2), T(3)}) == Seen({T(1), T(2)})
    assert not memory_leak(test)


def test_seen_equality3():
    assert Seen({1, 2}, [[0, 0]]) == Seen({1, 2}, [[0, 0]])

    def test():
        Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 0])])
    assert not memory_leak(test)


def test_seen_equality4():
    assert not Seen({1, 2}, [[0, 0]]) == Seen({1, 2}, [[0, 1]])

    def test():
        Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 1])])
    assert not memory_leak(test)


def test_seen_equality5():
    assert not Seen({1, 2}, [[0, 0], [1, 0]]) == Seen({1, 2}, [[0, 1]])

    def test():
        Seen({T(1)}, [T([0, 0]), T([1, 0])]) == Seen({T(1)}, [T([0, 1])])
    assert not memory_leak(test)


def test_seen_equality6():
    assert Seen(set()) == Seen(set(), [])

    def test():
        Seen(set()) == Seen(set(), [])
    assert not memory_leak(test)


def test_seen_equality7():
    assert Seen(set(), []) == Seen(set())

    def test():
        Seen(set(), []) == Seen(set())
    assert not memory_leak(test)


def test_seen_nequality0():
    assert not Seen() != Seen()

    def test():
        Seen() != Seen()
    assert not memory_leak(test)


def test_seen_nequality1():
    assert not Seen({1, 2}) != Seen({1, 2})

    def test():
        Seen({T(1), T(2)}) != Seen({T(1), T(2)})
    assert not memory_leak(test)


def test_seen_nequality2():
    assert Seen({1, 2, 3}) != Seen({1, 2})

    def test():
        Seen({T(1), T(2), T(3)}) != Seen({T(1), T(2)})
    assert not memory_leak(test)


def test_seen_nequality3():
    assert not Seen({1, 2}, [[0, 0]]) != Seen({1, 2}, [[0, 0]])

    def test():
        Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 0])])
    assert not memory_leak(test)


def test_seen_nequality4():
    assert Seen({1, 2}, [[0, 0]]) != Seen({1, 2}, [[0, 1]])

    def test():
        Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 1])])
    assert not memory_leak(test)


def test_seen_nequality5():
    assert Seen({1, 2}, [[0, 0], [1, 0]]) != Seen({1, 2}, [[0, 1]])

    def test():
        Seen({T(1)}, [T([0, 0]), [T(1), T(0)]]) != Seen({T(1)}, [T([0, 0])])
    assert not memory_leak(test)


def test_seen_nequality6():
    assert not Seen(set()) != Seen(set(), [])

    def test():
        Seen(set()) != Seen(set(), [])
    assert not memory_leak(test)


def test_seen_nequality7():
    assert not Seen(set(), []) != Seen(set())

    def test():
        Seen(set(), []) != Seen(set())
    assert not memory_leak(test)


def test_seen_len0():
    assert not len(Seen())
    assert len(Seen({1,2,3})) == 3
    assert len(Seen(seenlist=[[0, 0], [1, 1], [2, 2]])) == 3
    assert len(Seen({1,2,3}, seenlist=[[0, 0], [1, 1], [2, 2]])) == 6

    def test():
        len(Seen())
        len(Seen({T(1),T(2),T(3)}))
        len(Seen(seenlist=[[0, 0], [1, 1], [2, 2]]))
        len(Seen({1,2,3}, seenlist=[[0, 0], [1, 1], [2, 2]]))
    assert not memory_leak(test)


def test_seen_contains0():
    x = Seen()
    assert 1 not in x
    assert x == Seen(set())
    assert [0, 0] not in x
    assert x == Seen(set())

    def test():
        x = Seen()
        assert T(1) not in x
        assert x == Seen(set())
        assert T([0, 0]) not in x
        assert x == Seen(set())
    assert not memory_leak(test)


def test_seen_containsadd0():
    x = Seen()
    assert not x.contains_add(1)
    assert not x.contains_add([0, 0])
    assert 1 in x
    assert [0, 0] in x
    assert x == Seen({1}, [[0, 0]])

    def test():
        x = Seen()
        assert not x.contains_add(T(1))
        assert not x.contains_add(T([0, 0]))
        assert T(1) in x
        assert T([0, 0]) in x
        assert x == Seen({T(1)}, [T([0, 0])])
    assert not memory_leak(test)


# Pickle tests and most failure tests are implemented implicitly as part of
# unique_everseen, duplicates, all_distinct so there should be no need to
# repeat these here. But if "Seen" is expanded these should be included!!!
