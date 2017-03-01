Changelog for "iteration_utilities"
-----------------------------------


Version 0.3.0 (not released)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- implemented ``__length_hint__`` method for :py:func:`~iteration_utilities.clamp`.
  However sensible results (!= 0) are only possible if ``remove=False`` or
  both ``low`` and ``high`` were not set.


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
