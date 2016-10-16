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

Processing an iterable
^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`itertools.accumulate` (Python3 only)
- :py:func:`~iteration_utilities.accumulate`, similar to
  :py:func:`itertools.accumulate` but also for Python2.
- :py:func:`~iteration_utilities._recipes._additional.append`
- :py:meth:`itertools.chain.from_iterable`
- :py:func:`itertools.combinations`
- :py:func:`itertools.combinations_with_replacement`
- :py:func:`itertools.compress`
- :py:func:`~iteration_utilities._recipes._core.consume`
- :py:func:`itertools.cycle`
- :py:func:`~iteration_utilities._recipes._additional.deepflatten`
- :py:func:`itertools.dropwhile`
- :py:func:`enumerate`
- :py:func:`filter` (``itertools.ifilter`` in Python 2)
- :py:func:`itertools.filterfalse` (or ``itertools.ifilterfalse`` in Python 2)
- :py:func:`~iteration_utilities._recipes._core.flatten`
- :py:func:`itertools.groupby`
- :py:func:`~iteration_utilities.grouper`
- :py:func:`~iteration_utilities.intersperse`
- :py:func:`~iteration_utilities._recipes._core.ipartition`
- :py:func:`itertools.islice`
- :py:func:`~iteration_utilities._recipes._core.ncycles`
- :py:func:`~iteration_utilities._recipes._core.padnone`
- :py:func:`itertools.permutations`
- :py:func:`~iteration_utilities._recipes._core.powerset`
- :py:func:`~iteration_utilities._recipes._additional.prepend`
- :py:func:`reversed`
- :py:func:`~iteration_utilities.split`
- :py:func:`itertools.starmap`
- :py:func:`~iteration_utilities.successive`
- :py:func:`~iteration_utilities._recipes._core.tail`
- :py:func:`itertools.takewhile`
- :py:func:`itertools.tee`
- :py:func:`~iteration_utilities._recipes._core.tee_lookahead`
- :py:func:`~iteration_utilities.unique_everseen`
- :py:func:`~iteration_utilities.unique_justseen`


Processing several iterables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`itertools.chain`
- :py:func:`map` (``itertools.imap`` in Python 2)
- :py:func:`~iteration_utilities.merge`
- :py:func:`itertools.product`
- :py:func:`~iteration_utilities.roundrobin`
- :py:func:`zip` (``itertools.izip`` in Python 2)
- :py:func:`itertools.zip_longest` (or ``itertools.izip_longest`` in Python 2)



Processing a value
^^^^^^^^^^^^^^^^^^

- :py:func:`itertools.count`
- :py:func:`~iteration_utilities._recipes._additional.itersubclasses`
- :py:func:`itertools.repeat`


From a function
^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.applyfunc`
- :py:func:`~iteration_utilities.iter_except`
- :py:func:`~iteration_utilities._recipes._core.repeatfunc`
- :py:func:`~iteration_utilities._recipes._core.tabulate`
