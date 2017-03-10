# Built-ins
from __future__ import absolute_import, division, print_function
import copy
import pickle
import unittest
import weakref

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, failingTIterator


def capture(*args, **kw):
    """capture all positional and keyword arguments"""
    return args, kw


def signature(part):
    """ return the signature of a partial object """
    return (part.func, part.args, part.keywords, part.__dict__)


class MyTuple(tuple):
    pass


class BadTuple(tuple):
    def __add__(self, other):
        return list(self) + list(other)


class MyDict(dict):
    pass


class TestPartial(unittest.TestCase):

    partial = iteration_utilities.partial

    class AllowPickle:
        def __enter__(self):
            return self

        def __exit__(self, type, value, tb):
            return False

    def test_attributes_unwritable(self):
        # attributes should not be writable
        p = self.partial(capture, 1, 2, a=10, b=20)
        self.assertRaises(AttributeError,
                          setattr, p, 'func', map)
        self.assertRaises(AttributeError,
                          setattr, p, 'args', (1, 2))
        self.assertRaises(AttributeError,
                          setattr, p, 'keywords', dict(a=1, b=2))

        p = self.partial(hex)
        try:
            del p.__dict__
        except TypeError:
            pass
        else:
            self.fail('partial object allowed __dict__ to be deleted')

    def test_basic_examples(self):
        p = self.partial(capture, 1, 2, a=10, b=20)
        self.assertTrue(callable(p))
        self.assertEqual(p(3, 4, b=30, c=40),
                         ((1, 2, 3, 4), dict(a=10, b=30, c=40)))
        p = self.partial(map, lambda x: x*10)
        self.assertEqual(list(p([1, 2, 3, 4])), [10, 20, 30, 40])

    def test_attributes(self):
        p = self.partial(capture, 1, 2, a=10, b=20)
        # attributes should be readable
        self.assertEqual(p.func, capture)
        self.assertEqual(p.args, (1, 2))
        self.assertEqual(p.keywords, dict(a=10, b=20))

    def test_argument_checking(self):
        self.assertRaises(TypeError, self.partial)  # need at least a func arg
        try:
            self.partial(2)()
        except TypeError:
            pass
        else:
            self.fail('First arg not checked for callability')

    def test_protection_of_callers_dict_argument(self):
        # a caller's dictionary should not be altered by partial
        def func(a=10, b=20):
            return a
        d = {'a': 3}
        p = self.partial(func, a=5)
        self.assertEqual(p(**d), 3)
        self.assertEqual(d, {'a': 3})
        p(b=7)
        self.assertEqual(d, {'a': 3})

    def test_kwargs_copy(self):
        # Issue #29532: Altering a kwarg dictionary passed to a constructor
        # should not affect a partial object after creation
        d = {'a': 3}
        p = self.partial(capture, **d)
        self.assertEqual(p(), ((), {'a': 3}))
        d['a'] = 5
        self.assertEqual(p(), ((), {'a': 3}))

    def test_arg_combinations(self):
        # exercise special code paths for zero args in either partial
        # object or the caller
        p = self.partial(capture)
        self.assertEqual(p(), ((), {}))
        self.assertEqual(p(1, 2), ((1, 2), {}))
        p = self.partial(capture, 1, 2)
        self.assertEqual(p(), ((1, 2), {}))
        self.assertEqual(p(3, 4), ((1, 2, 3, 4), {}))

    def test_kw_combinations(self):
        # exercise special code paths for no keyword args in
        # either the partial object or the caller
        p = self.partial(capture)
        self.assertEqual(p.keywords, {})
        self.assertEqual(p(), ((), {}))
        self.assertEqual(p(a=1), ((), {'a': 1}))
        p = self.partial(capture, a=1)
        self.assertEqual(p.keywords, {'a': 1})
        self.assertEqual(p(), ((), {'a': 1}))
        self.assertEqual(p(b=2), ((), {'a': 1, 'b': 2}))
        # keyword args in the call override those in the partial object
        self.assertEqual(p(a=3, b=2), ((), {'a': 3, 'b': 2}))

    def test_positional(self):
        # make sure positional arguments are captured correctly
        for args in [(), (0,), (0, 1), (0, 1, 2), (0, 1, 2, 3)]:
            p = self.partial(capture, *args)
            expected = args + ('x',)
            got, empty = p('x')
            self.assertTrue(expected == got and empty == {})

    def test_keyword(self):
        # make sure keyword arguments are captured correctly
        for a in ['a', 0, None, 3.5]:
            p = self.partial(capture, a=a)
            expected = {'a': a, 'x': None}
            empty, got = p(x=None)
            self.assertTrue(expected == got and empty == ())

    def test_no_side_effects(self):
        # make sure there are no side effects that affect subsequent calls
        p = self.partial(capture, 0, a=1)
        args1, kw1 = p(1, b=2)
        self.assertTrue(args1 == (0, 1) and kw1 == {'a': 1, 'b': 2})
        args2, kw2 = p()
        self.assertTrue(args2 == (0,) and kw2 == {'a': 1})

    def test_error_propagation(self):
        def f(x, y):
            x / y
        self.assertRaises(ZeroDivisionError, self.partial(f, 1, 0))
        self.assertRaises(ZeroDivisionError, self.partial(f, 1), 0)
        self.assertRaises(ZeroDivisionError, self.partial(f), 1, 0)
        self.assertRaises(ZeroDivisionError, self.partial(f, y=0), 1)

    def test_weakref(self):
        f = self.partial(int, base=16)
        p = weakref.proxy(f)
        self.assertEqual(f.func, p.func)
        f = None
        self.assertRaises(ReferenceError, getattr, p, 'func')

    def test_with_bound_and_unbound_methods(self):
        data = list(map(str, range(10)))
        join = self.partial(str.join, '')
        self.assertEqual(join(data), '0123456789')
        join = self.partial(''.join)
        self.assertEqual(join(data), '0123456789')

    def test_nested_optimization(self):
        partial = self.partial
        inner = partial(signature, 'asdf')
        nested = partial(inner, bar=True)
        flat = partial(signature, 'asdf', bar=True)
        self.assertEqual(signature(nested), signature(flat))

    def test_nested_partial_with_attribute(self):
        # see issue 25137
        partial = self.partial

        def foo(bar):
            return bar

        p = partial(foo, 'first')
        p2 = partial(p, 'second')
        p2.new_attr = 'spam'
        self.assertEqual(p2.new_attr, 'spam')

    def test_repr(self):
        args = (object(), object())
        args_repr = ', '.join(repr(a) for a in args)
        kwargs = {'a': object(), 'b': object()}
        kwargs_reprs = ['a={a!r}, b={b!r}'.format_map(kwargs),
                        'b={b!r}, a={a!r}'.format_map(kwargs)]
        name = 'iteration_utilities.partial'

        f = self.partial(capture)
        self.assertEqual('{name}({capture!r})'
                         ''.format(name=name, capture=capture), repr(f))

        f = self.partial(capture, *args)
        self.assertEqual('{name}({capture!r}, {args_repr})'
                         ''.format(name=name, capture=capture,
                                   args_repr=args_repr), repr(f))

        f = self.partial(capture, **kwargs)
        self.assertIn(repr(f),
                      ['{name}({capture!r}, {kwargs_repr})'
                       ''.format(name=name, capture=capture,
                                 kwargs_repr=kwargs_repr)
                       for kwargs_repr in kwargs_reprs])

        f = self.partial(capture, *args, **kwargs)
        self.assertIn(repr(f),
                      ['{name}({capture!r}, {args_repr}, {kwargs_repr})'
                       ''.format(name=name, capture=capture,
                                 args_repr=args_repr, kwargs_repr=kwargs_repr)
                       for kwargs_repr in kwargs_reprs])

    def test_recursive_repr(self):
        name = 'iteration_utilities.partial'

        f = self.partial(capture)
        f.__setstate__((f, (), {}, {}))
        try:
            self.assertEqual(repr(f), '%s(...)' % (name,))
        finally:
            f.__setstate__((capture, (), {}, {}))

        f = self.partial(capture)
        f.__setstate__((capture, (f,), {}, {}))
        try:
            self.assertEqual(repr(f), '%s(%r, ...)' % (name, capture,))
        finally:
            f.__setstate__((capture, (), {}, {}))

        f = self.partial(capture)
        f.__setstate__((capture, (), {'a': f}, {}))
        try:
            self.assertEqual(repr(f), '%s(%r, a=...)' % (name, capture,))
        finally:
            f.__setstate__((capture, (), {}, {}))

    def test_pickle(self):
        with self.AllowPickle():
            f = self.partial(signature, ['asdf'], bar=[True])
            f.attr = []
            for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                f_copy = pickle.loads(pickle.dumps(f, proto))
                self.assertEqual(signature(f_copy), signature(f))

    def test_copy(self):
        f = self.partial(signature, ['asdf'], bar=[True])
        f.attr = []
        f_copy = copy.copy(f)
        self.assertEqual(signature(f_copy), signature(f))
        self.assertIs(f_copy.attr, f.attr)
        self.assertIs(f_copy.args, f.args)
        self.assertIs(f_copy.keywords, f.keywords)

    def test_deepcopy(self):
        f = self.partial(signature, ['asdf'], bar=[True])
        f.attr = []
        f_copy = copy.deepcopy(f)
        self.assertEqual(signature(f_copy), signature(f))
        self.assertIsNot(f_copy.attr, f.attr)
        self.assertIsNot(f_copy.args, f.args)
        self.assertIsNot(f_copy.args[0], f.args[0])
        self.assertIsNot(f_copy.keywords, f.keywords)
        self.assertIsNot(f_copy.keywords['bar'], f.keywords['bar'])

    def test_setstate(self):
        f = self.partial(signature)
        f.__setstate__((capture, (1,), dict(a=10), dict(attr=[])))

        self.assertEqual(signature(f),
                         (capture, (1,), dict(a=10), dict(attr=[])))
        self.assertEqual(f(2, b=20), ((1, 2), {'a': 10, 'b': 20}))

        f.__setstate__((capture, (1,), dict(a=10), None))

        self.assertEqual(signature(f), (capture, (1,), dict(a=10), {}))
        self.assertEqual(f(2, b=20), ((1, 2), {'a': 10, 'b': 20}))

        f.__setstate__((capture, (1,), None, None))
        # self.assertEqual(signature(f), (capture, (1,), {}, {}))
        self.assertEqual(f(2, b=20), ((1, 2), {'b': 20}))
        self.assertEqual(f(2), ((1, 2), {}))
        self.assertEqual(f(), ((1,), {}))

        f.__setstate__((capture, (), {}, None))
        self.assertEqual(signature(f), (capture, (), {}, {}))
        self.assertEqual(f(2, b=20), ((2,), {'b': 20}))
        self.assertEqual(f(2), ((2,), {}))
        self.assertEqual(f(), ((), {}))

    def test_setstate_errors(self):
        f = self.partial(signature)
        self.assertRaises(TypeError,
                          f.__setstate__, (capture, (), {}))
        self.assertRaises(TypeError,
                          f.__setstate__, (capture, (), {}, {}, None))
        self.assertRaises(TypeError,
                          f.__setstate__, [capture, (), {}, None])
        self.assertRaises(TypeError,
                          f.__setstate__, (None, (), {}, None))
        self.assertRaises(TypeError,
                          f.__setstate__, (capture, None, {}, None))
        self.assertRaises(TypeError,
                          f.__setstate__, (capture, [], {}, None))
        self.assertRaises(TypeError,
                          f.__setstate__, (capture, (), [], None))

    def test_setstate_subclasses(self):
        f = self.partial(signature)
        f.__setstate__((capture, MyTuple((1,)), MyDict(a=10), None))
        s = signature(f)
        self.assertEqual(s, (capture, (1,), dict(a=10), {}))
        self.assertIs(type(s[1]), tuple)
        self.assertIs(type(s[2]), dict)
        r = f()
        self.assertEqual(r, ((1,), {'a': 10}))
        self.assertIs(type(r[0]), tuple)
        self.assertIs(type(r[1]), dict)

        f.__setstate__((capture, BadTuple((1,)), {}, None))
        s = signature(f)
        self.assertEqual(s, (capture, (1,), {}, {}))
        self.assertIs(type(s[1]), tuple)
        r = f(2)
        self.assertEqual(r, ((1, 2), {}))
        self.assertIs(type(r[0]), tuple)

    def test_recursive_pickle(self):
        with self.AllowPickle():
            f = self.partial(capture)
            f.__setstate__((f, (), {}, {}))
            try:
                for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                    with self.assertRaises(RecursionError):
                        pickle.dumps(f, proto)
            finally:
                f.__setstate__((capture, (), {}, {}))

            f = self.partial(capture)
            f.__setstate__((capture, (f,), {}, {}))
            try:
                for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                    f_copy = pickle.loads(pickle.dumps(f, proto))
                    try:
                        self.assertIs(f_copy.args[0], f_copy)
                    finally:
                        f_copy.__setstate__((capture, (), {}, {}))
            finally:
                f.__setstate__((capture, (), {}, {}))

            f = self.partial(capture)
            f.__setstate__((capture, (), {'a': f}, {}))
            try:
                for proto in range(pickle.HIGHEST_PROTOCOL + 1):
                    f_copy = pickle.loads(pickle.dumps(f, proto))
                    try:
                        self.assertIs(f_copy.keywords['a'], f_copy)
                    finally:
                        f_copy.__setstate__((capture, (), {}, {}))
            finally:
                f.__setstate__((capture, (), {}, {}))

    # Issue 6083: Reference counting bug
    def test_setstate_refcount(self):
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

        f = self.partial(object)
        self.assertRaises(TypeError, f.__setstate__, BadSequence())
