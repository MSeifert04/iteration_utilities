# Licensed under Apache License Version 2.0 - see LICENSE

import collections
import copy
import pickle
import sys
import weakref

import pytest

import iteration_utilities
from iteration_utilities import partial

import helper_funcs as _hf
from helper_cls import T, toT


# =============================================================================
# These tests are taken from the python tests.
#
# They were changed from unitests to pytest and made py2 and py3 compatible.
# =============================================================================


def capture(*args, **kw):
    """capture all positional and keyword arguments"""
    return args, kw


def signature(part):
    """ return the signature of a partial object """
    return (part.func, part.args, part.keywords, part.__dict__)


class AllowPickle:
    def __enter__(self):
        return self

    def __exit__(self, typ, value, tb):
        return False


class MyTuple(tuple):
    pass


class BadTuple(tuple):
    def __add__(self, other):
        return list(self) + list(other)


class MyDict(dict):
    pass


class MyStr(str):
    pass


def test_attributes_unwritable():
    p = partial(capture, T(1), T(2), a=T(10), b=T(20))
    with pytest.raises(AttributeError):
        p.func = map
    with pytest.raises(AttributeError):
        p.args = (T(1), T(2))
    with pytest.raises(AttributeError):
        p.keywords = {'a': T(1), 'b': T(2)}

    p = partial(hex)
    with pytest.raises(TypeError):
        del p.__dict__


@_hf.skip_on_pypy_not_investigated_why
def test_recursive_pickle():

    with AllowPickle():
        f = partial(capture)
        f.__setstate__((f, (), {}, {}))
        try:
            for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                with pytest.raises(RecursionError):
                    pickle.dumps(f, proto)
        finally:
            f.__setstate__((capture, (), {}, {}))

        f = partial(capture)
        f.__setstate__((capture, (f,), {}, {}))
        try:
            for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                f_copy = pickle.loads(pickle.dumps(f, proto))
                try:
                    assert f_copy.args[0] is f_copy
                finally:
                    f_copy.__setstate__((capture, (), {}, {}))
        finally:
            f.__setstate__((capture, (), {}, {}))

        f = partial(capture)
        f.__setstate__((capture, (), {'a': f}, {}))
        try:
            for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                f_copy = pickle.loads(pickle.dumps(f, proto))
                try:
                    assert f_copy.keywords['a'] is f_copy
                finally:
                    f_copy.__setstate__((capture, (), {}, {}))
        finally:
            f.__setstate__((capture, (), {}, {}))


def test_repr():
    args = (object(), object())
    args_repr = ', '.join(repr(a) for a in args)
    kwargs = {'a': object(), 'b': object()}
    kwargs_reprs = ['a={a!r}, b={b!r}'.format(**kwargs),
                    'b={b!r}, a={a!r}'.format(**kwargs)]
    name = 'iteration_utilities.partial'

    f = partial(capture)
    compare = '{name}({capture!r})'.format(name=name, capture=capture)
    assert compare == repr(f)

    f = partial(capture, *args)
    compare = ('{name}({capture!r}, {args_repr})'
               ''.format(name=name, capture=capture, args_repr=args_repr))
    assert compare == repr(f)

    f = partial(capture, **kwargs)
    compare = ['{name}({capture!r}, {kwargs_repr})'
               ''.format(name=name, capture=capture, kwargs_repr=kwargs_repr)
               for kwargs_repr in kwargs_reprs]
    assert repr(f) in compare

    f = partial(capture, *args, **kwargs)
    compare = ['{name}({capture!r}, {args_repr}, {kwargs_repr})'
               ''.format(name=name, capture=capture,
                         args_repr=args_repr, kwargs_repr=kwargs_repr)
               for kwargs_repr in kwargs_reprs]
    assert repr(f) in compare


def test_basic_examples():
    p = partial(capture, T(1), T(2), a=T(10), b=T(20))
    assert callable(p)
    assert p(T(3), T(4), b=T(30), c=T(40)) == ((T(1), T(2), T(3), T(4)),
                                               dict(a=T(10), b=T(30), c=T(40)))
    p = partial(map, lambda x: x*T(10))
    assert list(p([T(1), T(2), T(3), T(4)])) == toT([10, 20, 30, 40])


