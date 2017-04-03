Iterable, InfiniteIterable and ManyIterables
--------------------------------------------

.. warning::
   :py:class:`~iteration_utilities.Iterable`,
   :py:class:`~iteration_utilities.InfiniteIterable` and
   :py:class:`~iteration_utilities.ManyIterables`
   are currently experimental.

:py:mod:`iteration_utilities` introduces these three classes that can be used
as wrapper for Python iterables. These classes implement the generators present
in the Python builtins, the :py:mod:`itertools`-module and
:py:mod:`iteration_utilities` as methods.

These can be broadly classified in 4 categories:

Creating an Iterable
^^^^^^^^^^^^^^^^^^^^

The constructor allows wrapping a specified `iterable` like a :py:class:`list`
or :py:class:`range` object. But it also has several staticmethods for creating
an :py:class:`~iteration_utilities.Iterable` by other means, these have the
prefix ``from_``. For example the
:py:meth:`iteration_utilities.Iterable.from_repeat` allows to create an
:py:class:`~iteration_utilities.Iterable` using :py:func:`itertools.repeat`.


Modifying and chaining operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As soon as the :py:class:`~iteration_utilities.Iterable` is created one can
process it. Each of the normal (not prefixed methods) returns the result of
the operation (as generator!) so these can be arbitarly chained. This allows to
chain several operations sequentially.

This can be demonstrated best with an actual example. Suppose we have a
:py:class:`list` of strings of numbers and we want to convert each letter to an
integer and then sum the numbers below 3::

    >>> # Python example
    >>> from itertools import chain
    >>> def less_than_three(x):
    ...     return x < 3
    >>> inp = ['12314253', '12368412612', '7812358', '12381531']
    >>> sum(filter(less_than_three, map(int, chain.from_iterable(inp))))
    23

    >>> # Example with Iterable
    >>> from iteration_utilities import Iterable
    >>> sum(Iterable(inp).flatten().map(int).filter(less_than_three))
    23


Conversion methods
^^^^^^^^^^^^^^^^^^

The :py:class:`~iteration_utilities.Iterable` implements the iteration protocol
so it's possible to use it everywhere where an iterable is needed. For example
with ``for item in ...`` or to construct containers, i.e. ``list()``. For
convenience (and to prevent some problems with infinite iterables) finite
:py:class:`~iteration_utilities.Iterable` also have methods to
convert them to the desired class. These are prefixed with ``as_``.
:py:class:`~iteration_utilities.InfiniteIterable` **don't** have these to avoid
creating an infinitly long :py:class:`list`.

.. warning::
   However :py:class:`~iteration_utilities.InfiniteIterable` also implement the
   iteration protocol and could be passed to :py:class:`list`, with severe
   consequences. So use the ``as_*`` and ``get_*`` methods which will still
   throw an :py:class:`AttributeError` but at least they won't create an
   :py:class:`MemoryError` or freeze your computer! You have been warned!

Currently folding methods like :py:func:`sum` are implemented with the prefix
``get_``.

.. note::
   See the documentation of :py:class:`~iteration_utilities.Iterable`
   to see which methods are possible or read the next chapters for more
   background information.

Operating on several iterables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :py:class:`~iteration_utilities.ManyIterables` class implements the methods
that operate on several iterables and return a single
:py:class:`~iteration_utilities.Iterable` or
:py:class:`~iteration_utilities.InfiniteIterable`.

However it is very important that the `iterables` given to
:py:class:`~iteration_utilities.ManyIterables` clearly indicate if they are
infinite, otherwise the methods won't know if the result should be finite or
infinite. These infinite iterables should be wrapped in
:py:class:`~iteration_utilities.InfiniteIterable` or created by the
``Iterable.from_*`` methods.
