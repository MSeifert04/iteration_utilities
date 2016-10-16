Callbacks
---------

The Python operator module [0]_ contains a large variety of operators and
``iteration_utilities`` only tries to fill in some missing ones:

- :py:func:`~iteration_utilities.constant`, factory that creates an instance,
  that always returns the initial value. Equivalent to ``lambda x: value``.
- :py:func:`~iteration_utilities._cfuncs.return_identity`, equivalent to
  ``lambda x: x``.
- :py:func:`~iteration_utilities._cfuncs.return_first_arg`, equivalent to
  ``lambda *args, **kwargs: args[0]``.
- :py:func:`~iteration_utilities._cfuncs.return_called`, equivalent to
  ``lambda x: x()``.
- ``return_True()``, equivalent to ``lambda x: True``.
- ``return_False()``, equivalent to ``lambda x: False``.
- ``return_None()``, equivalent to ``lambda x: None``.


References
~~~~~~~~~~

.. [0] https://docs.python.org/library/operator.html