def test_attributes():
    p = partial(capture, T(1), T(2), a=T(10), b=T(20))
    # attributes should be readable
    assert p.func == capture
    assert p.args == (T(1), T(2))
    assert p.keywords == dict(a=T(10), b=T(20))


def test_argument_checking():
    # at least one argument
    with pytest.raises(TypeError):
        partial()

    # must be callable
    with pytest.raises(TypeError):
        partial(T(2))


def test_protection_of_callers_dict_argument():
    # a caller's dictionary should not be altered by partial
    def func(a=10, b=20):
        return a
    d = {'a': T(3)}
    p = partial(func, a=T(5))
    assert p(**d) == T(3)
    assert d == {'a': T(3)}
    p(b=7)
    assert d == {'a': T(3)}


def test_kwargs_copy():
    # Issue #29532: Altering a kwarg dictionary passed to a constructor
    # should not affect a partial object after creation
    d = {'a': T(3)}
    p = partial(capture, **d)
    assert p() == ((), {'a': T(3)})
    d['a'] = T(5)
    assert p(), ((), {'a': T(3)})


def test_arg_combinations():
    # exercise special code paths for zero args in either partial
    # object or the caller
    p = partial(capture)
    assert p() == ((), {})
    assert p(T(1), T(2)) == ((T(1), T(2)), {})
    p = partial(capture, T(1), T(2))
    assert p() == ((T(1), T(2)), {})
    assert p(T(3), T(4)) == ((T(1), T(2), T(3), T(4)), {})


def test_kw_combinations():
    # exercise special code paths for no keyword args in
    # either the partial object or the caller
    p = partial(capture)
    assert p.keywords == {}
    assert p() == ((), {})
    assert p(a=T(1)) == ((), {'a': T(1)})
    p = partial(capture, a=T(1))
    assert p.keywords == {'a': T(1)}
    assert p() == ((), {'a': T(1)})
    assert p(b=T(2)) == ((), {'a': T(1), 'b': T(2)})
    # keyword args in the call override those in the partial object
    assert p(a=T(3), b=T(2)) == ((), {'a': T(3), 'b': T(2)})


def test_positional():
    # make sure positional arguments are captured correctly
    for args in [(), (T(0),), (T(0), T(1)),
                 (T(0), T(1), T(2)), (T(0), T(1), T(2), T(3))]:
        p = partial(capture, *args)
        expected = args + (T('x'),)
        got, empty = p(T('x'))
        assert expected == got and empty == {}


def test_keyword():
    # make sure keyword arguments are captured correctly
    for a in [T('a'), T(0), T(None), T(3.5)]:
        p = partial(capture, a=T(a))
        expected = {'a': T(a), 'x': T(None)}
        empty, got = p(x=T(None))
        assert expected == got and empty == ()


def test_no_side_effects():
    # make sure there are no side effects that affect subsequent calls
    p = partial(capture, T(0), a=T(1))
    args1, kw1 = p(T(1), b=T(2))
    assert args1 == (T(0), T(1)) and kw1 == {'a': T(1), 'b': T(2)}
    args2, kw2 = p()
    assert args2 == (T(0),) and kw2 == {'a': T(1)}


def test_error_propagation():
    def f(x, y):
        x / y
    with pytest.raises(ZeroDivisionError):
        partial(f, 1, 0)()
    with pytest.raises(ZeroDivisionError):
        partial(f, 1)(0)
    with pytest.raises(ZeroDivisionError):
        partial(f)(1, 0)
    with pytest.raises(ZeroDivisionError):
        partial(f, y=0)(1)


@_hf.skip_on_pypy_not_investigated_why
def test_weakref():
    f = partial(int, base=16)
    p = weakref.proxy(f)
    assert f.func == p.func
    f = None
    with pytest.raises(ReferenceError):
        p.func


