Fold functions
--------------

Fold functions [0]_ reduce an iterable to a single value.

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

- :py:func:`functools.reduce`, reduces the iterable by successivly applying a
  binary function.

:py:func:`functools.reduce` is probably the most general function that could be
used to recreate all the builtin functions. For example:

- ``reduce(lambda x, y: x and y, iterable)`` is equivalent to ``all()``
- ``reduce(lambda x, y: x or y, iterable)`` is equivalent to ``any()``
- ``reduce(lambda x, y: x + y, iterable)`` is equivalent to ``sum()``
- ``reduce(lambda x, y: x if x < y else y, iterable)`` is equivalent to ``min()``
- ``reduce(lambda x, y: x if x > y else y, iterable)`` is equivalent to ``max()``
- ``reduce(lambda x, y: x + 1, iterable, 0)`` is equivalent to ``len()``

.. warning::
   These :py:func:`functools.reduce` functions are much slower than the
   builtins!

There are several other fold functions in the standard library and in third-party
packages, most notably:

- :py:func:`math.fsum`
- ``statistics`` [1]_
- ``operator`` [4]_
- ``NumPy`` [2]_
- ``pandas`` [3]_


Additional
^^^^^^^^^^

The :py:mod:`iteration_utilities` package includes some additional fold functions:

- :py:func:`~iteration_utilities.all_distinct`, reduces the iterable to a
  boolean value indicating if all the items are distinct.
- :py:func:`~iteration_utilities.all_equal`, reduces the iterable to a boolean
  value indicating if all the items are equal.
- :py:func:`~iteration_utilities.all_monotone`, reduces the iterable to a
  boolean value indicating if all the items are (strictly) bigger or smaller
  than their predecessor.
- :py:func:`~iteration_utilities.argmax`, reduces the iterable to the index of
  the maximum.
- :py:func:`~iteration_utilities.argmin`, reduces the iterable to the index of
  the minimum.
- :py:func:`~iteration_utilities.count_items`, reduces the iterable to the
  number of (matching) items.
- :py:func:`~iteration_utilities.minmax`, reduces the iterable to a tuple
  containing the mimumum and maximum value.

- :py:func:`~iteration_utilities.nth`, reduces the iterable to it's nth value.
- :py:func:`~iteration_utilities.first`, reduces the iterable to it's first
  value. See also :py:func:`~iteration_utilities.nth`.
- :py:func:`~iteration_utilities.second`, reduces the iterable to it's second
  value. See also :py:func:`~iteration_utilities.nth`.
- :py:func:`~iteration_utilities.third`, reduces the iterable to it's third
  value. See also :py:func:`~iteration_utilities.nth`.
- :py:func:`~iteration_utilities.last`, reduces the iterable to it's last
  value. See also :py:func:`~iteration_utilities.nth`.

- :py:func:`~iteration_utilities.nth_combination`, creates the *nth* combination
  from the elements in the iterable without having to create the previous
  combinations.


Helper functions
^^^^^^^^^^^^^^^^

Included in the :py:mod:`iteration_utilities` package are several helper functions
that are based on normal Python code but chosen to evaluate faster than
alternatives:

- :py:func:`~iteration_utilities.all_isinstance`, reduces the iterable to the
  truthiness of :py:func:`isinstance` applied to all items.
- :py:func:`~iteration_utilities.any_isinstance`, reduces the iterable to the
  truthiness of :py:func:`isinstance` applied to all items.
- :py:func:`~iteration_utilities.dotproduct`, reduces two iterables to the
  result of the dotproduct.


Fold to other data structure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Most fold functions reduce an iterable by discarding most of the iterable.
However :py:mod:`iteration_utilities` includes functions that discard no
elements or only a few:

- :py:func:`~iteration_utilities.argsorted`, create a list
  of indices that would sort the iterable.
- :py:func:`~iteration_utilities.groupedby`, create a dictionary
  containing lists representing the groups of values of the iterable.
- :py:func:`heapq.nlargest`, create a list containing the `n` largest items.
- :py:func:`heapq.nsmallest`, create a list containing the `n` smallest items.
- :py:func:`~iteration_utilities.partition`, create a list containing
  the items which do not fulfill some predicate and one containing the items
  that do.
- :py:func:`sorted`, create a sorted list from an iterable.

This list contains some builtin Python functions for completeness.


Short-circuit functions
^^^^^^^^^^^^^^^^^^^^^^^

Short-circuit functions [5]_ stop as soon as the exit condition is met. These
functions can yield significant speedups over functions that eagerly process
the operand.

There are several instances of short-circuit functions Python library:

- :py:func:`all`, stops as soon as one item in the iterable is falsy.
- :py:func:`any`, stops as soon as one item in the iterable is truthy.
- :py:func:`next`, get the next item of an iterable.

and also two short-circuit operators:

- ``and``, evaluates the right side only if the left side is truthy.
- ``or``, evaluates the right side only if the left side is falsy.


:py:mod:`iteration_utilities` includes some additional short-circuit functions:

- :py:func:`~iteration_utilities.all_distinct`, stops as soon as a duplicate item is found.
- :py:func:`~iteration_utilities.all_equal`, stops as soon as a deviating item is found.
- :py:func:`~iteration_utilities.all_monotone`, stops as soon as a item is found violating monotony.
- :py:func:`~iteration_utilities.one`, get the one and only item of an iterable.

- :py:func:`~iteration_utilities.nth`, stops after the nth item.
- :py:func:`~iteration_utilities.first`, like :py:func:`~iteration_utilities.nth`
  this function stops after the first item.
- :py:func:`~iteration_utilities.second`, like :py:func:`~iteration_utilities.nth`
  this function stops after the second item.
- :py:func:`~iteration_utilities.third`, like :py:func:`~iteration_utilities.nth`
  this function stops after the third item.


Included in the :py:mod:`iteration_utilities` package are several helper functions
that are based on normal Python code but chosen to evaluate faster than
alternatives:

- :py:func:`~iteration_utilities.all_isinstance`, stops as soon as one item is
  not an instance of the specified types.
- :py:func:`~iteration_utilities.any_isinstance`, stops as soon as one item is
  an instance of the specified types.



References
~~~~~~~~~~

.. [0] https://en.wikipedia.org/wiki/Fold_(higher-order_function)
.. [1] https://docs.python.org/library/statistics.html
.. [2] http://www.numpy.org/
.. [3] http://pandas.pydata.org/
.. [4] https://docs.python.org/library/operator.html
.. [5] https://en.wikipedia.org/wiki/Short-circuit_evaluation
