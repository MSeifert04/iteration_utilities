# Built-ins
from __future__ import absolute_import, division, print_function
import operator
import pickle

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak
from helper_pytest_monkeypatch import pytest_raises


def test_complement():
    complement = iteration_utilities.complement

    assert not complement(lambda x: x is True)(True)
    assert complement(lambda x: x is True)(False)

    assert complement(lambda x: x is False)(True)
    assert not complement(lambda x: x is False)(False)

    assert not complement(iteration_utilities.is_None)(None)
    assert complement(iteration_utilities.is_None)(False)
    assert complement(iteration_utilities.is_None)(True)


def test_complement_memoryleak():
    complement = iteration_utilities.complement

    def test():
        complement(lambda x: x is True)(True)
    assert not memory_leak(test)

    def test():
        complement(lambda x: x is True)(False)
    assert not memory_leak(test)

    def test():
        complement(lambda x: x is False)(True)
    assert not memory_leak(test)

    def test():
        complement(lambda x: x is False)(False)
    assert not memory_leak(test)

    def test():
        complement(iteration_utilities.is_None)(None)
    assert not memory_leak(test)

    def test():
        complement(iteration_utilities.is_None)(False)
    assert not memory_leak(test)

    def test():
        complement(iteration_utilities.is_None)(True)
    assert not memory_leak(test)


def test_compose():
    compose = iteration_utilities.compose

    double_increment = compose(lambda x: x*2, lambda x: x+1)
    assert double_increment(10) == 21
    assert double_increment(2.) == 5

    with pytest.raises(TypeError):  # at least one func must be present
        compose()

    with pytest.raises(TypeError):  # kwarg not accepted
        compose(lambda x: x+1, invalidkwarg=lambda x: x*2)

    with pytest.raises(TypeError):  # func fails
        compose(lambda x: x+1)('a')

    with pytest.raises(TypeError):  # second func fails
        compose(lambda x: x*2, lambda x: x+1)('a')


def test_compose_memoryleak():
    compose = iteration_utilities.compose

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __add__(self, other):
            return self.__class__(self.value + other)

        def __mul__(self, other):
            return self.__class__(self.value + other)

    def test():
        compose(lambda x: x*2, lambda x: x+1)(Test(10))
        compose(lambda x: x*2, lambda x: x+1)(Test(2))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # at least one func must be present
            compose()
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # kwarg not accepted
            compose(lambda x: x+1, invalidkwarg=lambda x: x*2)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # func fails
            compose(lambda x: x+1)(Test('a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # second func fails
            compose(lambda x: x*2, lambda x: x+1)(Test('a'))
    assert not memory_leak(test)


def test_one():
    one = iteration_utilities.one

    assert one([0]) == 0
    assert one('a') == 'a'
    assert one({'o': 10}) == 'o'

    with pytest.raises(TypeError):
        one(10)

    with pytest.raises(ValueError):  # empty iterable
        one([])

    with pytest.raises(ValueError):  # more than 1 element
        one([1, 2])


def test_one_memoryleak():
    one = iteration_utilities.one

    class Test(object):
        def __init__(self, value):
            self.value = value

    def test():
        one([Test(0)])
    assert not memory_leak(test)

    def test():
        one('a')
    assert not memory_leak(test)

    def test():
        one({Test(0): 10})
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            one(Test(0))
    assert not memory_leak(test)

    def test():
        with pytest_raises(ValueError):  # empty iterable
            one([])
    assert not memory_leak(test)

    def test():
        with pytest_raises(ValueError):  # more than 1 element
            one([1, 2])
    assert not memory_leak(test)


def test_nth():
    nth = iteration_utilities.nth

    assert nth([1, 2, 3], 1) == 2
    assert nth(range(10), 2) == 2

    # With pred
    assert nth([0, 1, 2], 1, pred=bool) == 2
    assert nth([0, 1, 2], 1, pred=None) == 2
    assert nth([0]*100 + [1], 0, pred=bool) == 1
    assert nth([[1], [1, 2], [1, 2]], 1, pred=lambda x: len(x) > 1) == [1, 2]

    # pred with truthy/retpred
    assert nth([0, 1, 2, 3, 0], 1, pred=bool, truthy=False) == 0
    assert nth([0, 1, 2, 3, 0], 1, pred=bool, truthy=False,
                retpred=True) == False
    assert nth([0, 1, 2, 3, 0], 1, pred=lambda x: x**2, truthy=False) == 0
    assert nth([0, 1, 2, 3, 0], 1,
               pred=lambda x: x**2, truthy=False, retpred=True) == 0
    assert nth([0, 1, 2, 3], 2, pred=bool) == 3
    assert nth([0, 1, 2, 3], 2, pred=bool, retpred=True) == True
    assert nth([0, 2, 3, 4], 2, pred=lambda x: x**2) == 4
    assert nth([0, 2, 3, 4], 2, pred=lambda x: x**2, retpred=True) == 16

    # With default
    assert nth([], 2, default=None) is None
    assert nth([0, 0, 0], 1, default=None, pred=bool) is None

    # failures
    with pytest.raises(TypeError):
        nth(100, 10)

    with pytest.raises(IndexError):
        nth([], 10)

    with pytest.raises(IndexError):
        nth([0], 1, pred=bool)

    with pytest.raises(TypeError):
        nth(['a', 'b'], 1, pred=abs)


def test_nth_memoryleak():
    nth = iteration_utilities.nth

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __bool__(self):
            return bool(self.value)

        def __nonzero__(self):
            return bool(self.value)

        def __pow__(self, other):
            return self.__class__(self.value ** other.value)

    def test():
        nth([Test(1), Test(2), Test(3)], 1)
    assert not memory_leak(test)

    # With pred
    def test():
        nth([Test(0), Test(1), Test(2)], 1, pred=bool)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(1), Test(2)], 1, pred=None)
    assert not memory_leak(test)

    def test():
        nth([Test(0)]*100 + [Test(1)]*2, 1, pred=bool)
    assert not memory_leak(test)

    def test():
        nth([[Test(0)], [Test(1), Test(2)]]*2, 1, pred=lambda x: len(x) > 1)
    assert not memory_leak(test)

    # pred with truthy/retpred
    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 1,
            pred=bool, truthy=False)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 1,
            pred=bool, truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 1,
            pred=lambda x: x**Test(2), truthy=False)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3), Test(0)], 0,
            pred=lambda x: x**Test(2), truthy=False, retpred=True)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1, pred=bool)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1, pred=bool, retpred=True)
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1, pred=lambda x: x**Test(2))
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(2), Test(3)], 1,
            pred=lambda x: x**Test(2), retpred=True)
    assert not memory_leak(test)

    # With default
    def test():
        nth([], 2, default=None) is None
    assert not memory_leak(test)

    def test():
        nth([Test(0), Test(0), Test(0)], 1, default=None, pred=bool) is None
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(IndexError):
            nth([], 10)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            nth(Test(100), 10)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            nth([Test('a'), Test('b')], 1, pred=lambda x: abs(x.value))
    assert not memory_leak(test)