def test_with_bound_and_unbound_methods():
    data = list(map(str, range(10)))
    join = partial(str.join, '')
    assert join(data) == '0123456789'
    join = partial(''.join)
    assert join(data) == '0123456789'


def test_nested_optimization():
    inner = partial(signature, 'asdf')
    nested = partial(inner, bar=True)
    flat = partial(signature, 'asdf', bar=True)
    assert signature(nested) == signature(flat)


def test_nested_partial_with_attribute():
    # see issue 25137

    def foo(bar):
        return bar

    p = partial(foo, 'first')
    p2 = partial(p, 'second')
    p2.new_attr = 'spam'
    assert p2.new_attr == 'spam'


def test_recursive_repr():
    name = 'iteration_utilities.partial'

    f = partial(capture)
    f.__setstate__((f, (), {}, {}))
    try:
        assert repr(f) == '{}(...)'.format(name)
    finally:
        f.__setstate__((capture, (), {}, {}))

    f = partial(capture)
    f.__setstate__((capture, (f,), {}, {}))
    try:
        assert repr(f) == '{}({!r}, ...)'.format(name, capture)
    finally:
        f.__setstate__((capture, (), {}, {}))

    f = partial(capture)
    f.__setstate__((capture, (), {'a': f}, {}))
    try:
        assert repr(f) == '{}({!r}, a=...)'.format(name, capture)
    finally:
        f.__setstate__((capture, (), {}, {}))


def test_pickle():
    with AllowPickle():
        f = partial(signature, ['asdf'], bar=[True])
        f.attr = []
        for proto in range(pickle.HIGHEST_PROTOCOL + 1):
            f_copy = pickle.loads(pickle.dumps(f, proto))
            assert signature(f_copy) == signature(f)


@_hf.skip_on_pypy_not_investigated_why
def test_copy():
    f = partial(signature, ['asdf'], bar=[True])
    f.attr = []
    f_copy = copy.copy(f)
    assert signature(f_copy) == signature(f)
    assert f_copy.attr is f.attr
    assert f_copy.args is f.args
    assert f_copy.keywords is f.keywords


@_hf.skip_on_pypy_not_investigated_why
def test_deepcopy():
    f = partial(signature, ['asdf'], bar=[True])
    f.attr = []
    f_copy = copy.deepcopy(f)
    assert signature(f_copy) == signature(f)
    assert f_copy.attr is not f.attr
    assert f_copy.args is not f.args
    assert f_copy.args[0] is not f.args[0]
    assert f_copy.keywords is not f.keywords
    assert f_copy.keywords['bar'] is not f.keywords['bar']


def test_setstate():
    f = partial(signature)
    f.__setstate__((capture, (1,), dict(a=10), dict(attr=[])))

    assert signature(f) == (capture, (1,), dict(a=10), dict(attr=[]))
    assert f(2, b=20) == ((1, 2), {'a': 10, 'b': 20})

    f.__setstate__((capture, (1,), dict(a=10), None))

    assert signature(f) == (capture, (1,), dict(a=10), {})
    assert f(2, b=20) == ((1, 2), {'a': 10, 'b': 20})

    f.__setstate__((capture, (1,), None, None))
    # self.assertEqual(signature(f), (capture, (1,), {}, {}))
    assert f(2, b=20) == ((1, 2), {'b': 20})
    assert f(2) == ((1, 2), {})
    assert f() == ((1,), {})

    f.__setstate__((capture, (), {}, None))
    assert signature(f) == (capture, (), {}, {})
    assert f(2, b=20) == ((2,), {'b': 20})
    assert f(2) == ((2,), {})
    assert f() == ((), {})


def test_setstate_errors():
    f = partial(signature)
    with pytest.raises(TypeError):
        f.__setstate__((capture, (), {}))
    with pytest.raises(TypeError):
        f.__setstate__((capture, (), {}, {}, None))
    with pytest.raises(TypeError):
        f.__setstate__([capture, (), {}, None])
    with pytest.raises(TypeError):
        f.__setstate__((None, (), {}, None))
    with pytest.raises(TypeError):
        f.__setstate__((capture, None, {}, None))
    with pytest.raises(TypeError):
        f.__setstate__((capture, [], {}, None))
    with pytest.raises(TypeError):
        f.__setstate__((capture, (), [], None))


