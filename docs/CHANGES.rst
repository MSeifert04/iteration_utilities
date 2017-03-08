Changelog for "iteration_utilities"
-----------------------------------


Version 0.3.0 (not released)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- implemented ``__length_hint__`` method for :py:func:`~iteration_utilities.clamp`.
  However sensible results (!= 0) are only possible if ``remove=False`` or
  both ``low`` and ``high`` were not set.

- fixed ``SystemError`` in several functions when accessing the next item of
  the iterable resulted in an Exception different from ``StopIteration``.

- added :py:func:`~iteration_utilities.starfilter` iterator.

- added :py:func:`~iteration_utilities.packed` callback class.

- fixed a segfault in :py:func:`~iteration_utilities.complement` ``__call__``
  method when the function raised an Exception.

- fixed a segfault in :py:func:`~iteration_utilities._cfuncs.partition`
  when ``bool(item)`` raised an Exception.

- included a missing ``ValueError`` in  :py:func:`~iteration_utilities.split`
  when two of the ``keep*`` parameters are True. The case where all three were
  given already raised the correct exception.

- :py:func:`~iteration_utilities.clamp` now interprets ``low=None`` or
  ``high=None`` as if the corresponding value wasn't given. Before it tried to
  compare the items with ``None``.


Version 0.2.1 (01.03.2017)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- fixed segfault in ``nth`` when ``retpred=True``.


Version 0.2.0 (27.02.2017)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- added ``remove`` parameter to :py:func:`~iteration_utilities.clamp`.
- made :py:func:`~iteration_utilities.deepflatten` string-aware. For other
  recusive-iterable classes a RecursionException is raised instead of freezing.


Version 0.1.0 (25.01.2017)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- initial release
