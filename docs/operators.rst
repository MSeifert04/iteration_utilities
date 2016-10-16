Operators
---------

The Python operator module [0]_ contains a large variety of operators and
``iteration_utilities`` only tries to fill in some missing ones:

.. py:currentmodule:: iteration_utilities._cfuncs

- :py:func:`radd`, equivalent to ``lambda x, y: y + x``
- :py:func:`rsub`, equivalent to ``lambda x, y: y - x``
- :py:func:`rmul`, equivalent to ``lambda x, y: y * x``
- :py:func:`rdiv`, equivalent to ``lambda x, y: y / x``
- :py:func:`rpow`, equivalent to ``lambda x, y: y ** x``

As well as some convenience functions:

- :py:func:`reciprocal`, equivalent to ``lambda x: 1 / x``
- :py:func:`double`, equivalent to ``lambda x: x * 2``
- :py:func:`square`, equivalent to ``lambda x: x ** 2``

And some comparison functions:

- :py:func:`is_None`, equivalent to ``lambda x: x is None``.
- :py:func:`is_not_None`, equivalent to ``lambda x: x is not None``.
- :py:func:`is_even`, equivalent to ``lambda x: (x % 2) == 0``.
- :py:func:`is_odd`, equivalent to ``lambda x: (x % 2) != 0``.
- :py:func:`is_iterable`, roughly equivalent to
  ``lambda x: isinstance(x, collections.Iterable)``.


References
~~~~~~~~~~

.. [0] https://docs.python.org/library/operator.html