def test_setstate_subclasses():
    f = partial(signature)
    f.__setstate__((capture, MyTuple((1,)), MyDict(a=10), None))
    s = signature(f)
    assert s == (capture, (1,), dict(a=10), {})
    assert type(s[1]) is tuple
    assert type(s[2]) is dict
    r = f()
    assert r == ((1,), {'a': 10})
    assert type(r[0]) is tuple
    assert type(r[1]) is dict

    f.__setstate__((capture, BadTuple((1,)), {}, None))
    s = signature(f)
    assert s == (capture, (1,), {}, {})
    assert type(s[1]) is tuple
    r = f(2)
    assert r == ((1, 2), {})
    assert type(r[0]) is tuple


def test_setstate_refcount():
    # Issue 6083: Reference counting bug
    class BadSequence:
        def __len__(self):
            return 4

        def __getitem__(self, key):
            if key == 0:
                return max
            elif key == 1:
                return tuple(range(1000000))
            elif key in (2, 3):
                return {}
            raise IndexError

    f = partial(object)
    with pytest.raises(TypeError):
        f.__setstate__(BadSequence())


# =============================================================================
# New tests (not taken from CPython source)
# =============================================================================


def test_invalid_kwargs_with_setstate():
    f = partial(object)
    f.__setstate__((object, (), {1: 1}, {}))
    # Shouldn't segfault!!!
    with pytest.raises(TypeError):
        repr(f)


def test_partial_from_partial_kwargs():
    f1 = partial(capture, b=10)
    f2 = partial(f1)
    assert f2() == ((), {'b': 10})
    assert signature(f2) == (capture, (), {'b': 10}, {})

    f1 = partial(capture, b=10)
    f2 = partial(f1, c=10)
    assert f2() == ((), {'b': 10, 'c': 10})
    assert signature(f2) == (capture, (), {'b': 10, 'c': 10}, {})

    f1 = partial(capture, b=10)
    f2 = partial(f1, b=20)
    assert f2() == ((), {'b': 20})
    assert signature(f2) == (capture, (), {'b': 20}, {})


def test_partial_dict_setter():
    p = partial(capture, b=10)
    with pytest.raises(TypeError):
        p.__dict__ = 10

    p = partial(capture, b=10)
    p.__dict__ = {}
    assert signature(p) == (capture, (), {'b': 10}, {})

    p = partial(capture, b=10)
    p.__dict__ = collections.OrderedDict()
    assert signature(p) == (capture, (), {'b': 10}, collections.OrderedDict())
    assert isinstance(p.__dict__, collections.OrderedDict)


def test_partial_has_placeholder():
    assert hasattr(partial, '_')


def test_partial_placeholder_basic():
    p = partial(isinstance, partial._, int)
    assert p.num_placeholders == 1
    assert p.args == (partial._, int)
    assert p(20)
    assert not p(1.2)
    assert not p(T(1.2))


def test_partial_placeholder_someone_holds_ref():
    p = partial(isinstance, partial._, int)
    # hold a reference to args while calling the function
    x = p.args
    assert p(20)
    assert not p(1.2)
    assert not p(T(1.2))
    del x


def test_partial_placeholder_copy():
    p = partial(isinstance, partial._, int)
    # call a copy of a partial with placeholders
    p2 = copy.copy(p)
    assert p2.num_placeholders == 1
    assert p2(20)
    assert not p2(1.2)
    assert not p2(T(1.2))


@_hf.skip_on_pypy_because_sizeof_makes_no_sense_there
def test_partial_sizeof():
    p1 = partial(isinstance, 10, int)
    p2 = partial(isinstance, partial._, int)
    p3 = partial(isinstance, partial._, partial._)
    # The sizes should be different because each placeholder leads to one more
    # element in the posph array.
    sizes = [sys.getsizeof(p) for p in (p1, p2, p3)]
    assert sizes[2] > sizes[1]
    assert sizes[1] > sizes[0]
    # Also make sure that the difference is the same between 3 vs. 2 and 2 vs. 1
    assert sizes[2] - sizes[1] == sizes[1] - sizes[0]


