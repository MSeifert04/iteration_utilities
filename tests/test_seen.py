# Built-ins
from __future__ import absolute_import, division, print_function
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


Seen = iteration_utilities.Seen


@memory_leak_decorator()
def test_seen_equality0():
    assert Seen() == Seen()


@memory_leak_decorator()
def test_seen_equality1():
    assert Seen({T(1), T(2)}) == Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_equality2():
    assert not Seen({T(1), T(2), T(3)}) == Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_equality3():
    assert Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 0])])


@memory_leak_decorator()
def test_seen_equality4():
    assert not Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 1])])


@memory_leak_decorator()
def test_seen_equality5():
    assert not Seen({T(1)}, [T([0, 0]), T([1, 0])]) == Seen({T(1)}, [T([0, 1])])


@memory_leak_decorator()
def test_seen_equality6():
    assert Seen(set()) == Seen(set(), [])


@memory_leak_decorator()
def test_seen_equality7():
    assert Seen(set(), []) == Seen(set())


@memory_leak_decorator()
def test_seen_nequality0():
    assert not Seen() != Seen()


@memory_leak_decorator()
def test_seen_nequality1():
    assert not Seen({T(1), T(2)}) != Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_nequality2():
    assert Seen({T(1), T(2), T(3)}) != Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_nequality3():
    assert not Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 0])])


@memory_leak_decorator()
def test_seen_nequality4():
    assert Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 1])])


@memory_leak_decorator()
def test_seen_nequality5():
    assert Seen({T(1)}, [T([0, 0]), [T(1), T(0)]]) != Seen({T(1)}, [T([0, 0])])


@memory_leak_decorator()
def test_seen_nequality6():
    assert not Seen(set()) != Seen(set(), [])


@memory_leak_decorator()
def test_seen_nequality7():
    assert not Seen(set(), []) != Seen(set())


@memory_leak_decorator()
def test_seen_len0():
    assert not len(Seen())
    assert len(Seen({T(1), T(2), T(3)})) == 3
    assert len(Seen(seenlist=[[T(0), T(0)], [T(1), T(1)], [T(2), T(2)]])) == 3
    assert len(Seen({T(1), T(2), T(3)},
                    seenlist=[[T(0), T(0)], [T(1), T(1)], [T(2), T(2)]])) == 6


@memory_leak_decorator()
def test_seen_contains0():
    x = Seen()
    assert T(1) not in x
    assert x == Seen(set())
    assert T([0, 0]) not in x
    assert x == Seen(set())


@memory_leak_decorator()
def test_seen_containsadd0():
    x = Seen()
    assert not x.contains_add(T(1))
    assert not x.contains_add(T([0, 0]))
    assert T(1) in x
    assert T([0, 0]) in x
    assert x == Seen({T(1)}, [T([0, 0])])


# Pickle tests and most failure tests are implemented implicitly as part of
# unique_everseen, duplicates, all_distinct so there should be no need to
# repeat these here. But if "Seen" is expanded these should be included!!!
