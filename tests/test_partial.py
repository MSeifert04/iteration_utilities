# Built-ins
from __future__ import absolute_import, division, print_function
import collections
import copy
import pickle
import weakref

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, toT


partial = iteration_utilities.partial


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

    def __exit__(self, type, value, tb):
        return False


class MyTuple(tuple):
    pass


class BadTuple(tuple):
    def __add__(self, other):
        return list(self) + list(other)


class MyDict(dict):
    pass


@memory_leak_decorator(collect=True)
def test_attributes_unwritable():
    p = partial(capture, T(1), T(2), a=T(10), b=T(20))
    expectedexc = TypeError if iteration_utilities.EQ_PY2 else AttributeError
    with pytest.raises(expectedexc):
        p.func = map
    with pytest.raises(expectedexc):
        p.args = (T(1), T(2))
    with pytest.raises(expectedexc):
        p.keywords = {'a': T(1), 'b': T(2)}

    p = partial(hex)
    with pytest.raises(TypeError):
        del p.__dict__


@memory_leak_decorator(offset=1)
def test_recursive_pickle():
    if iteration_utilities.GE_PY35:
        expectedexc = RecursionError
    else:
        expectedexc = RuntimeError

    with AllowPickle():
        f = partial(capture)
        f.__setstate__((f, (), {}, {}))
        try:
            for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                with pytest.raises(expectedexc):
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


@memory_leak_decorator()
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


@memory_leak_decorator()
def test_basic_examples():
    p = partial(capture, T(1), T(2), a=T(10), b=T(20))
    assert callable(p)
    assert p(T(3), T(4), b=T(30), c=T(40)) == ((T(1), T(2), T(3), T(4)),
                                               dict(a=T(10), b=T(30), c=T(40)))
    p = partial(map, lambda x: x*T(10))
    assert list(p([T(1), T(2), T(3), T(4)])) == toT([10, 20, 30, 40])


@memory_leak_decorator()
def test_attributes():
    p = partial(capture, T(1), T(2), a=T(10), b=T(20))
    # attributes should be readable
    assert p.func == capture
    assert p.args == (T(1), T(2))
    assert p.keywords == dict(a=T(10), b=T(20))


@memory_leak_decorator()
def test_argument_checking():
    # at least one argument
    with pytest.raises(TypeError):
        partial()

    # must be callable
    with pytest.raises(TypeError):
        partial(T(2))


@memory_leak_decorator()
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


@memory_leak_decorator()
def test_kwargs_copy():
    # Issue #29532: Altering a kwarg dictionary passed to a constructor
    # should not affect a partial object after creation
    d = {'a': T(3)}
    p = partial(capture, **d)
    assert p() == ((), {'a': T(3)})
    d['a'] = T(5)
    assert p(), ((), {'a': T(3)})


@memory_leak_decorator()
def test_arg_combinations():
    # exercise special code paths for zero args in either partial
    # object or the caller
    p = partial(capture)
    assert p() == ((), {})
    assert p(T(1), T(2)) == ((T(1), T(2)), {})
    p = partial(capture, T(1), T(2))
    assert p() == ((T(1), T(2)), {})
    assert p(T(3), T(4)) == ((T(1), T(2), T(3), T(4)), {})


@memory_leak_decorator()
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


@memory_leak_decorator()
def test_positional():
    # make sure positional arguments are captured correctly
    for args in [(), (T(0),), (T(0), T(1)),
                 (T(0), T(1), T(2)), (T(0), T(1), T(2), T(3))]:
        p = partial(capture, *args)
        expected = args + (T('x'),)
        got, empty = p(T('x'))
        assert expected == got and empty == {}


@memory_leak_decorator()
def test_keyword():
    # make sure keyword arguments are captured correctly
    for a in [T('a'), T(0), T(None), T(3.5)]:
        p = partial(capture, a=T(a))
        expected = {'a': T(a), 'x': T(None)}
        empty, got = p(x=T(None))
        assert expected == got and empty == ()


@memory_leak_decorator()
def test_no_side_effects():
    # make sure there are no side effects that affect subsequent calls
    p = partial(capture, T(0), a=T(1))
    args1, kw1 = p(T(1), b=T(2))
    assert args1 == (T(0), T(1)) and kw1 == {'a': T(1), 'b': T(2)}
    args2, kw2 = p()
    assert args2 == (T(0),) and kw2 == {'a': T(1)}


@memory_leak_decorator()
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


@memory_leak_decorator()
def test_weakref():
    f = partial(int, base=16)
    p = weakref.proxy(f)
    assert f.func == p.func
    f = None
    with pytest.raises(ReferenceError):
        p.func


@memory_leak_decorator()
def test_with_bound_and_unbound_methods():
    data = list(map(str, range(10)))
    join = partial(str.join, '')
    assert join(data) == '0123456789'
    join = partial(''.join)
    assert join(data) == '0123456789'


@memory_leak_decorator()
def test_nested_optimization():
    inner = partial(signature, 'asdf')
    nested = partial(inner, bar=True)
    flat = partial(signature, 'asdf', bar=True)
    assert signature(nested) == signature(flat)


@memory_leak_decorator()
def test_nested_partial_with_attribute():
    # see issue 25137

    def foo(bar):
        return bar

    p = partial(foo, 'first')
    p2 = partial(p, 'second')
    p2.new_attr = 'spam'
    assert p2.new_attr == 'spam'


@memory_leak_decorator()
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


@memory_leak_decorator(offset=1)
def test_pickle():
    with AllowPickle():
        f = partial(signature, ['asdf'], bar=[True])
        f.attr = []
        for proto in range(pickle.HIGHEST_PROTOCOL + 1):
            f_copy = pickle.loads(pickle.dumps(f, proto))
            assert signature(f_copy) == signature(f)


@memory_leak_decorator()
def test_copy():
    f = partial(signature, ['asdf'], bar=[True])
    f.attr = []
    f_copy = copy.copy(f)
    assert signature(f_copy) == signature(f)
    assert f_copy.attr is f.attr
    assert f_copy.args is f.args
    assert f_copy.keywords is f.keywords


@memory_leak_decorator()
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


@memory_leak_decorator()
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


@memory_leak_decorator()
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


@memory_leak_decorator()
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


@memory_leak_decorator(collect=True)
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


@memory_leak_decorator(collect=True)
def test_invalid_kwargs_with_setstate():
    f = partial(object)
    f.__setstate__((object, (), {1: 1}, {}))
    # Shouldn't segfault!!!
    with pytest.raises(TypeError):
        repr(f)


@memory_leak_decorator()
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


@memory_leak_decorator(collect=True)
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


@memory_leak_decorator()
def test_partial_has_placeholder():
    assert hasattr(partial, '_')


@memory_leak_decorator()
def test_placeholder():
    assert partial._ is partial._
    assert copy.copy(partial._) is partial._
    assert copy.deepcopy(partial._) is partial._
    # PlaceholderType.__new__()
    assert type(partial._)() is partial._
    assert repr(partial._) == '_'


@memory_leak_decorator(collect=True)
def test_placeholder_new():
    with pytest.raises(TypeError) as exc:
        type(partial._)(1)
    assert "PlaceholderType takes no arguments" in str(exc)
    with pytest.raises(TypeError) as exc:
        type(partial._)(a=1)
    assert "PlaceholderType takes no arguments" in str(exc)
