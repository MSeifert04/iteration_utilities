Welcome to iteration_utilities's documentation!
===============================================

.. module:: iteration_utilities
   :synopsis: Utilities based on Pythons iterators and generators.

:py:mod:`iteration_utilities` is a general purpose collection around the
concept of functional programming based on and utilizing iterators and
generators. Most of the functions presented here are **inspired** by the
:py:mod:`itertools` module, especially the "recipes" section, but also by the
``toolz`` [0]_ package. It should be noted that there are lots more packages
with similar functions, for example ``more-itertools`` [1]_, ``pydash`` [2]_
and many, many more.

Large fractions of the code are implemented in C to achieve very good
performance. However this library cannot compete (intentionally) with
specialized libraries like ``NumPy``, ``pandas`` or ``SciPy``.

.. note::
   The documentation also presents functionality from several built-in modules
   and functions to provide a general overview over the avaiable functionality.

.. warning::
   This library is under on-going development and may change it's API!

Overview
--------

There are three classes providing a sequential functional interface for
several built-in and additional functions:

- :py:func:`~iteration_utilities.Iterable`
- :py:func:`~iteration_utilities.InfiniteIterable`
- :py:func:`~iteration_utilities.ManyIterables`

And a complete list of all functionality provided in this package:

=========================================================== ============================================== ================================================ ==================================================
                 :py:func:`~iteration_utilities.accumulate`   :py:func:`~iteration_utilities.all_distinct`        :py:func:`~iteration_utilities.all_equal`     :py:func:`~iteration_utilities.all_isinstance`
               :py:func:`~iteration_utilities.all_monotone` :py:func:`~iteration_utilities.any_isinstance`        :py:func:`~iteration_utilities.applyfunc`             :py:func:`~iteration_utilities.argmax`
                     :py:func:`~iteration_utilities.argmin`      :py:func:`~iteration_utilities.argsorted`          :py:func:`~iteration_utilities.chained`              :py:func:`~iteration_utilities.clamp`
:py:func:`~iteration_utilities.combinations_from_relations`     :py:func:`~iteration_utilities.complement`         :py:func:`~iteration_utilities.constant`            :py:func:`~iteration_utilities.consume`
                :py:func:`~iteration_utilities.count_items`    :py:func:`~iteration_utilities.deepflatten`       :py:func:`~iteration_utilities.dotproduct`             :py:func:`~iteration_utilities.double`
                 :py:func:`~iteration_utilities.duplicates`          :py:func:`~iteration_utilities.first`          :py:func:`~iteration_utilities.flatten`               :py:func:`~iteration_utilities.flip`
                    :py:func:`~iteration_utilities.getitem`      :py:func:`~iteration_utilities.groupedby`          :py:func:`~iteration_utilities.grouper`   :py:func:`~iteration_utilities.InfiniteIterable`
                     :py:func:`~iteration_utilities.insert`    :py:func:`~iteration_utilities.intersperse`       :py:func:`~iteration_utilities.ipartition`            :py:func:`~iteration_utilities.is_even`
                :py:func:`~iteration_utilities.is_iterable`        :py:func:`~iteration_utilities.is_None`      :py:func:`~iteration_utilities.is_not_None`             :py:func:`~iteration_utilities.is_odd`
                 :py:func:`~iteration_utilities.ItemIdxKey`    :py:func:`~iteration_utilities.iter_except`         :py:func:`~iteration_utilities.Iterable`     :py:func:`~iteration_utilities.itersubclasses`
                       :py:func:`~iteration_utilities.last`  :py:func:`~iteration_utilities.ManyIterables`            :py:func:`~iteration_utilities.merge`             :py:func:`~iteration_utilities.minmax`
                    :py:func:`~iteration_utilities.ncycles`            :py:func:`~iteration_utilities.nth`              :py:func:`~iteration_utilities.one`             :py:func:`~iteration_utilities.packed`
                        :py:func:`~iteration_utilities.pad`        :py:func:`~iteration_utilities.partial`        :py:func:`~iteration_utilities.partition`        :py:func:`~iteration_utilities.Placeholder`
            :py:func:`~iteration_utilities.PlaceholderType`       :py:func:`~iteration_utilities.powerset`             :py:func:`~iteration_utilities.radd` :py:func:`~iteration_utilities.random_combination`
         :py:func:`~iteration_utilities.random_permutation` :py:func:`~iteration_utilities.random_product`             :py:func:`~iteration_utilities.rdiv`         :py:func:`~iteration_utilities.reciprocal`
                     :py:func:`~iteration_utilities.remove`     :py:func:`~iteration_utilities.repeatfunc`          :py:func:`~iteration_utilities.replace`          :py:func:`~iteration_utilities.replicate`
              :py:func:`~iteration_utilities.return_called`   :py:func:`~iteration_utilities.return_False` :py:func:`~iteration_utilities.return_first_arg`    :py:func:`~iteration_utilities.return_identity`
                :py:func:`~iteration_utilities.return_None`    :py:func:`~iteration_utilities.return_True`            :py:func:`~iteration_utilities.rfdiv`               :py:func:`~iteration_utilities.rmod`
                       :py:func:`~iteration_utilities.rmul`     :py:func:`~iteration_utilities.roundrobin`             :py:func:`~iteration_utilities.rpow`               :py:func:`~iteration_utilities.rsub`
                     :py:func:`~iteration_utilities.second`           :py:func:`~iteration_utilities.Seen`      :py:func:`~iteration_utilities.sideeffects`              :py:func:`~iteration_utilities.split`
                     :py:func:`~iteration_utilities.square`     :py:func:`~iteration_utilities.starfilter`       :py:func:`~iteration_utilities.successive`           :py:func:`~iteration_utilities.tabulate`
                       :py:func:`~iteration_utilities.tail`  :py:func:`~iteration_utilities.tee_lookahead`            :py:func:`~iteration_utilities.third`    :py:func:`~iteration_utilities.unique_everseen`
            :py:func:`~iteration_utilities.unique_justseen`
=========================================================== ============================================== ================================================ ==================================================

Contents:

.. toctree::
   :maxdepth: 2

   installation
   iterable
   generators
   reduce
   functools
   random
   callbacks
   operators
   tipps
   copy_iterators
   misc
   api
   license
   CHANGES
   AUTHORS

.. [0] https://toolz.readthedocs.io
.. [1] https://more-itertools.readthedocs.io/en/latest/
.. [2] https://pydash.readthedocs.io/en/latest/

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
