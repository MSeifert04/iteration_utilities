Callbacks
---------

Some constant and some very simple callbacks.

Constant callbacks
^^^^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.constant`, factory that creates an instance,
  that always returns the initial value. Equivalent to ``lambda x: value``.
- :py:func:`~iteration_utilities.return_True`, equivalent to
  ``lambda x: True``.
- :py:func:`~iteration_utilities.return_False`, equivalent to
  ``lambda x: False``.
- :py:func:`~iteration_utilities.return_None`, equivalent to
  ``lambda x: None``.

Simple callbacks
^^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.packed`, equivalent to
  ``lambda func: lambda x: func(*x)``.
- :py:func:`~iteration_utilities.return_identity`, equivalent to
  ``lambda x: x``.
- :py:func:`~iteration_utilities.return_first_arg`, equivalent to
  ``lambda *args, **kwargs: args[0]``.
- :py:func:`~iteration_utilities.return_called`, equivalent to
  ``lambda x: x()``.
