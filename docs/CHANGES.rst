Changelog for "iteration_utilities"
-----------------------------------

Version 0.14.0 (not released)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Python 3.14 compatibility: Thanks @akretz for fixing the code so the C extension compiles
  with Python 3.14.

- ``Placeholder`` and ``Empty`` are no singletons anymore. This may lead to slightly worse
  performance.


Version 0.13.0 (2024-10-08)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Python 3.13 compatibility: A few optimizations are not possible anymore in Python 3.13.
  This means the performance of some functions (most notably ``groupedby``) will be slightly
  slower.


Version 0.12.1 (2024-03-03)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Source distribution now includes all files needed to run the tests from the tarball


Version 0.12.0 (2023-10-13)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Improve performance of ``ipartition`` by evaluating the predicate only once
  per item.

- Add benchmarks comparing some functions with other libraries.

- Python 3.12 compatibility

- Dropped Python 3.5 and 3.6 compatibility

- The top level ``__version__`` property was removed. ``importlib.metadata.version`` from the
  Python standard library should be used if you need the version of ``iteration_utilities``.


Version 0.11.0 (2020-11-19)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- The functions implemented in C now parse boolean arguments based on their
  truth value instead of their integer value. This should be unnoticeable in
  typical usage.

- Added ``always_iterable`` which wraps non-iterable inputs with an iterable.

- Added ``empty`` as singleton representing an empty iterable.

- The type of ``Placeholder`` which was previously accessible as
  ``PlaceholderType`` private.

- Added Python 3.9 support.


Version 0.10.1 (2019-11-20)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Fixed reference counting bugs in ``merge``, ``minmax``, and ``sideeffects``.


Version 0.10.0 (2019-11-16)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Exception message for ``one`` if the *iterable* contains more than one element
  does show the first two elements.

- Fixed a bug in ``grouper`` that lead to a fatal exception.


Version 0.9.0 (2019-11-02)
^^^^^^^^^^^^^^^^^^^^^^^^^^

This library does not support Python 2.7 anymore. Attempting to install
``iteration_utilities`` on Python <= 3.4 should not work anymore.

- Add the statistics functions introduced in Python 3.6 and 3.8 as methods
  on the ``Iterable`` class. This includes:

  - ``fmean`` (Python >= 3.8)
  - ``geometric_mean`` (Python >= 3.8)
  - ``harmonic_mean`` (Python >= 3.6)
  - ``multimode`` (Python >= 3.8)
  - ``quantiles`` (Python >= 3.8)

- Add a new recipe from the ``itertools`` documentation: ``nth_combination``.


Version 0.8.0 (2019-10-27)
^^^^^^^^^^^^^^^^^^^^^^^^^^

Support for Python 3.3 and 3.4 was dropped. But it now supports Python 3.7 and
3.8. This will be the last release supporting Python 2.

- ``Iterable.islice`` will raise a more appropriate ``TypeError`` when called
  without arguments.

- ``partial`` only allows plain ``str`` as keyword-names in CPython 3.8.

- Some constants have been available in the module namespace that were intended
  to be private. These have been removed. This includes ``EQ_PY2``, ``GE_PY3``
  and similar constants.

- Added support for PyPy (3.5 and 3.6).

- Use experimental vectorcall protocol (PEP 590) in a few places.


Contributors:

- Solomon Ucko (@sollyucko)


Version 0.7.0 (2018-01-28)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Add ``__sizeof__`` method for ``partial`` to return a more accurate size
  for the instance.

- Fixed a problem when creating ``merge`` instances depending on the compiler.


Version 0.6.1 (2017-04-15)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- The ``__next__`` or ``next`` method of the processed iterator is not cached
  in the ``iteration_utilities`` iterators anymore. This correctly handles the
  (rare) case that this method is reassigned or deleted.


Version 0.6.0 (2017-04-08)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Renamed ``func`` parameter name of ``partition`` to ``pred``.

- Renamed ``function`` parameter name of ``tabulate`` to ``func``.

