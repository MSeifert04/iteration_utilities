Functools
---------

Some classes that are applied to functions and change the way the function is
called. Some of those listed are present in the ``functools`` [0]_ module.


Functools for one function
^^^^^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.complement`, negate the return value of the
  function. Equivalent to ``lambda func: not func``.
- :py:func:`~iteration_utilities.flip`, reverse the order of the arguments
  passed to the function. Equivalent to
  ``lambda func: lambda *args, **kwargs: func(*list(reversed(args)), **kwargs)``
- :py:func:`functools.lru_cache`, cache the return value of a function.
- :py:func:`functools.partial`, partially set the arguments of a function.
- :py:func:`~iteration_utilities.partial`, partially set the arguments of a
  function; also accepting placeholders.
- :py:class:`functools.partialmethod`, same as :py:func:`functools.partial` but
  works on methods.


Functools for several functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.chained`, apply several functions
  (successivly).


References
~~~~~~~~~~

.. [0] https://docs.python.org/library/functools.html
