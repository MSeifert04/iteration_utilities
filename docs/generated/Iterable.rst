Iterable
========

.. currentmodule:: iteration_utilities

.. autoclass:: Iterable

   .. method:: __getitem__(idx)

      See :py:func:`~iteration_utilities.getitem`. If the `idx` is a
      :py:class:`slice` then it is appropriatly converted for the
      :py:func:`~iteration_utilities.getitem` call.

   .. method:: __length_hint__()

      Tries to estimate for the length of the instance (returns ``0`` if an
      estimation is not possible).

      .. note::
         Requires Python >= 3.4
