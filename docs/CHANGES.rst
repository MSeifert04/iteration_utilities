Changelog for "iteration_utilities"
-----------------------------------


Version 0.4.1 (unreleased)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- minor speedup for ``next(merge)``.

- removed **unused** ``key`` parameter from ``combinations_from_relations``.

- replaced ``Iterable.as_string`` parameter ``seperaror`` (sic!) by
  ``seperator``.

- included signature for ``__reduce__``, ``__setstate__`` and
  ``__length_hint__`` methods.

- fixed ``Seen.contains_add`` method signature.

- fixed potential segfault in ``ItemIdxKey.__repr__``.

- removed unnecessary ``__setstate__`` method for ``ItemIdxKey``.

- various ``__setstate__`` and ``__reduce__`` methods were changed so they
  can't used to cause segmentation faults, ``SystemError`` or blatantly wrong
  behaviour. However, serializing or copying such an instance can significantly
  slower as a result of this change. Unpickling these instances from previous
  versions could be impossible and ``copy.copy`` is **not** suported (and
  probably won't be ever because ``itertools.tee`` interacts with ``__copy__``
  methods). Affected iterators: ``chained``, ``deepflatten``, ``duplicates``,
  ``grouper``, ``intersperse``, ``merge``, ``roundrobin``, ``sideeffects``,
  ``split``, ``successive``, ``unique_everseen``, ``unique_justseen``.

- added ``__repr__`` method for ``chained``, ``complement``, ``constant``,
  ``flip``, ``nth`` and ``packed``.

- fixed a bug with ``partial`` when the function kept the arguments and a call
  only provided exactly the number of arguments as there are placeholders in
  the partial.


Version 0.4.0 (2017-03-20)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.Seen` and
  :py:func:`~iteration_utilities.ItemIdxKey` can detect recursive objects in
  their :py:func:`repr`.

- The representation for :py:func:`~iteration_utilities.Seen` and
  :py:func:`~iteration_utilities.ItemIdxKey` now uses the classname even for
  subclasses.

- added :py:func:`~iteration_utilities.partial` callback class, which is
  essentially :py:func:`functools.partial` but also allows positional
  placeholders.

- several functions now interpret ``None`` as if that argument for the function
  wasn't given:

  - ``key`` argument for ``minmax``, ``merge``, ``argmin`` and ``argmax``.
  - ``reduce`` argument for ``groupedby``.
  - all arguments for ``Seen.__new__``.


Version 0.3.0 (2017-03-09)
^^^^^^^^^^^^^^^^^^^^^^^^^^

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


Version 0.2.1 (2017-03-01)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- fixed segfault in ``nth`` when ``retpred=True``.


Version 0.2.0 (2017-02-27)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- added ``remove`` parameter to :py:func:`~iteration_utilities.clamp`.
- made :py:func:`~iteration_utilities.deepflatten` string-aware. For other
  recusive-iterable classes a RecursionException is raised instead of freezing.


Version 0.1.0 (2017-01-25)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- initial release
