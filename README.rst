Iteration utilities
-------------------

Utilities based on Pythons iterators and generators.

The full list of functions and classes included in this package:

============================================================================================ ==================================================================== ================================================================= ====================================================================
                                                  :py:func:`~iteration_utilities.accumulate`                 :py:func:`~iteration_utilities._cfuncs.all_distinct`                 :py:func:`~iteration_utilities._cfuncs.all_equal` :py:func:`~iteration_utilities._helpers._performance.all_isinstance`
                                        :py:func:`~iteration_utilities._cfuncs.all_monotone` :py:func:`~iteration_utilities._helpers._performance.any_isinstance`                         :py:func:`~iteration_utilities.applyfunc`                       :py:func:`~iteration_utilities._cfuncs.argmax`
                                              :py:func:`~iteration_utilities._cfuncs.argmin`       :py:func:`~iteration_utilities._recipes._additional.argsorted`                           :py:func:`~iteration_utilities.chained`                                :py:func:`~iteration_utilities.clamp`
:py:func:`~iteration_utilities._recipes._additional.combinations_from_equivalence_relations`                           :py:func:`~iteration_utilities.complement`                          :py:func:`~iteration_utilities.constant`               :py:func:`~iteration_utilities._recipes._core.consume`
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
                                                        :py:func:`~iteration_utilities.Seen`                                :py:func:`~iteration_utilities.split`                    :py:func:`~iteration_utilities._cfuncs.square`                           :py:func:`~iteration_utilities.successive`
                                                    :py:func:`~iteration_utilities.tabulate`                  :py:func:`~iteration_utilities._recipes._core.tail`      :py:func:`~iteration_utilities._recipes._core.tee_lookahead`                      :py:func:`~iteration_utilities.unique_everseen`
                                             :py:func:`~iteration_utilities.unique_justseen`
============================================================================================ ==================================================================== ================================================================= ====================================================================

But also some convenience classes providing a lazy and chainable interface for
function evaluation:

- `Iterable <http://iteration-utilities.readthedocs.io/en/latest/api/cls.html#iteration_utilities.core.Iterable>`_
- `InfiniteIterable <http://iteration-utilities.readthedocs.io/en/latest/api/cls.html#iteration_utilities.core.InfiniteIterable>`_
- `ManyIterables <http://iteration-utilities.readthedocs.io/en/latest/api/cls.html#iteration_utilities.core.ManyIterables>`_


.. image:: https://img.shields.io/pypi/pyversions/iteration_utilities.svg
   :target: https://www.python.org/
   :alt: Supported Python versions

Documentation
^^^^^^^^^^^^^

.. image:: https://readthedocs.org/projects/iteration-utilities/badge/?version=stable
   :target: http://iteration-utilities.readthedocs.io/en/stable/?badge=stable
   :alt: Documentation Status

.. image:: https://readthedocs.org/projects/iteration-utilities/badge/?version=latest
   :target: http://iteration-utilities.readthedocs.io/en/latest/?badge=latest
   :alt: Documentation Status


Downloads
^^^^^^^^^

.. image:: https://img.shields.io/pypi/v/iteration_utilities.svg
   :target: https://pypi.python.org/pypi/iteration_utilities
   :alt: PyPI Project

.. image:: https://img.shields.io/github/release/MSeifert04/iteration_utilities.svg
   :target: https://github.com/MSeifert04/iteration_utilities/releases
   :alt: GitHub Project

.. image:: https://anaconda.org/conda-forge/iteration_utilities/badges/version.svg
   :target: https://anaconda.org/conda-forge/iteration_utilities
   :alt: Anaconda-Server Badge


Test status
^^^^^^^^^^^

.. image:: https://travis-ci.org/MSeifert04/iteration_utilities.svg?branch=master
   :target: https://travis-ci.org/MSeifert04/iteration_utilities
   :alt: Travis CI Status

.. image:: https://ci.appveyor.com/api/projects/status/7dcitqxmh82d0x0m?svg=true
   :target: https://ci.appveyor.com/project/MSeifert04/iteration-utilities
   :alt: AppVeyor Status

.. image:: https://coveralls.io/repos/github/MSeifert04/iteration_utilities/badge.svg?branch=master
   :target: https://coveralls.io/github/MSeifert04/iteration_utilities?branch=master
   :alt: Coverage Status

.. image:: https://img.shields.io/badge/benchmarked%20by-asv-green.svg?style=flat
   :target: https://mseifert04.github.io/iutils_benchmarks/
   :alt: Benchmarks
