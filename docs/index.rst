Welcome to iteration_utilities's documentation!
===============================================

``iterations_utilities`` is a general purpose collection around the concept of
functional programming based on and utilizing iterators and generators. Most of
the functions presented here are **inspired** by the ``itertools`` [0]_ module,
especially the "recipes" section, but also by the ``toolz`` [1]_ package. It
should be noted that there are lots more packages with similar functions, for
example ``more-itertools`` [2]_, ``pydash`` [3]_ and many, many more.

Large fractions of the code are implemented in C to achieve very good
performance. However this library cannot compete (intentionally) with
specialized libraries like ``NumPy``, ``pandas`` or ``SciPy``.

.. note::
   The documentation does not only present functionality from the
   ``iteration_utilities`` library but also includes links to several
   builtin modules and functions.

.. warning::
   This library is under ongoing development and may **not** be stable!

Overview
--------

There are three classes providing a sequential functional interface for
several built-in and additional functions:

- :py:func:`~iteration_utilities.core.Iterable`
- :py:func:`~iteration_utilities.core.InfiniteIterable`
- :py:func:`~iteration_utilities.core.ManyIterables`

And a complete list of all functionality provided in this package:

================================================================================ ==================================================================== ================================================================= ====================================================================
                                      :py:func:`~iteration_utilities.accumulate`                 :py:func:`~iteration_utilities._cfuncs.all_distinct`                 :py:func:`~iteration_utilities._cfuncs.all_equal` :py:func:`~iteration_utilities._helpers._performance.all_isinstance`
                            :py:func:`~iteration_utilities._cfuncs.all_monotone` :py:func:`~iteration_utilities._helpers._performance.any_isinstance`                         :py:func:`~iteration_utilities.applyfunc`                       :py:func:`~iteration_utilities._cfuncs.argmax`
                                  :py:func:`~iteration_utilities._cfuncs.argmin`       :py:func:`~iteration_utilities._recipes._additional.argsorted`                           :py:func:`~iteration_utilities.chained`                                :py:func:`~iteration_utilities.clamp`
:py:func:`~iteration_utilities._recipes._additional.combinations_from_relations`                           :py:func:`~iteration_utilities.complement`                          :py:func:`~iteration_utilities.constant`               :py:func:`~iteration_utilities._recipes._core.consume`
                             :py:func:`~iteration_utilities._cfuncs.count_items`                          :py:func:`~iteration_utilities.deepflatten`                :py:func:`~iteration_utilities._cfuncs.dotproduct`                       :py:func:`~iteration_utilities._cfuncs.double`
                                      :py:func:`~iteration_utilities.duplicates`               :py:func:`~iteration_utilities._recipes._core.flatten`                              :py:func:`~iteration_utilities.flip`         :py:func:`~iteration_utilities._recipes._additional.getitem`
                               :py:func:`~iteration_utilities._cfuncs.groupedby`                              :py:func:`~iteration_utilities.grouper`       :py:func:`~iteration_utilities._recipes._additional.insert`                          :py:func:`~iteration_utilities.intersperse`
                       :py:func:`~iteration_utilities._recipes._core.ipartition`                      :py:func:`~iteration_utilities._cfuncs.is_even`               :py:func:`~iteration_utilities._cfuncs.is_iterable`                      :py:func:`~iteration_utilities._cfuncs.is_None`
                             :py:func:`~iteration_utilities._cfuncs.is_not_None`                       :py:func:`~iteration_utilities._cfuncs.is_odd`                        :py:func:`~iteration_utilities.ItemIdxKey`                          :py:func:`~iteration_utilities.iter_except`
             :py:func:`~iteration_utilities._recipes._additional.itersubclasses`                                :py:func:`~iteration_utilities.merge`                    :py:func:`~iteration_utilities._cfuncs.minmax`               :py:func:`~iteration_utilities._recipes._core.ncycles`
                                             :py:func:`~iteration_utilities.nth`                          :py:func:`~iteration_utilities._cfuncs.one`          :py:func:`~iteration_utilities._recipes._additional.pad`                    :py:func:`~iteration_utilities._cfuncs.partition`
                         :py:func:`~iteration_utilities._recipes._core.powerset`                         :py:func:`~iteration_utilities._cfuncs.radd` :py:func:`~iteration_utilities._recipes._core.random_combination`    :py:func:`~iteration_utilities._recipes._core.random_permutation`
                   :py:func:`~iteration_utilities._recipes._core.random_product`                         :py:func:`~iteration_utilities._cfuncs.rdiv`                :py:func:`~iteration_utilities._cfuncs.reciprocal`          :py:func:`~iteration_utilities._recipes._additional.remove`
                       :py:func:`~iteration_utilities._recipes._core.repeatfunc`         :py:func:`~iteration_utilities._recipes._additional.replace`    :py:func:`~iteration_utilities._recipes._additional.replicate`                :py:func:`~iteration_utilities._cfuncs.return_called`
                        :py:func:`~iteration_utilities._cfuncs.return_first_arg`              :py:func:`~iteration_utilities._cfuncs.return_identity`                     :py:func:`~iteration_utilities._cfuncs.rfdiv`                         :py:func:`~iteration_utilities._cfuncs.rmod`
                                    :py:func:`~iteration_utilities._cfuncs.rmul`                           :py:func:`~iteration_utilities.roundrobin`                      :py:func:`~iteration_utilities._cfuncs.rpow`                         :py:func:`~iteration_utilities._cfuncs.rsub`
                                            :py:func:`~iteration_utilities.Seen`                          :py:func:`~iteration_utilities.sideeffects`                             :py:func:`~iteration_utilities.split`                       :py:func:`~iteration_utilities._cfuncs.square`
                                      :py:func:`~iteration_utilities.successive`                             :py:func:`~iteration_utilities.tabulate`               :py:func:`~iteration_utilities._recipes._core.tail`         :py:func:`~iteration_utilities._recipes._core.tee_lookahead`
                                 :py:func:`~iteration_utilities.unique_everseen`                      :py:func:`~iteration_utilities.unique_justseen`
================================================================================ ==================================================================== ================================================================= ====================================================================

Contents:

.. toctree::
   :maxdepth: 2

   installation
   license
   CHANGES
   iterable
   generators
   reduce
   functools
   random
   callbacks
   operators
   tipps
   misc
   api/cls
   api/core
   api/additional
   api/helper
   api/cfuncs
   AUTHORS

.. [0] https://docs.python.org/library/itertools.html
.. [1] https://toolz.readthedocs.io
.. [2] https://pythonhosted.org/more-itertools/
.. [3] https://pydash.readthedocs.io/en/latest/

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