- The ``key`` attribute of ``ItemIdxKey`` throws an ``AttributeError`` if it
  is not set and an attempt is made to get or delete it.

- Added several attributes to classes.

- Fixed a Bug in ``deepflatten`` when ``isinstance`` fails for the classes
  given as ``types`` or ``ignore`` parameter.

- Changed internal package structure (shouldn't affect end-users that imported
  everything from ``iteration_utilities`` directly).

- improved performance of ``all_isinstance`` and ``any_isinstance``.

- improved performance of ``replicate``.

- ``replicate`` now throws an exception if the ``times`` argument is smaller
  or equal to 1.

- corrected handling of exceptions and overflow in ``__length_hint__`` methods.


Version 0.5.2 (2017-03-30)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- fix release (again).


Version 0.5.1 (2017-03-30)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- fixed major mistake that made 0.5.0 unusable. (``%R`` formatter isn't allowed
  in ``PyErr_Format``).


Version 0.5.0 (2017-03-30)
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
  versions could be impossible and ``copy.copy`` is **not** supported (and
  probably won't be ever because ``itertools.tee`` interacts with ``__copy__``
  methods). Affected iterators: ``chained``, ``deepflatten``, ``duplicates``,
  ``grouper``, ``intersperse``, ``merge``, ``roundrobin``, ``sideeffects``,
  ``split``, ``successive``, ``unique_everseen``, ``unique_justseen``.

- added ``__repr__`` method for ``chained``, ``complement``, ``constant``,
  ``flip``, ``nth`` and ``packed``.

- fixed a bug with ``partial`` when the function kept the arguments and a call
  only provided exactly the number of arguments as there are placeholders in
  the partial.

- Applying ``flip`` on another ``flip`` instance now simply returns the
  original function.

- ``chained`` now unwraps (if possible) other ``chained`` instances when
  creating a new instance. This is only done if this **won't** change the
  current behaviour.


Version 0.4.0 (2017-03-20)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- ``Seen`` and ``ItemIdxKey`` can detect recursive objects in their ``repr``.

- The representation for ``Seen`` and ``ItemIdxKey`` now uses the classname
  even for subclasses.

- added ``partial`` callback class, which is essentially ``functools.partial``
  but also allows positional placeholders.

- several functions now interpret ``None`` as if that argument for the function
  wasn't given:

  - ``key`` argument for ``minmax``, ``merge``, ``argmin`` and ``argmax``.
  - ``reduce`` argument for ``groupedby``.
  - all arguments for ``Seen.__new__``.


Version 0.3.0 (2017-03-09)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- implemented ``__length_hint__`` method for ``clamp``. However sensible
  results (!= 0) are only possible if ``remove=False`` or both ``low`` and
  ``high`` were not set.

- fixed ``SystemError`` in several functions when accessing the next item of
  the iterable resulted in an Exception different from ``StopIteration``.

- added ``starfilter`` iterator.

- added ``packed`` callback class.

- fixed a segfault in ``complement.__call__`` method when the function raised
  an Exception.

- fixed a segfault in ``partition`` when ``bool(item)`` raised an Exception.

- included a missing ``ValueError`` in  ``split`` when two of the ``keep*``
  parameters are True. The case where all three were given already raised the
  correct exception.

- ``clamp`` now interprets ``low=None`` or ``high=None`` as if the
  corresponding value wasn't given. Before it tried to compare the items with
  ``None``.


Version 0.2.1 (2017-03-01)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- fixed segfault in ``nth`` when ``retpred=True``.


Version 0.2.0 (2017-02-27)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- added ``remove`` parameter to ``clamp``.

- made ``deepflatten`` string-aware. For other recursive-iterable classes a
  ``RecursionException`` (or ``RuntimeException`` on python < 3.5) is raised
  instead of freezing.


Version 0.1.0 (2017-01-25)
^^^^^^^^^^^^^^^^^^^^^^^^^^

- initial release
