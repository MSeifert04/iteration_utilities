# Licensed under Apache License Version 2.0 - see LICENSE

# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T


Seen = iteration_utilities.Seen


@memory_leak_decorator()
def test_seen_new_None1():
    # seenset=None is identical to no seenset
    assert Seen(None) == Seen()


@memory_leak_decorator()
def test_seen_new_None2():
    # seenlist=None is identical to no seenlist
    assert Seen(set(), None) == Seen(set())


@memory_leak_decorator()
def test_seen_equality0():
    assert Seen() == Seen()


@memory_leak_decorator()
def test_seen_equality1():
    # only sets, identical contents
    assert Seen({T(1), T(2)}) == Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_equality2():
    # only sets, not identical contents
    assert not Seen({T(1), T(2), T(3)}) == Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_equality3():
    # set and list, identical contents
    assert Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 0])])


@memory_leak_decorator()
def test_seen_equality4():
    # set and list, not identical list contents
    assert not Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 1])])


@memory_leak_decorator()
def test_seen_equality5():
    # set and list, not identical list contents
    assert not Seen({T(1)}, [T([0, 0]), T([1, 0])]) == Seen({T(1)}, [T([0, 1])])


@memory_leak_decorator()
def test_seen_equality6():
    # empty sets, one has empty list
    assert Seen(set()) == Seen(set(), [])


@memory_leak_decorator()
def test_seen_equality7():
    # empty sets, one has empty list
    assert Seen(set(), []) == Seen(set())


@memory_leak_decorator()
def test_seen_equality8():
    # empty sets, one has not-empty list
    assert not Seen(set(), [[T(0)]]) == Seen(set())


@memory_leak_decorator()
def test_seen_equality9():
    # empty sets, one has not-empty list
    assert not Seen(set()) == Seen(set(), [[T(0)]])


@memory_leak_decorator()
def test_seen_nequality0():
    assert not Seen() != Seen()


@memory_leak_decorator()
def test_seen_nequality1():
    # only sets, identical contents
    assert not Seen({T(1), T(2)}) != Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_nequality2():
    # only sets, not identical contents
    assert Seen({T(1), T(2), T(3)}) != Seen({T(1), T(2)})


@memory_leak_decorator()
def test_seen_nequality3():
    # set and list, identical contents
    assert not Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 0])])


@memory_leak_decorator()
def test_seen_nequality4():
    # set and list, not identical list contents
    assert Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 1])])


@memory_leak_decorator()
def test_seen_nequality5():
    # set and list, not identical list contents
    assert Seen({T(1)}, [T([0, 0]), [T(1), T(0)]]) != Seen({T(1)}, [T([0, 0])])


@memory_leak_decorator()
def test_seen_nequality6():
    # empty sets, one has empty list
    assert not Seen(set()) != Seen(set(), [])


@memory_leak_decorator()
def test_seen_nequality7():
    # empty sets, one has empty list
    assert not Seen(set(), []) != Seen(set())


@memory_leak_decorator()
def test_seen_noequality8():
    # empty sets, one has not-empty list
    assert Seen(set(), [[T(0)]]) != Seen(set())


@memory_leak_decorator()
def test_seen_noequality9():
    # empty sets, one has not-empty list
    assert Seen(set()) != Seen(set(), [[T(0)]])


@memory_leak_decorator(collect=True)
def test_seen_cmpfailure1():
    class HashButNoEq(object):
        def __init__(self, val):
            self.val = val

        def __hash__(self):
            return 1

        def __eq__(self, other):
            raise ValueError('bad class')
    s1 = Seen({HashButNoEq(1)})
    s2 = Seen({HashButNoEq(1)})
    with pytest.raises(ValueError) as exc:
        s1 == s2
    assert 'bad class' in str(exc)
    with pytest.raises(ValueError) as exc:
        s1 != s2
    assert 'bad class' in str(exc)


@memory_leak_decorator(collect=True)
def test_seen_cmpfailure2():
    class HashButNoEq(object):
        def __init__(self, val):
            self.val = val

        def __hash__(self):
            return 1

        def __eq__(self, other):
            raise ValueError('bad class')
    s1 = Seen(set(), [HashButNoEq(1)])
    s2 = Seen(set(), [HashButNoEq(1)])
    with pytest.raises(ValueError) as exc:
        s1 == s2
    assert 'bad class' in str(exc)
    with pytest.raises(ValueError) as exc:
        s1 != s2
    assert 'bad class' in str(exc)


@memory_leak_decorator()
def test_seen_othercmp1():
    # other comparisons than == or != fail
    with pytest.raises(TypeError):
        Seen(set()) < Seen(set())
    with pytest.raises(TypeError):
        Seen(set()) <= Seen(set())
    with pytest.raises(TypeError):
        Seen(set()) >= Seen(set())
    with pytest.raises(TypeError):
        Seen(set()) > Seen(set())


