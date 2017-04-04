Iteration utilities
-------------------

Utilities based on Pythons iterators and generators.

The full list of functions and classes included in this package:

=============================================================================================================================== ===================================================================================================== ========================================================================================================= =============================================================================================================
                                  `accumulate <http://iteration-utilities.readthedocs.io/en/latest/generated/accumulate.html>`_     `all_distinct <http://iteration-utilities.readthedocs.io/en/latest/generated/all_distinct.html>`_               `all_equal <http://iteration-utilities.readthedocs.io/en/latest/generated/all_equal.html>`_         `all_isinstance <http://iteration-utilities.readthedocs.io/en/latest/generated/all_isinstance.html>`_
                              `all_monotone <http://iteration-utilities.readthedocs.io/en/latest/generated/all_monotone.html>`_ `any_isinstance <http://iteration-utilities.readthedocs.io/en/latest/generated/any_isinstance.html>`_               `applyfunc <http://iteration-utilities.readthedocs.io/en/latest/generated/applyfunc.html>`_                         `argmax <http://iteration-utilities.readthedocs.io/en/latest/generated/argmax.html>`_
                                          `argmin <http://iteration-utilities.readthedocs.io/en/latest/generated/argmin.html>`_           `argsorted <http://iteration-utilities.readthedocs.io/en/latest/generated/argsorted.html>`_                   `chained <http://iteration-utilities.readthedocs.io/en/latest/generated/chained.html>`_                           `clamp <http://iteration-utilities.readthedocs.io/en/latest/generated/clamp.html>`_