def test_quantify():
    quantify = iteration_utilities.quantify

    assert quantify([]) == 0
    assert quantify([0, 0]) == 0
    assert quantify([0, 0, 1]) == 1
    assert quantify([0, 0, 1, 1], None) == 2

    assert quantify([], lambda x: x) == 0

    assert quantify([1, 2, 3], lambda x: x > 2) == 1
    assert quantify([1, 2, 3], lambda x: x < 3) == 2

    with pytest.raises(TypeError):
        quantify(1)

    with pytest.raises(TypeError):
        quantify([1], 1)


def test_quantify_memoryleak():
    quantify = iteration_utilities.quantify

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __lt__(self, other):
            return self.value < other.value

        def __gt__(self, other):
            return self.value > other.value

        def __bool__(self):
            return bool(self.value)

        def __nonzero__(self):
            return bool(self.value)

    def test():
        quantify([])
    assert not memory_leak(test)

    def test():
        quantify([Test(0), Test(0)])
    assert not memory_leak(test)

    def test():
        quantify([Test(0), Test(0), Test(1)])
    assert not memory_leak(test)

    def test():
        quantify([Test(0), Test(0), Test(1), Test(1)], None)
    assert not memory_leak(test)

    def test():
        quantify([], iteration_utilities.return_first_positional_argument)
    assert not memory_leak(test)

    def test():
        quantify([Test(1), Test(2), Test(3)], lambda x: x > Test(2))
    assert not memory_leak(test)

    def test():
        quantify([Test(1), Test(2), Test(3)], lambda x: x < Test(3))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            quantify(Test(1))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):
            quantify([Test(1)], Test(1))
    assert not memory_leak(test)


