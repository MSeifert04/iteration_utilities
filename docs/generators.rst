Generators
----------

Generators are lazy-evaluating data structures. The values are generated
on demand which allows processing iterables without loading all of the
iterable into memory at once. This makes chaining several generators very
efficient.

.. warning::
   Generators have one disadvantage over data structures like i.e. lists or
   tuples: They can be only processed once. Once the generator is exhausted the
   generator cannot be processed again.

Generators can be created in very different contexts, in this section these are
grouped into three categories: processing an iterable, processing a value and
from a function

.. note::
   Several of the functions mentioned later do not return a generator in
   **Python2**. But there are equivalents in the :py:mod:`itertools` module.
   For example ``itertools.imap`` instead of :py:func:`map`.

Processing an iterable
^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`itertools.accumulate`
- :py:func:`~iteration_utilities.accumulate`
- :py:meth:`itertools.chain.from_iterable` (implemented as `flatten` in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.clamp`
- :py:func:`itertools.combinations`
- :py:func:`~iteration_utilities.combinations_from_relations` (not implemented
  in :py:class:`~iteration_utilities.Iterable`)
- :py:func:`itertools.combinations_with_replacement`
- :py:func:`itertools.compress`
- :py:func:`~iteration_utilities.consume` (not implemented in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`itertools.cycle`
- :py:func:`~iteration_utilities.deepflatten`
- :py:func:`itertools.dropwhile`
- :py:func:`~iteration_utilities.duplicates`
- :py:func:`enumerate`
- :py:func:`filter`
- :py:func:`itertools.filterfalse`
- :py:func:`~iteration_utilities.flatten`
- :py:func:`~iteration_utilities.getitem`
- :py:func:`itertools.groupby` (not implemented in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.grouper`
- :py:func:`~iteration_utilities.insert`
- :py:func:`~iteration_utilities.intersperse`
- :py:func:`~iteration_utilities.ipartition` (not implemented in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`itertools.islice`
- :py:func:`iter` (one argument) (not implemented in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.ncycles`
- :py:func:`~iteration_utilities.pad`
- :py:func:`itertools.permutations`
- :py:func:`~iteration_utilities.powerset`
- :py:func:`~iteration_utilities.remove`
- :py:func:`~iteration_utilities.replace`
- :py:func:`~iteration_utilities.replicate`
- :py:func:`reversed`
- :py:func:`~iteration_utilities.sideeffects` (not implemented in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.split`
- :py:func:`~iteration_utilities.starfilter`
- :py:func:`itertools.starmap`
- :py:func:`~iteration_utilities.successive`
- :py:func:`~iteration_utilities.tail`
- :py:func:`itertools.takewhile`
- :py:func:`itertools.tee` (not implemented in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.tee_lookahead` (not implemented in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.unique_everseen`
- :py:func:`~iteration_utilities.unique_justseen`


Processing several iterables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::
   These functions are implemented as methods for
   :py:class:`~iteration_utilities.ManyIterables`.

- :py:func:`itertools.chain`
- :py:func:`map`
- :py:func:`heapq.merge`
- :py:func:`~iteration_utilities.merge`
- :py:func:`itertools.product`
- :py:func:`~iteration_utilities.roundrobin`
- :py:func:`zip`
- :py:func:`itertools.zip_longest`



Processing a value
^^^^^^^^^^^^^^^^^^

- :py:func:`itertools.count`
- :py:func:`~iteration_utilities.itersubclasses`
- :py:func:`itertools.repeat`


From a function
^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.applyfunc`
- :py:func:`iter` (two arguments) (implemented as `from_iterfunc_sentinel` in
  :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.iter_except` (implemented as `from_iterfunc_exception`
  in :py:class:`~iteration_utilities.Iterable`)
- :py:func:`~iteration_utilities.repeatfunc`
- :py:func:`~iteration_utilities.tabulate`
