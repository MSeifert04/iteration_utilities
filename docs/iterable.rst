Iterable and InfiniteIterable
-----------------------------

.. warning::
   :py:class:`~iteration_utilities.core.Iterable` and
   :py:class:`~iteration_utilities.core.InfiniteIterable` are currently
   experimental.

``iteration_utilities`` introduces these two classes that can be used as
wrapper for Python iterables. These classes implement the generators present
in the Python builtins, the ``itertools``-module and ``iteration_utilities``
as methods.

These can be broadly classified in 3 categories:

Creating an Iterable
^^^^^^^^^^^^^^^^^^^^

The constructor allows wrapping a specified `iterable` like a `list` or
`range` object. But it also has several staticmethods for creating an
`Iterable` by other means, these have the prefix ``from_``. For example the
:py:meth:`~iteration_utilities.core.Iterable.from_repeat` allows to create an
`Iterable` by using :py:func:`itertools.repeat`.


Modifying and chaining operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As soon as the `Iterable` is created one can process it. Each of the normal
(not prefixed methods) returns the result of the operation (as generator!)
so these can be arbitarly chained. This allows to chain several operations
sequentially.

This can be demonstrated best with an actual example. Suppose we have a list of
strings of numbers and we want to convert each letter to an integer and then
sum the numbers below 3::

    >>> # Python example
    >>> from itertools import chain
    >>> def less_than_three(x):
    ...     return x < 3
    >>> inp = ['12314253', '12368412612', '7812358', '12381531']
    >>> sum(filter(less_than_three, map(int, chain.from_iterable(inp))))
    23

    >>> # Example with Iterable
    >>> sum(Iterable(inp).flatten().map(int).filter(less_than_three))
    23


Conversion methods
^^^^^^^^^^^^^^^^^^

The `Iterable` implements the iteration protocol so it's possible to use it
everywhere where an iterable is needed. For example with ``for item in ...``
or to construct containers, i.e. ``list()``. For convenience (and to prevent
some problems with infinite iterables) finite `Iterables` also have methods to
convert them to the desired class. These are prefixed with ``as_``.
`InfiniteIterables` **don't** have these to avoid creating an infinitly long
`list`.

.. warning::
   However `InfiniteIterables` also implement the iteration protocol and could
   be passed to `list`, with severe consequences. So use the ``as_*`` methods
   which will still throw an ``AttributeError`` but at least they won't create
   an ``MemoryError``! You have been warned.

Currently folding methods like ``sum()`` are not implemented.


.. note::
   See the documentation of :py:class:`~iteration_utilities.core.Iterable`
   to see which methods are possible or read the next chapters for more
   background information.