def test_partial_placeholder_deepcopy():
    p = partial(isinstance, partial._, int)
    p2 = copy.deepcopy(p)
    assert p2.num_placeholders == 1
    assert p2(20)
    assert not p2(1.2)
    assert not p2(T(1.2))


def test_partial_placeholder_setstate_frees_old_array():
    p = partial(isinstance, partial._, int)
    p.__setstate__((isinstance, (10, int), {}, {}))
    # TODO: How to check the memory is freed? :(


def test_partial_placeholder_missing_args():
    p = partial(isinstance, partial._, int)

    with pytest.raises(TypeError, match='not enough values'):
        p()

    # partial with multiple placeholders and too many or too few arguments
    p = partial(isinstance, partial._, partial._)
    assert p.num_placeholders == 2

    with pytest.raises(TypeError, match='not enough values'):
        p()

    with pytest.raises(TypeError, match='not enough values'):
        p(T(1))


def test_partial_placeholder_more_args():
    p = partial(capture, partial._, T(2))
    assert p(T(1), T(3), T(4)) == ((T(1), T(2), T(3), T(4)), {})


def test_partial_from_partial_with_one_placeholder():
    # One placeholder, no additional arguments, placeholder in different
    # positions
    p1 = partial(capture, partial._, T(2), T(3))
    p2 = partial(p1)
    assert p1.args is p2.args
    assert p1.keywords == p2.keywords
    assert p1(T(1)) == ((T(1), T(2), T(3)), {})
    assert p2(T(1)) == ((T(1), T(2), T(3)), {})
    assert p1(T(1)) == p2(T(1))

    p1 = partial(capture, T(1), partial._, T(3))
    p2 = partial(p1)
    assert p1.args is p2.args
    assert p1.keywords == p2.keywords
    assert p1(T(2)) == ((T(1), T(2), T(3)), {})
    assert p2(T(2)) == ((T(1), T(2), T(3)), {})
    assert p1(T(2)) == p2(T(2))

    p1 = partial(capture, T(1), T(2), partial._)
    p2 = partial(p1)
    assert p1.args is p2.args
    assert p1.keywords == p2.keywords
    assert p1(T(3)) == ((T(1), T(2), T(3)), {})
    assert p2(T(3)) == ((T(1), T(2), T(3)), {})
    assert p1(T(3)) == p2(T(3))


def test_partial_from_partial_with_one_placeholder_fail():
    p1 = partial(capture, partial._, T(2), T(3))
    p2 = partial(p1)
    with pytest.raises(TypeError, match='not enough values'):
        p2()


def test_partial_from_partial_basic1():
    # One placeholder, one argument given
    p1 = partial(capture, partial._, T(2), T(3))
    p2 = partial(p1, T(1))
    assert p1.args == (partial._, T(2), T(3))
    assert p1(T(1)) == ((T(1), T(2), T(3)), {})
    assert p1(T(1), T(4)) == ((T(1), T(2), T(3), T(4)), {})
    assert p2.args == (T(1), T(2), T(3))
    assert p2() == ((T(1), T(2), T(3)), {})
    assert p2(T(4)) == ((T(1), T(2), T(3), T(4)), {})


def test_partial_from_partial_basic2():
    # Two placeholders, one argument given
    p1 = partial(capture, partial._, T(2), partial._)
    p2 = partial(p1, T(1))
    assert p1.args == (partial._, T(2), partial._)
    assert p1(T(1), T(3)) == ((T(1), T(2), T(3)), {})
    assert p1(T(1), T(3), T(4)) == ((T(1), T(2), T(3), T(4)), {})
    assert p2.args == (T(1), T(2), partial._)
    assert p2(T(3)) == ((T(1), T(2), T(3)), {})
    assert p2(T(3), T(4)) == ((T(1), T(2), T(3), T(4)), {})


