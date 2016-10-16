Short-circuit functions
-----------------------

Short-circuit functions [0]_ stop as soon as the exit condition is met. These
functions can yield significant speedups over functions that eagerly process
the operand.

Builtins
^^^^^^^^

There are several instances of short-circuit functions Python library:

- :py:func:`all`, stops as soon as one item in the iterable is falsy.
- :py:func:`any`, stops as soon as one item in the iterable is truthy.
- :py:func:`next`, get the next item of an iterable.

and also two short-circuit operators:

- ``and``, evaluates the right side only if the left side is truthy.
- ``or``, evaluates the right side only if the left side is falsy.


Additional
^^^^^^^^^^

The ``iteration_utilities`` package includes some additional short-circuit
functions:

.. py:currentmodule:: iteration_utilities._cfuncs

- :py:func:`all_distinct`, stops as soon as a duplicate item is found.
- :py:func:`all_equal`, stops as soon as a deviating item is found.
- :py:func:`one`, get the one and only item of an iterable.

.. py:currentmodule:: iteration_utilities

- :py:func:`nth`, stops after the nth item.
- ``first()``, like ``nth`` this function stops after the first item.
- ``second()``, like ``nth`` this function stops after the second item.
- ``third()``, like ``nth`` this function stops after the third item.


Helper functions
^^^^^^^^^^^^^^^^

Included in the ``iteration_utilities`` package are several helper functions
that are based on normal Python code but chosen to evaluate faster than
alternatives:

.. py:currentmodule:: iteration_utilities._helpers._performance

- :py:func:`all_isinstance`, stops as soon as one item is not an instance of
  the specified types.
- :py:func:`any_isinstance`, stops as soon as one item is an instance of the
  specified types.


References
~~~~~~~~~~

.. [0] https://en.wikipedia.org/wiki/Short-circuit_evaluation
