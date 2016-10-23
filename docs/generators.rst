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
   **Python2**. But there are equivalents in the ``itertools`` [0]_ module. For
   example ``itertools.imap`` instead of ``map``.

Processing an iterable
^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`itertools.accumulate`
- :py:func:`~iteration_utilities.accumulate`
- :py:meth:`itertools.chain.from_iterable` (implemented as `flatten` in `Iterable`)
- :py:func:`~iteration_utilities.clamp`
- :py:func:`itertools.combinations`
- :py:func:`itertools.combinations_with_replacement`
- :py:func:`itertools.compress`
- :py:func:`~iteration_utilities._recipes._core.consume` (not implemented in `Iterable`)
- :py:func:`itertools.cycle`
- :py:func:`~iteration_utilities.deepflatten`
- :py:func:`itertools.dropwhile`
- :py:func:`~iteration_utilities.duplicates`
- :py:func:`enumerate`
- :py:func:`filter`
- :py:func:`itertools.filterfalse`
- :py:func:`~iteration_utilities._recipes._core.flatten`
- :py:func:`~iteration_utilities._recipes._additional.getitem`
- :py:func:`itertools.groupby` (not implemented in `Iterable`)
- :py:func:`~iteration_utilities.grouper`
- :py:func:`~iteration_utilities._recipes._additional.insert`
- :py:func:`~iteration_utilities.intersperse`
- :py:func:`~iteration_utilities._recipes._core.ipartition` (not implemented in `Iterable`)
- :py:func:`itertools.islice`
- :py:func:`iter` (one argument) (not implemented in `Iterable`)
- :py:func:`~iteration_utilities._recipes._core.ncycles`
- :py:func:`~iteration_utilities._recipes._additional.pad`
- :py:func:`itertools.permutations`
- :py:func:`~iteration_utilities._recipes._core.powerset`
- :py:func:`~iteration_utilities._recipes._additional.remove`
- :py:func:`~iteration_utilities._recipes._additional.replace`
- :py:func:`~iteration_utilities._recipes._additional.replicate`
- :py:func:`reversed`
- :py:func:`~iteration_utilities.split`
- :py:func:`itertools.starmap`
- :py:func:`~iteration_utilities.successive`
- :py:func:`~iteration_utilities._recipes._core.tail`
- :py:func:`itertools.takewhile`
- :py:func:`itertools.tee` (not implemented in `Iterable`)
- :py:func:`~iteration_utilities._recipes._core.tee_lookahead` (not implemented in `Iterable`)
- :py:func:`~iteration_utilities.unique_everseen`
- :py:func:`~iteration_utilities.unique_justseen`


Processing several iterables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::
   These functions are implemented as methods for `ManyIterables`.

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
- :py:func:`~iteration_utilities._recipes._additional.itersubclasses`
- :py:func:`itertools.repeat`


From a function
^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.applyfunc`
- :py:func:`iter` (two arguments) (implemented as `from_iterfunc_sentinel` in `Iterable`)
- :py:func:`~iteration_utilities.iter_except` (implemented as `from_iterfunc_exception` in `Iterable`)
- :py:func:`~iteration_utilities._recipes._core.repeatfunc`
- :py:func:`~iteration_utilities._recipes._core.tabulate`


References
~~~~~~~~~~

.. [0] https://docs.python.org/library/itertools.html