def test_partial_from_partial_basic3():
    # One placeholders, two arguments given
    p1 = partial(capture, partial._, T(2))
    p2 = partial(p1, T(1), T(3))
    assert p1.args == (partial._, T(2))
    assert p1(T(1)) == ((T(1), T(2)), {})
    assert p1(T(1), T(3)) == ((T(1), T(2), T(3)), {})
    assert p1(T(1), T(3), T(4)) == ((T(1), T(2), T(3), T(4)), {})
    assert p2.args == (T(1), T(2), T(3))
    assert p2() == ((T(1), T(2), T(3)), {})
    assert p2(T(4)) == ((T(1), T(2), T(3), T(4)), {})


def test_partial_from_partial_basic4():
    # Two placeholders, two arguments given
    p1 = partial(capture, partial._, partial._, T(3))
    p2 = partial(p1, T(1), T(2))
    assert p1.args == (partial._, partial._, T(3))
    assert p1(T(1), T(2)) == ((T(1), T(2), T(3)), {})
    assert p1(T(1), T(2), T(4)) == ((T(1), T(2), T(3), T(4)), {})
    assert p2.args == (T(1), T(2), T(3))
    assert p2() == ((T(1), T(2), T(3)), {})
    assert p2(T(4)) == ((T(1), T(2), T(3), T(4)), {})


def test_partial_from_partial_basic5():
    # Two placeholders, three arguments given
    p1 = partial(capture, partial._, partial._, T(3))
    p2 = partial(p1, T(1), T(2), T(4))
    assert p1.args == (partial._, partial._, T(3))
    assert p1(T(1), T(2)) == ((T(1), T(2), T(3)), {})
    assert p1(T(1), T(2), T(4)) == ((T(1), T(2), T(3), T(4)), {})
    assert p2.args == (T(1), T(2), T(3), T(4))
    assert p2() == ((T(1), T(2), T(3), T(4)), {})
    assert p2(T(5)) == ((T(1), T(2), T(3), T(4), T(5)), {})


def test_partial_with_function_that_keeps_args():
    # A function that keeps its args as-is was a problem with partial because
    # it reused the arguments. chained is such a function (currently).
    chained = iteration_utilities.chained
    assert partial(chained, partial._, str)(complex)(10) == '(10+0j)'


@_hf.skip_if_vectorcall_is_not_used
def test_partial_with_str_subclasses_fails1():
    p = partial(capture, **{MyStr('a'): 10})
    with pytest.raises(TypeError):
        p()


@_hf.skip_if_vectorcall_is_not_used
def test_partial_with_str_subclasses_fails2():
    p = partial(capture, **{MyStr('a'): 10})
    with pytest.raises(TypeError):
        p(b=20)


@_hf.skip_if_vectorcall_is_not_used
def test_partial_with_str_subclasses_fails3():
    p = partial(capture)
    with pytest.raises(TypeError):
        p(**{MyStr('a'): 10})


def test_partial_with_lots_of_kwargs():
    """The purpose of this test is to test the vectorcall implementation which
    converts the kwargs passed to the call to a set to speed-up the lookup
    behavior.
    """
    p = partial(capture, a=1, b=2, c=3, d=4, e=5, f=6, g=7, h=8, i=9, j=10, k=11)
    r = p(l=12, m=13, n=14, o=15, p=16, q=17, r=18, s=19, t=20, u=21, v=22, w=23)
    assert r == (tuple(), dict(zip('abcdefghijklmnopqrstuvw', range(1, 24))))


def test_partial_with_lots_of_kwargs_with_duplicate():
    p = partial(capture, a=1, b=2, c=3, d=4, e=5, f=6, g=7, h=8, i=9, j=10, k=11)
    r = p(a=12, b=13, c=14, d=15, e=16, f=17, g=18, h=19, i=20, j=21, k=22, l=23)
    assert r == (tuple(), dict(zip('abcdefghijkl', range(12, 24))))