def test_unique_justseen():
    unique_justseen = iteration_utilities.unique_justseen

    assert list(unique_justseen([])) == []
    assert list(unique_justseen([1, 1, 2, 3, 3])) == [1, 2, 3]
    assert list(unique_justseen('aAabBb')) == ['a', 'A', 'a', 'b', 'B', 'b']
    assert list(unique_justseen('aAabBb', key=str.lower)) == ['a', 'b']

    with pytest.raises(TypeError):  # not iterable
        unique_justseen(10)

    with pytest.raises(TypeError):  # function call fails
        list(unique_justseen([1, 2, 3], key=lambda x: x + 'a'))

    class Test2(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            raise TypeError()

    with pytest.raises(TypeError):  # objects do not support eq or ne
        list(unique_justseen([Test2(1), Test2(2)]))

    class Test3(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            return self.value != other.value

    res = list(unique_justseen([Test3(1), Test3(1)]))
    assert len(res) == 1
    assert isinstance(res[0], Test3)
    assert res[0].value == 1


def test_unique_justseen_memoryleak():
    unique_justseen = iteration_utilities.unique_justseen

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            return self.value == other.value

    class Test2(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            raise TypeError()

    class Test3(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            raise TypeError()

        def __ne__(self, other):
            return self.value != other.value

    def test():
        list(unique_justseen([])) == []
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test(1), Test(1), Test(2), Test(3), Test(3)]))
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test(1), Test(-1), Test(1),
                              Test(2), Test(-2), Test(2)]))
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test(1), Test(-1), Test(1),
                              Test(2), Test(-2), Test(2)],
                             key=lambda x: abs(x.value)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # not iterable
            unique_justseen(Test(1))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # function call fails
            list(unique_justseen([Test(1), Test(2), Test(3)],
                                 key=lambda x: x + 'a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # objects do not support eq or ne
            list(unique_justseen([Test2(1), Test2(2)]))
    assert not memory_leak(test)

    def test():
        list(unique_justseen([Test3(1), Test3(1)]))
    assert not memory_leak(test)


def test_groupby2():
    groupby2 = iteration_utilities.groupby2

    assert groupby2([], key=lambda x: x) == {}
    assert groupby2(['a', 'ab', 'abc'],
                    key=operator.itemgetter(0)) == {'a': ['a', 'ab', 'abc']}
    assert groupby2(['a', 'ba', 'ab', 'abc', 'b'],
                    key=operator.itemgetter(0)) == {'a': ['a', 'ab', 'abc'],
                                                    'b': ['ba', 'b']}
    assert groupby2(['a', 'ba', 'ab', 'abc', 'b'],
                    key=operator.itemgetter(0),
                    keepkey=len) == {'a': [1, 2, 3], 'b': [2, 1]}

    with pytest.raises(TypeError):  # not iterable
        groupby2(1, key=len)

    with pytest.raises(TypeError):  # key func fails
        groupby2([1, 2, 3], key=lambda x: x + 'a')

    with pytest.raises(TypeError):  # keepkey func fails
        groupby2([1, 2, 3], key=lambda x: x, keepkey=lambda x: x + 'a')

    with pytest.raises(TypeError):  # unhashable
        groupby2([{'a': 10}], key=lambda x: x)


def test_groupby2_memoryleak():
    groupby2 = iteration_utilities.groupby2

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            return self.value == other.value

        def __hash__(self):
            return hash(self.value)

    def test():
        groupby2([], key=lambda x: x)
    assert not memory_leak(test)

    def test():
        groupby2([Test('a'), Test('ab'), Test('abc')],
                 key=lambda x: Test(x.value[0]))
    assert not memory_leak(test)

    def test():
        groupby2([Test('a'), Test('ba'), Test('ab'), Test('abc'), Test('b')],
                 key=lambda x: Test(x.value[0]))
    assert not memory_leak(test)

    def test():
        groupby2([Test('a'), Test('ba'), Test('ab'), Test('abc'), Test('b')],
                 key=lambda x: Test(x.value[0]),
                 keepkey=lambda x: Test(len(x.value)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # not iterable
            groupby2(Test(1), key=lambda x: Test(len(x.value)))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # key func fails
            groupby2([Test(1), Test(2), Test(3)],
                     key=lambda x: Test(x.value + 'a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # keepkey func fails
            groupby2([Test(1), Test(2), Test(3)],
                     key=lambda x: x, keepkey=lambda x: Test(x.value + 'a'))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # unhashable
            groupby2([{Test('a'): Test(10)}], key=lambda x: x)
    assert not memory_leak(test)


def test_alldistinct():
    all_distinct = iteration_utilities.all_distinct

    assert all_distinct([1, 2, 3])
    assert not all_distinct([1, 1, 1])

    assert all_distinct([{'a': 1}, {'a': 2}])
    assert not all_distinct([{'a': 1}, {'a': 1}])

    with pytest.raises(TypeError):  # iterable is not iterable
        all_distinct(1)


def test_alldistinct_memoryleak():
    all_distinct = iteration_utilities.all_distinct

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            return self.value == other.value

        def __hash__(self):
            return hash(self.value)

    def test():
        all_distinct([Test(1), Test(2), Test(3)])
    assert not memory_leak(test)

    def test():
        all_distinct([Test(1), Test(1), Test(1)])
    assert not memory_leak(test)

    def test():
        all_distinct([{Test('a'): Test(1)}, {Test('a'): Test(2)}])
    assert not memory_leak(test)

    def test():
        all_distinct([{Test('a'): Test(1)}, {Test('a'): Test(1)}])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # iterable is not iterable
            all_distinct(Test(1))
    assert not memory_leak(test)


def test_all_equal():
    all_equal = iteration_utilities.all_equal

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    assert all_equal([])
    assert all_equal([1, 1, 1])
    assert not all_equal([1, 1, 2])

    with pytest.raises(TypeError):  # not iterable
        all_equal(1)

    if not iteration_utilities.PY2:
        with pytest.raises(TypeError):  # comparison fail
            all_equal([Test(1), Test('a')])


def test_all_equal_memoryleak():
    all_equal = iteration_utilities.all_equal

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    def test():
        all_equal([])
    assert not memory_leak(test)

    def test():
        all_equal([Test(1), Test(1), Test(1)])
    assert not memory_leak(test)

    def test():
        all_equal([Test(1), Test(1), Test(2)])
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # not iterable
            all_equal(Test(1))
    assert not memory_leak(test)

    if not iteration_utilities.PY2:
        def test():
            with pytest_raises(TypeError):  # comparison fail
                all_equal([Test(1), Test('a')])
        assert not memory_leak(test)


def test_split():
    split = iteration_utilities.split

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    assert list(split([], lambda x: False)) == []

    assert list(split([1, 2, 3], lambda x: x == 2)) == [[1], [3]]
    assert list(split([1, 2, 3], lambda x: x == 3)) == [[1, 2]]

    # keep
    assert list(split([1, 2, 3], lambda x: x == 2,
                      keep=True)) == [[1], [2], [3]]
    assert list(split([1, 2, 3], lambda x: x == 3,
                      keep=True)) == [[1, 2], [3]]

    # maxsplit
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=1)) == [[1], [3, 4, 5]]
    assert list(split([1, 2, 3, 4, 5], lambda x: x % 2 == 0,
                      maxsplit=2)) == [[1], [3], [5]]

    # equality
    assert list(split([1, 2, 3, 2, 5], 2,
                      eq=True)) == [[1], [3], [5]]

    # failures
    with pytest.raises(TypeError):  # not iterable
        split(1, lambda x: False)
    with pytest.raises(TypeError):  # func fails
        list(split([1, 2, 3], lambda x: x + 'a'))
    with pytest.raises(TypeError):  # cmp fails
        list(split([Test(1), Test(2), Test(3)], Test('a'), eq=True))


def test_split_memoryleak():
    split = iteration_utilities.split

    class Test(object):
        def __init__(self, value):
            self.value = value

        def __eq__(self, other):
            if type(self.value) != type(other.value):
                raise TypeError('simulated failure.')
            return self.value == other.value

    def test():
        list(split([], lambda x: False)) == []
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 2))
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 3))
    assert not memory_leak(test)

    # keep
    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 2,
                   keep=True))
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3)], lambda x: x.value == 3,
                   keep=True))
    assert not memory_leak(test)

    # maxsplit
    def test():
        list(split([Test(1), Test(2), Test(3), Test(4), Test(5)],
                   lambda x: x.value % 2 == 0, maxsplit=1))
    assert not memory_leak(test)

    def test():
        list(split([Test(1), Test(2), Test(3), Test(4), Test(5)],
                   lambda x: x.value % 2 == 0, maxsplit=2))
    assert not memory_leak(test)

    # equality
    def test():
        list(split([Test(1), Test(2), Test(3), Test(2), Test(5)], Test(2),
                   eq=True))
    assert not memory_leak(test)

    # failures
    def test():
        with pytest_raises(TypeError):  # not iterable
            split(Test(1), lambda x: False)
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # func fails
            list(split([Test(1), Test(2), Test(3)],
                       lambda x: Test(x.value + 'a')))
    assert not memory_leak(test)

    def test():
        with pytest_raises(TypeError):  # cmp fails
            list(split([Test(1), Test(2), Test(3)], Test('a'), eq=True))
    assert not memory_leak(test)