`combinations_from_relations <http://iteration-utilities.readthedocs.io/en/latest/generated/combinations_from_relations.html>`_         `complement <http://iteration-utilities.readthedocs.io/en/latest/generated/complement.html>`_                 `constant <http://iteration-utilities.readthedocs.io/en/latest/generated/constant.html>`_                       `consume <http://iteration-utilities.readthedocs.io/en/latest/generated/consume.html>`_
                                `count_items <http://iteration-utilities.readthedocs.io/en/latest/generated/count_items.html>`_       `deepflatten <http://iteration-utilities.readthedocs.io/en/latest/generated/deepflatten.html>`_             `dotproduct <http://iteration-utilities.readthedocs.io/en/latest/generated/dotproduct.html>`_                         `double <http://iteration-utilities.readthedocs.io/en/latest/generated/double.html>`_
                                  `duplicates <http://iteration-utilities.readthedocs.io/en/latest/generated/duplicates.html>`_                   `first <http://iteration-utilities.readthedocs.io/en/latest/generated/first.html>`_                   `flatten <http://iteration-utilities.readthedocs.io/en/latest/generated/flatten.html>`_                             `flip <http://iteration-utilities.readthedocs.io/en/latest/generated/flip.html>`_
                                        `getitem <http://iteration-utilities.readthedocs.io/en/latest/generated/getitem.html>`_           `groupedby <http://iteration-utilities.readthedocs.io/en/latest/generated/groupedby.html>`_                   `grouper <http://iteration-utilities.readthedocs.io/en/latest/generated/grouper.html>`_     `InfiniteIterable <http://iteration-utilities.readthedocs.io/en/latest/generated/InfiniteIterable.html>`_
                                          `insert <http://iteration-utilities.readthedocs.io/en/latest/generated/insert.html>`_       `intersperse <http://iteration-utilities.readthedocs.io/en/latest/generated/intersperse.html>`_             `ipartition <http://iteration-utilities.readthedocs.io/en/latest/generated/ipartition.html>`_                       `is_even <http://iteration-utilities.readthedocs.io/en/latest/generated/is_even.html>`_
                                `is_iterable <http://iteration-utilities.readthedocs.io/en/latest/generated/is_iterable.html>`_               `is_None <http://iteration-utilities.readthedocs.io/en/latest/generated/is_None.html>`_           `is_not_None <http://iteration-utilities.readthedocs.io/en/latest/generated/is_not_None.html>`_                         `is_odd <http://iteration-utilities.readthedocs.io/en/latest/generated/is_odd.html>`_
                                  `ItemIdxKey <http://iteration-utilities.readthedocs.io/en/latest/generated/ItemIdxKey.html>`_       `iter_except <http://iteration-utilities.readthedocs.io/en/latest/generated/iter_except.html>`_                 `Iterable <http://iteration-utilities.readthedocs.io/en/latest/generated/Iterable.html>`_         `itersubclasses <http://iteration-utilities.readthedocs.io/en/latest/generated/itersubclasses.html>`_
                                              `last <http://iteration-utilities.readthedocs.io/en/latest/generated/last.html>`_   `ManyIterables <http://iteration-utilities.readthedocs.io/en/latest/generated/ManyIterables.html>`_                       `merge <http://iteration-utilities.readthedocs.io/en/latest/generated/merge.html>`_                         `minmax <http://iteration-utilities.readthedocs.io/en/latest/generated/minmax.html>`_
                                        `ncycles <http://iteration-utilities.readthedocs.io/en/latest/generated/ncycles.html>`_                       `nth <http://iteration-utilities.readthedocs.io/en/latest/generated/nth.html>`_                           `one <http://iteration-utilities.readthedocs.io/en/latest/generated/one.html>`_                         `packed <http://iteration-utilities.readthedocs.io/en/latest/generated/packed.html>`_
                                                `pad <http://iteration-utilities.readthedocs.io/en/latest/generated/pad.html>`_               `partial <http://iteration-utilities.readthedocs.io/en/latest/generated/partial.html>`_               `partition <http://iteration-utilities.readthedocs.io/en/latest/generated/partition.html>`_               `Placeholder <http://iteration-utilities.readthedocs.io/en/latest/generated/Placeholder.html>`_
                        `PlaceholderType <http://iteration-utilities.readthedocs.io/en/latest/generated/PlaceholderType.html>`_             `powerset <http://iteration-utilities.readthedocs.io/en/latest/generated/powerset.html>`_                         `radd <http://iteration-utilities.readthedocs.io/en/latest/generated/radd.html>`_ `random_combination <http://iteration-utilities.readthedocs.io/en/latest/generated/random_combination.html>`_
                  `random_permutation <http://iteration-utilities.readthedocs.io/en/latest/generated/random_permutation.html>`_ `random_product <http://iteration-utilities.readthedocs.io/en/latest/generated/random_product.html>`_                         `rdiv <http://iteration-utilities.readthedocs.io/en/latest/generated/rdiv.html>`_                 `reciprocal <http://iteration-utilities.readthedocs.io/en/latest/generated/reciprocal.html>`_
                                          `remove <http://iteration-utilities.readthedocs.io/en/latest/generated/remove.html>`_         `repeatfunc <http://iteration-utilities.readthedocs.io/en/latest/generated/repeatfunc.html>`_                   `replace <http://iteration-utilities.readthedocs.io/en/latest/generated/replace.html>`_                   `replicate <http://iteration-utilities.readthedocs.io/en/latest/generated/replicate.html>`_
                            `return_called <http://iteration-utilities.readthedocs.io/en/latest/generated/return_called.html>`_     `return_False <http://iteration-utilities.readthedocs.io/en/latest/generated/return_False.html>`_ `return_first_arg <http://iteration-utilities.readthedocs.io/en/latest/generated/return_first_arg.html>`_       `return_identity <http://iteration-utilities.readthedocs.io/en/latest/generated/return_identity.html>`_
                                `return_None <http://iteration-utilities.readthedocs.io/en/latest/generated/return_None.html>`_       `return_True <http://iteration-utilities.readthedocs.io/en/latest/generated/return_True.html>`_                       `rfdiv <http://iteration-utilities.readthedocs.io/en/latest/generated/rfdiv.html>`_                             `rmod <http://iteration-utilities.readthedocs.io/en/latest/generated/rmod.html>`_
                                              `rmul <http://iteration-utilities.readthedocs.io/en/latest/generated/rmul.html>`_         `roundrobin <http://iteration-utilities.readthedocs.io/en/latest/generated/roundrobin.html>`_                         `rpow <http://iteration-utilities.readthedocs.io/en/latest/generated/rpow.html>`_                             `rsub <http://iteration-utilities.readthedocs.io/en/latest/generated/rsub.html>`_
                                          `second <http://iteration-utilities.readthedocs.io/en/latest/generated/second.html>`_                     `Seen <http://iteration-utilities.readthedocs.io/en/latest/generated/Seen.html>`_           `sideeffects <http://iteration-utilities.readthedocs.io/en/latest/generated/sideeffects.html>`_                           `split <http://iteration-utilities.readthedocs.io/en/latest/generated/split.html>`_
                                          `square <http://iteration-utilities.readthedocs.io/en/latest/generated/square.html>`_         `starfilter <http://iteration-utilities.readthedocs.io/en/latest/generated/starfilter.html>`_             `successive <http://iteration-utilities.readthedocs.io/en/latest/generated/successive.html>`_                     `tabulate <http://iteration-utilities.readthedocs.io/en/latest/generated/tabulate.html>`_
                                              `tail <http://iteration-utilities.readthedocs.io/en/latest/generated/tail.html>`_   `tee_lookahead <http://iteration-utilities.readthedocs.io/en/latest/generated/tee_lookahead.html>`_                       `third <http://iteration-utilities.readthedocs.io/en/latest/generated/third.html>`_       `unique_everseen <http://iteration-utilities.readthedocs.io/en/latest/generated/unique_everseen.html>`_
                        `unique_justseen <http://iteration-utilities.readthedocs.io/en/latest/generated/unique_justseen.html>`_
=============================================================================================================================== ===================================================================================================== ========================================================================================================= =============================================================================================================

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
