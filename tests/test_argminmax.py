# Built-ins
from __future__ import absolute_import, division, print_function

# 3rd party
import pytest

# This module
import iteration_utilities

# Test helper
from helper_leak import memory_leak_decorator
from helper_cls import T, failingTIterator


argmin = iteration_utilities.argmin
argmax = iteration_utilities.argmax


@memory_leak_decorator()
def test_argmax_normal1():
    # Just one test for argmax because the internals are identical to argmin
    assert argmax(T(0), T(1), T(2)) == 2


@memory_leak_decorator()
def test_argmin_positional1():
    assert argmin(T(0), T(1), T(2)) == 0


@memory_leak_decorator()
def test_argmin_positional2():
    assert argmin(T(3), T(0), T(1)) == 1


@memory_leak_decorator()
def test_argmin_positional3():
    # key=None is identical to no key
    assert argmin(T(3), T(1), T(2), key=None) == 1


@memory_leak_decorator()
def test_argmin_sequence1():
    assert argmin([T(3), T(0), T(1)]) == 1


@memory_leak_decorator()
def test_argmin_generator1():
    assert argmin((T(i) for i in [5, 4, 3, 2])) == 3


@memory_leak_decorator()
def test_argmin_key1():
    assert argmin([T(3), T(-2), T(1)], key=abs) == 2


@memory_leak_decorator()
def test_argmin_default1():
    assert argmin([], default=2) == 2


@memory_leak_decorator(collect=True)
def test_argmin_failure1():
    # default not possible if given multiple positional arguments
    with pytest.raises(TypeError):
        argmin(T(3), T(0), T(1), default=1)


@memory_leak_decorator(collect=True)
def test_argmin_failure2():
    # not integer default value
    with pytest.raises(TypeError):
        argmin([T(3), T(0), T(1)], default='1.5', key=lambda x: x + 1)


@memory_leak_decorator(collect=True)
def test_argmin_failure3():
    # unwanted kwarg
    with pytest.raises(TypeError):
        argmin([T(3), T(0), T(1)], default=1, key=lambda x: x + 1, blub=10)


@memory_leak_decorator(collect=True)
def test_argmin_failure4():
    # no args
    with pytest.raises(TypeError):
        argmin(key=lambda x: x + 1)


@memory_leak_decorator(collect=True)
def test_argmin_failure5():
    # empty sequence
    with pytest.raises(ValueError):
        argmin([], key=lambda x: x + 1)


@memory_leak_decorator(collect=True)
def test_argmin_failure6():
    # cmp failed
    with pytest.raises(TypeError):
        argmin([T(1), T(2), T('a')], key=lambda x: x)


@memory_leak_decorator(collect=True)
def test_argmin_failure7():
    # key failed
    with pytest.raises(TypeError):
        argmin([T(1), T(2), T('a')], key=lambda x: x + 1)


@memory_leak_decorator(collect=True)
def test_argmin_failure8():
    # Test that a failing iterator doesn't raise a SystemError
    with pytest.raises(TypeError) as exc:
        argmin(failingTIterator())
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_argmin_failure9():
    # Test that a failing iterator doesn't raise a SystemError
    # with default
    with pytest.raises(TypeError) as exc:
        argmin(failingTIterator(), default=1)
    assert 'eq expected 2 arguments, got 1' in str(exc)


@memory_leak_decorator(collect=True)
def test_argmin_failure10():
    # not iterable
    with pytest.raises(TypeError):
        argmin(T(1))