@pytest.mark.xfail(iteration_utilities.PY2,
                   reason='Python 2 does not support this way of pickling.')
def test_cfuncs_pickle():

    grouper = iteration_utilities.grouper
    intersperse = iteration_utilities.intersperse
    merge = iteration_utilities.merge
    unique_justseen = iteration_utilities.unique_justseen
    roundrobin = iteration_utilities.roundrobin
    complement = iteration_utilities.complement
    compose = iteration_utilities.compose

    # IMPORTANT: methoddescriptors like "str.lower" as key functions can not
    #            be pickled before python 3.4

    # ----- Roundrobin
    rr = roundrobin([1, 2, 3], [1, 2, 3])
    assert next(rr) == 1
    x = pickle.dumps(rr)
    assert list(pickle.loads(x)) == [1, 2, 2, 3, 3]

    rr2 = roundrobin([1], [1, 2, 3])
    assert next(rr2) == 1
    assert next(rr2) == 1
    assert next(rr2) == 2
    x = pickle.dumps(rr2)
    assert list(pickle.loads(x)) == [3]

    # ----- Merge
    mge = merge([0], [1, 2], [2])
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, 2]

    mge = merge([1, 2], [0], [-2], key=abs)
    assert next(mge) == 0
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [1, 2, -2]

    mge = merge([2, 1], [0], [3], reverse=True)
    assert next(mge) == 3
    x = pickle.dumps(mge)
    assert list(pickle.loads(x)) == [2, 1, 0]

    # ----- Grouper
    grp = grouper(range(10), 3)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9,)]

    grp = grouper(range(10), 3, fillvalue=0)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8), (9, 0, 0)]

    grp = grouper(range(10), 3, truncate=True)
    assert next(grp) == (0, 1, 2)
    x = pickle.dumps(grp)
    assert list(pickle.loads(x)) == [(3, 4, 5), (6, 7, 8)]

    # ----- Intersperse
    its = intersperse([1, 2, 3], 0)
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [1, 0, 2, 0, 3]

    its = intersperse([1, 2, 3], 0)  # start value must be set!
    assert next(its) == 1
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 2, 0, 3]

    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [2, 0, 3]

    its = intersperse([1, 2, 3], 0)
    assert next(its) == 1
    assert next(its) == 0
    assert next(its) == 2
    x = pickle.dumps(its)
    assert list(pickle.loads(x)) == [0, 3]

    # ----- Complement
    x = pickle.dumps(complement(iteration_utilities.is_None))
    assert pickle.loads(x)(False)
    assert pickle.loads(x)(True)
    assert not pickle.loads(x)(None)

    # ----- Unique_justseen
    ujs = unique_justseen([1, 2, 3])
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [1, 2, 3]

    ujs = unique_justseen([1, 2, 3])
    assert next(ujs) == 1
    x = pickle.dumps(ujs)
    assert list(pickle.loads(x)) == [2, 3]

    # Pickling a method descriptor is not possible for Python 3.3 and before
    # Also operator.methodcaller loses it's methodname when pickled for Python
    #   3.4 and lower...
    if iteration_utilities.PY34:
        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        x = pickle.dumps(ujs)
        assert list(pickle.loads(x)) == ['a']

        ujs = unique_justseen(['a', 'A', 'a'], key=str.lower)
        assert next(ujs) == 'a'
        x = pickle.dumps(ujs)
        assert list(pickle.loads(x)) == []

    # ----- Compose

    cmp = compose(iteration_utilities.square, iteration_utilities.one_over)
    x = pickle.dumps(cmp)
    assert pickle.loads(x)(10) == 1/100
    assert pickle.loads(x)(2) == 1/4


def test_callbacks():
    assert iteration_utilities.return_True()
    assert not iteration_utilities.return_False()
    assert iteration_utilities.return_None() is None
    assert iteration_utilities.return_identity(1) == 1
    assert iteration_utilities.return_first_positional_argument(1, 2, 3) == 1
    assert iteration_utilities.return_called(int) == 0

    assert iteration_utilities.square(2) == 4
    assert iteration_utilities.one_over(2) == 0.5

    assert iteration_utilities.is_None(None)
    assert not iteration_utilities.is_None(False)

    assert not iteration_utilities.is_not_None(None)
    assert iteration_utilities.is_not_None(False)

    assert iteration_utilities.is_even(2)
    assert not iteration_utilities.is_even(1)

    assert iteration_utilities.is_odd(1)
    assert not iteration_utilities.is_odd(2)

    assert not iteration_utilities.is_iterable(1)
    assert iteration_utilities.is_iterable([1])
