# Licensed under Apache License Version 2.0 - see LICENSE

import pytest

import iteration_utilities
from iteration_utilities import Seen

import helper_funcs as _hf
from helper_cls import T


def test_seen_new_None1():
    # seenset=None is identical to no seenset
    assert Seen(None) == Seen()


def test_seen_new_None2():
    # seenlist=None is identical to no seenlist
    assert Seen(set(), None) == Seen(set())


def test_seen_equality0():
    assert Seen() == Seen()
    assert not Seen() != Seen()


def test_seen_equality1():
    # only sets, identical contents
    assert Seen({T(1), T(2)}) == Seen({T(1), T(2)})
    assert not Seen({T(1), T(2)}) != Seen({T(1), T(2)})


def test_seen_equality2():
    # only sets, not identical contents
    assert not Seen({T(1), T(2), T(3)}) == Seen({T(1), T(2)})
    assert Seen({T(1), T(2), T(3)}) != Seen({T(1), T(2)})


def test_seen_equality3():
    # set and list, identical contents
    assert Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 0])])
    assert not Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 0])])


def test_seen_equality4():
    # set and list, not identical list contents
    assert not Seen({T(1)}, [T([0, 0])]) == Seen({T(1)}, [T([0, 1])])
    assert Seen({T(1)}, [T([0, 0])]) != Seen({T(1)}, [T([0, 1])])


def test_seen_equality5():
    # set and list, not identical list contents
    assert not Seen({T(1)}, [T([0, 0]), T([1, 0])]) == Seen({T(1)}, [T([0, 1])])
    assert Seen({T(1)}, [T([0, 0]), [T(1), T(0)]]) != Seen({T(1)}, [T([0, 0])])


def test_seen_equality6():
    # empty sets, one has empty list
    assert Seen(set()) == Seen(set(), [])
    assert not Seen(set()) != Seen(set(), [])


def test_seen_equality7():
    # empty sets, one has empty list
    assert Seen(set(), []) == Seen(set())
    assert not Seen(set(), []) != Seen(set())


def test_seen_equality8():
    # empty sets, one has not-empty list
    assert not Seen(set(), [[T(0)]]) == Seen(set())
    assert Seen(set(), [[T(0)]]) != Seen(set())


def test_seen_equality9():
    # empty sets, one has not-empty list
    assert not Seen(set()) == Seen(set(), [[T(0)]])
    assert Seen(set()) != Seen(set(), [[T(0)]])


def test_seen_cmpfailure1():
    s1 = Seen({_hf.FailEqWithHash()})
    s2 = Seen({_hf.FailEqWithHash()})
    with pytest.raises(_hf.FailEqWithHash.EXC_TYP, match=_hf.FailEqWithHash.EXC_MSG):
        s1 == s2
    with pytest.raises(_hf.FailEqWithHash.EXC_TYP, match=_hf.FailEqWithHash.EXC_MSG):
        s1 != s2


def test_seen_cmpfailure2():
    s1 = Seen(set(), [_hf.FailEqWithHash()])
    s2 = Seen(set(), [_hf.FailEqWithHash()])
    with pytest.raises(_hf.FailEqWithHash.EXC_TYP, match=_hf.FailEqWithHash.EXC_MSG):
        s1 == s2
    with pytest.raises(_hf.FailEqWithHash.EXC_TYP, match=_hf.FailEqWithHash.EXC_MSG):
        s1 != s2


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


def test_seen_len0():
    assert not len(Seen())
    assert len(Seen({T(1), T(2), T(3)})) == 3
    assert len(Seen(seenlist=[[T(0), T(0)], [T(1), T(1)], [T(2), T(2)]])) == 3
    assert len(Seen({T(1), T(2), T(3)},
                    seenlist=[[T(0), T(0)], [T(1), T(1)], [T(2), T(2)]])) == 6


def test_seen_repr0():
    assert repr(Seen()) == 'iteration_utilities.Seen(set())'
    assert repr(Seen({T(1)})) == 'iteration_utilities.Seen({T(1)})'
    assert repr(Seen(set(), [])) == repr(Seen())
    expected = 'iteration_utilities.Seen(set(), seenlist=[T(1)])'
    assert repr(Seen(set(), [T(1)])) == expected


def test_seen_repr1():
    # check that even though it can't be immediately set that recursive
    # representations are catched
    s = Seen()
    s.contains_add([s])
    assert repr(s) == 'iteration_utilities.Seen(set(), seenlist=[[...]])'


def test_seen_repr2():
    # Check that the representation is class name aware
    class Fun(Seen):
        pass

    assert 'Fun' in repr(Fun())
    assert 'Fun' in repr(Fun({T(1)}))
    assert 'Fun' in repr(Fun(set(), []))
    assert 'Fun' in repr(Fun(set(), [T(1)]))


def test_seen_attributes1():
    x = Seen()
    assert isinstance(x.seenset, set)
    assert x.seenlist is None


def test_seen_contains0():
    x = Seen()
    assert T(1) not in x
    assert x == Seen(set())
    assert T([0, 0]) not in x
    assert x == Seen(set())


def test_seen_contains_failure1():
    # Failure (no TypeError) when trying to hash the value
    x = Seen({T(0)})
    with pytest.raises(_hf.FailHash.EXC_TYP, match=_hf.FailHash.EXC_MSG):
        _hf.FailHash() in x


@_hf.skip_on_pypy_not_investigated_why
def test_seen_contains_failure2():
    # Failure when comparing the object to the objects in the list
    x = Seen(set(), [_hf.FailEqNoHash()])
    with pytest.raises(_hf.FailEqNoHash.EXC_TYP, match=_hf.FailEqNoHash.EXC_MSG):
        _hf.FailEqNoHash() in x


def test_seen_containsadd0():
    x = Seen()
    assert not x.contains_add(T(1))
    assert not x.contains_add(T([0, 0]))
    assert T(1) in x
    assert T([0, 0]) in x
    assert x == Seen({T(1)}, [T([0, 0])])


def test_seen_containsadd_failure1():
    # Failure (no TypeError) when trying to hash the value
    x = Seen({T(0)})
    with pytest.raises(_hf.FailHash.EXC_TYP, match=_hf.FailHash.EXC_MSG):
        x.contains_add(_hf.FailHash())


def test_seen_containsadd_failure2():
    # Failure when comparing the object to the objects in the list.
    x = Seen(set(), [_hf.FailEqNoHash()])
    with pytest.raises(_hf.FailEqNoHash.EXC_TYP, match=_hf.FailEqNoHash.EXC_MSG):
        x.contains_add(_hf.FailEqNoHash())


# Pickle tests and most failure tests are implemented implicitly as part of
# unique_everseen, duplicates, all_distinct so there should be no need to
# repeat these here. But if "Seen" is expanded these should be included!!!


def test_seen_failures1():
    # too many arguments
    with pytest.raises(TypeError):
        Seen({10, 20}, [1, 2, 3], [1, 2, 3])


def test_seen_failures2():
    # seenset not a set
    with pytest.raises(TypeError) as exc:
        Seen(frozenset({10, 20}))
    assert '`seenset`' in str(exc.value) and 'set' in str(exc.value)


def test_seen_failures3():
    # seenlist must be a list
    with pytest.raises(TypeError) as exc:
        Seen({10, 20}, tuple([1, 2, 3]))
    assert '`seenlist`' in str(exc.value) and 'list' in str(exc.value)


def test_seen_failures4():
    # seen can only be compared to other seen's.
    with pytest.raises(TypeError,
                       match='`Seen` instances can only compared to other '
                             '`Seen` instances'):
        Seen() == set()