@memory_leak_decorator()
def test_seen_len0():
    assert not len(Seen())
    assert len(Seen({T(1), T(2), T(3)})) == 3
    assert len(Seen(seenlist=[[T(0), T(0)], [T(1), T(1)], [T(2), T(2)]])) == 3
    assert len(Seen({T(1), T(2), T(3)},
                    seenlist=[[T(0), T(0)], [T(1), T(1)], [T(2), T(2)]])) == 6


@memory_leak_decorator()
def test_seen_repr0():
    # Just test that no exception occurs, due to the representation change in
    # sets this would otherwise have to be tested depending on the python
    # version.
    assert repr(Seen())
    assert repr(Seen({T(1)}))
    assert repr(Seen(set(), []))
    assert repr(Seen(set(), [T(1)]))


@memory_leak_decorator(collect=True)
def test_seen_repr1():
    # check that even though it can't be immediatly set that recursive
    # representations are catched
    s = Seen()
    s.contains_add([s])
    if iteration_utilities.EQ_PY2:
        assert repr(s) == 'iteration_utilities.Seen(set([]), seenlist=[[...]])'
    else:
        assert repr(s) == 'iteration_utilities.Seen(set(), seenlist=[[...]])'


@memory_leak_decorator(collect=True, offset=1)
def test_seen_repr2():
    # Check that the representation is class name aware
    class Fun(Seen):
        pass

    assert 'Fun' in repr(Fun())
    assert 'Fun' in repr(Fun({T(1)}))
    assert 'Fun' in repr(Fun(set(), []))
    assert 'Fun' in repr(Fun(set(), [T(1)]))


@memory_leak_decorator()
def test_seen_contains0():
    x = Seen()
    assert T(1) not in x
    assert x == Seen(set())
    assert T([0, 0]) not in x
    assert x == Seen(set())


@memory_leak_decorator(collect=True)
def test_seen_contains_failure1():
    # Failure (no TypeError) when trying to hash the value
    class NoHash():
        def __hash__(self):
            raise ValueError('bad class')

    x = Seen({T(0)})
    with pytest.raises(ValueError) as exc:
        NoHash() in x
    assert 'bad class' in str(exc)


@memory_leak_decorator(collect=True)
def test_seen_contains_failure2():
    # Failure when comparing the object to the objects in the list
    class NoHashNoEq():
        def __hash__(self):
            raise TypeError('cannot be hashed')

        def __eq__(self, other):
            raise ValueError('bad class')

    x = Seen(set(), [T(0)])
    with pytest.raises(ValueError) as exc:
        NoHashNoEq() in x
    assert 'bad class' in str(exc)


@memory_leak_decorator()
def test_seen_containsadd0():
    x = Seen()
    assert not x.contains_add(T(1))
    assert not x.contains_add(T([0, 0]))
    assert T(1) in x
    assert T([0, 0]) in x
    assert x == Seen({T(1)}, [T([0, 0])])


@memory_leak_decorator(collect=True)
def test_seen_containsadd_failure1():
    # Failure (no TypeError) when trying to hash the value
    class NoHash():
        def __hash__(self):
            raise ValueError('bad class')

    x = Seen({T(0)})
    with pytest.raises(ValueError) as exc:
        x.contains_add(NoHash())
    assert 'bad class' in str(exc)


@memory_leak_decorator(collect=True)
def test_seen_containsadd_failure2():
    # Failure when comparing the object to the objects in the list
    class NoHashNoEq():
        def __hash__(self):
            raise TypeError('cannot be hashed')

        def __eq__(self, other):
            raise ValueError('bad class')

    x = Seen(set(), [T(0)])
    with pytest.raises(ValueError) as exc:
        x.contains_add(NoHashNoEq())
    assert 'bad class' in str(exc)


# Pickle tests and most failure tests are implemented implicitly as part of
# unique_everseen, duplicates, all_distinct so there should be no need to
# repeat these here. But if "Seen" is expanded these should be included!!!


@memory_leak_decorator(collect=True)
def test_seen_failures1():
    # too many arguments
    with pytest.raises(TypeError):
        Seen({10, 20}, [1, 2, 3], [1, 2, 3])


@memory_leak_decorator(collect=True)
def test_seen_failures2():
    # seenset not a set
    with pytest.raises(TypeError) as exc:
        Seen(frozenset({10, 20}))
    assert '`seenset`' in str(exc) and 'set' in str(exc)


@memory_leak_decorator(collect=True)
def test_seen_failures3():
    # seenlist must be a list
    with pytest.raises(TypeError) as exc:
        Seen({10, 20}, tuple([1, 2, 3]))
    assert '`seenlist`' in str(exc) and 'list' in str(exc)


@memory_leak_decorator(collect=True)
def test_seen_failures4():
    # seen can only be compared to other seen's.
    with pytest.raises(TypeError) as exc:
        Seen() == set()
    assert ('`Seen` instances can only compared to other `Seen` '
            'instances.' in str(exc))
