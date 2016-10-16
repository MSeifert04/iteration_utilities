Fold functions
--------------

Fold functions [0]_ reduce an iterable to a single value. These also include
most short-circuit functions.

Builtins
^^^^^^^^

There are several instances of fold functions Python library:

- :py:func:`all`, reduces the iterable based on the truthiness of all elements.
- :py:func:`any`, reduces the iterable based on the truthiness of all elements.
- :py:func:`len`, reduces the iterable to the number of all elements. Does not
  work with generators!
- :py:func:`max`, reduces the iterable to the maximum of all elements.
- :py:func:`min`, reduces the iterable to the minimum of all elements.
- :py:func:`sum`, reduces the iterable to the sum of all elements.


and also several fold operators:

- the boolean ``and`` and ``or`` operator.
- the mathematical operators ``+``, ``-``, ``*``, ``/``, ``//``, ``%`` and ``**``.
- the bitwise operators ``<<``, ``>>``, ``|``, ``^`` and ``&``.
- the comparison operators ``<``, ``<=``, ``==``, ``!=``, ``>=``, ``>``.


Builtin Library functions
^^^^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`functools.reduce` (in Python 2 part of the builtins), reduces the
  iterable by successivly applying a binary function.

``reduce()`` is probably the most general function that could be used to
recreate all the builtin functions. For example:

- ``reduce(lambda x, y: x and y, iterable)`` is equivalent to ``all()``
- ``reduce(lambda x, y: x or y, iterable)`` is equivalent to ``any()``
- ``reduce(lambda x, y: x + y, iterable)`` is equivalent to ``sum()``
- ``reduce(lambda x, y: x if x < y else y, iterable)`` is equivalent to ``min()``
- ``reduce(lambda x, y: x if x > y else y, iterable)`` is equivalent to ``max()``
- ``reduce(lambda x, y: x + 1, iterable, 0)`` is equivalent to ``len()``

.. warning::
   These ``reduce()`` functions are much slower than the builtins!

There are several other fold functions in the standard library and in third-
party packages, most notably:

- :py:func:`math.fsum`
- ``statistics`` [1]_
- ``operator`` [4]_
- ``NumPy`` [2]_
- ``pandas`` [3]_


Additional
^^^^^^^^^^

The ``iteration_utilities`` package includes some additional fold functions:

.. py:currentmodule:: iteration_utilities._cfuncs

- :py:func:`all_distinct`, reduces the iterable to a boolean value indicating
  if all the items are distinct.
- :py:func:`all_equal`, reduces the iterable to a boolean value indicating
  if all the items are equal.
- :py:func:`argmax`, reduces the iterable to the index of the maximum.
- :py:func:`argmin`, reduces the iterable to the index of the minimum.
- :py:func:`count_items`, reduces the iterable to the number of (matching) items.
- :py:func:`minmax`, reduces the iterable to a tuple containing the mimumum
  and maximum value.

.. py:currentmodule:: iteration_utilities

- :py:func:`nth`, reduces the iterable to it's nth value.
- ``first()``, reduces the iterable to it's first value. See also `nth`.
- ``second()``, reduces the iterable to it's second value. See also `nth`.
- ``third()``, reduces the iterable to it's third value. See also `nth`.
- ``last()``, reduces the iterable to it's last value. See also `nth`.



Helper functions
^^^^^^^^^^^^^^^^

Included in the ``iteration_utilities`` package are several helper functions
that are based on normal Python code but chosen to evaluate faster than
alternatives:

- :py:func:`~iteration_utilities._helpers._performance.all_isinstance`, reduces
  the iterable to the truthiness of isinstance applied to all items.
- :py:func:`~iteration_utilities._helpers._performance.any_isinstance`, reduces
  the iterable to the truthiness of isinstance applied to all items.
- :py:func:`~iteration_utilities._recipes._core.dotproduct`, reduces two
  iterables to the result of the dotproduct.


Fold to other data structure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Most fold functions reduce an iterable by discarding most of the iterable.
However ``iteration_utilities`` includes functions that discard no elements or
only a few:

- :py:func:`~iteration_utilities._cfuncs.groupedby`, create a dictionary
  containing lists representing the groups of values of the iterable.
- :py:func:`heapq.nlargest`, create a list containing the `n` largest items.
- :py:func:`heapq.nsmallest`, create a list containing the `n` smallest items.
- :py:func:`~iteration_utilities._cfuncs.partition`, create a list containing
  the items which do not fulfill some predicate and one containing the items
  that do.
- :py:func:`sorted`, create a sorted list from an iterable.
- :py:func:`~iteration_utilities._recipes._core.take`, take several items from
  the iterable as list.

This list contains some builtin Python functions for completeness.


References
~~~~~~~~~~

.. [0] https://en.wikipedia.org/wiki/Fold_(higher-order_function)
.. [1] https://docs.python.org/library/statistics.html
.. [2] http://www.numpy.org/
.. [3] http://pandas.pydata.org/
.. [4] https://docs.python.org/library/operator.html
