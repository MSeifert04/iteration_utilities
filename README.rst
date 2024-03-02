Iteration utilities
-------------------

Utilities based on Pythons iterators and generators.

The full list of functions and classes included in this package:

========================================================================================================== ================================================================================================================================ ============================================================================================================== ======================================================================================================
            `accumulate <https://iteration-utilities.readthedocs.io/en/latest/generated/accumulate.html>`_                               `all_distinct <https://iteration-utilities.readthedocs.io/en/latest/generated/all_distinct.html>`_                   `all_equal <https://iteration-utilities.readthedocs.io/en/latest/generated/all_equal.html>`_ `all_isinstance <https://iteration-utilities.readthedocs.io/en/latest/generated/all_isinstance.html>`_
        `all_monotone <https://iteration-utilities.readthedocs.io/en/latest/generated/all_monotone.html>`_                         `always_iterable <https://iteration-utilities.readthedocs.io/en/latest/generated/always_iterable.html>`_         `any_isinstance <https://iteration-utilities.readthedocs.io/en/latest/generated/any_isinstance.html>`_           `applyfunc <https://iteration-utilities.readthedocs.io/en/latest/generated/applyfunc.html>`_
                    `argmax <https://iteration-utilities.readthedocs.io/en/latest/generated/argmax.html>`_                                           `argmin <https://iteration-utilities.readthedocs.io/en/latest/generated/argmin.html>`_                   `argsorted <https://iteration-utilities.readthedocs.io/en/latest/generated/argsorted.html>`_               `chained <https://iteration-utilities.readthedocs.io/en/latest/generated/chained.html>`_
                      `clamp <https://iteration-utilities.readthedocs.io/en/latest/generated/clamp.html>`_ `combinations_from_relations <https://iteration-utilities.readthedocs.io/en/latest/generated/combinations_from_relations.html>`_                 `complement <https://iteration-utilities.readthedocs.io/en/latest/generated/complement.html>`_             `constant <https://iteration-utilities.readthedocs.io/en/latest/generated/constant.html>`_
                  `consume <https://iteration-utilities.readthedocs.io/en/latest/generated/consume.html>`_                                 `count_items <https://iteration-utilities.readthedocs.io/en/latest/generated/count_items.html>`_               `deepflatten <https://iteration-utilities.readthedocs.io/en/latest/generated/deepflatten.html>`_         `dotproduct <https://iteration-utilities.readthedocs.io/en/latest/generated/dotproduct.html>`_
                    `double <https://iteration-utilities.readthedocs.io/en/latest/generated/double.html>`_                                   `duplicates <https://iteration-utilities.readthedocs.io/en/latest/generated/duplicates.html>`_                           `empty <https://iteration-utilities.readthedocs.io/en/latest/generated/empty.html>`_                   `first <https://iteration-utilities.readthedocs.io/en/latest/generated/first.html>`_
                  `flatten <https://iteration-utilities.readthedocs.io/en/latest/generated/flatten.html>`_                                               `flip <https://iteration-utilities.readthedocs.io/en/latest/generated/flip.html>`_                       `getitem <https://iteration-utilities.readthedocs.io/en/latest/generated/getitem.html>`_           `groupedby <https://iteration-utilities.readthedocs.io/en/latest/generated/groupedby.html>`_
                  `grouper <https://iteration-utilities.readthedocs.io/en/latest/generated/grouper.html>`_                       `InfiniteIterable <https://iteration-utilities.readthedocs.io/en/latest/generated/InfiniteIterable.html>`_                         `insert <https://iteration-utilities.readthedocs.io/en/latest/generated/insert.html>`_       `intersperse <https://iteration-utilities.readthedocs.io/en/latest/generated/intersperse.html>`_
            `ipartition <https://iteration-utilities.readthedocs.io/en/latest/generated/ipartition.html>`_                                         `is_even <https://iteration-utilities.readthedocs.io/en/latest/generated/is_even.html>`_               `is_iterable <https://iteration-utilities.readthedocs.io/en/latest/generated/is_iterable.html>`_               `is_None <https://iteration-utilities.readthedocs.io/en/latest/generated/is_None.html>`_
          `is_not_None <https://iteration-utilities.readthedocs.io/en/latest/generated/is_not_None.html>`_                                           `is_odd <https://iteration-utilities.readthedocs.io/en/latest/generated/is_odd.html>`_                 `ItemIdxKey <https://iteration-utilities.readthedocs.io/en/latest/generated/ItemIdxKey.html>`_       `iter_except <https://iteration-utilities.readthedocs.io/en/latest/generated/iter_except.html>`_
                `Iterable <https://iteration-utilities.readthedocs.io/en/latest/generated/Iterable.html>`_                           `itersubclasses <https://iteration-utilities.readthedocs.io/en/latest/generated/itersubclasses.html>`_                             `last <https://iteration-utilities.readthedocs.io/en/latest/generated/last.html>`_   `ManyIterables <https://iteration-utilities.readthedocs.io/en/latest/generated/ManyIterables.html>`_
                      `merge <https://iteration-utilities.readthedocs.io/en/latest/generated/merge.html>`_                                           `minmax <https://iteration-utilities.readthedocs.io/en/latest/generated/minmax.html>`_                       `ncycles <https://iteration-utilities.readthedocs.io/en/latest/generated/ncycles.html>`_                       `nth <https://iteration-utilities.readthedocs.io/en/latest/generated/nth.html>`_
  `nth_combination <https://iteration-utilities.readthedocs.io/en/latest/generated/nth_combination.html>`_                                                 `one <https://iteration-utilities.readthedocs.io/en/latest/generated/one.html>`_                         `packed <https://iteration-utilities.readthedocs.io/en/latest/generated/packed.html>`_                       `pad <https://iteration-utilities.readthedocs.io/en/latest/generated/pad.html>`_
                  `partial <https://iteration-utilities.readthedocs.io/en/latest/generated/partial.html>`_                                     `partition <https://iteration-utilities.readthedocs.io/en/latest/generated/partition.html>`_               `Placeholder <https://iteration-utilities.readthedocs.io/en/latest/generated/Placeholder.html>`_             `powerset <https://iteration-utilities.readthedocs.io/en/latest/generated/powerset.html>`_
                        `radd <https://iteration-utilities.readthedocs.io/en/latest/generated/radd.html>`_                   `random_combination <https://iteration-utilities.readthedocs.io/en/latest/generated/random_combination.html>`_ `random_permutation <https://iteration-utilities.readthedocs.io/en/latest/generated/random_permutation.html>`_ `random_product <https://iteration-utilities.readthedocs.io/en/latest/generated/random_product.html>`_
                        `rdiv <https://iteration-utilities.readthedocs.io/en/latest/generated/rdiv.html>`_                                   `reciprocal <https://iteration-utilities.readthedocs.io/en/latest/generated/reciprocal.html>`_                         `remove <https://iteration-utilities.readthedocs.io/en/latest/generated/remove.html>`_         `repeatfunc <https://iteration-utilities.readthedocs.io/en/latest/generated/repeatfunc.html>`_
                  `replace <https://iteration-utilities.readthedocs.io/en/latest/generated/replace.html>`_                                     `replicate <https://iteration-utilities.readthedocs.io/en/latest/generated/replicate.html>`_           `return_called <https://iteration-utilities.readthedocs.io/en/latest/generated/return_called.html>`_     `return_False <https://iteration-utilities.readthedocs.io/en/latest/generated/return_False.html>`_
`return_first_arg <https://iteration-utilities.readthedocs.io/en/latest/generated/return_first_arg.html>`_                         `return_identity <https://iteration-utilities.readthedocs.io/en/latest/generated/return_identity.html>`_               `return_None <https://iteration-utilities.readthedocs.io/en/latest/generated/return_None.html>`_       `return_True <https://iteration-utilities.readthedocs.io/en/latest/generated/return_True.html>`_
                      `rfdiv <https://iteration-utilities.readthedocs.io/en/latest/generated/rfdiv.html>`_                                               `rmod <https://iteration-utilities.readthedocs.io/en/latest/generated/rmod.html>`_                             `rmul <https://iteration-utilities.readthedocs.io/en/latest/generated/rmul.html>`_         `roundrobin <https://iteration-utilities.readthedocs.io/en/latest/generated/roundrobin.html>`_
                        `rpow <https://iteration-utilities.readthedocs.io/en/latest/generated/rpow.html>`_                                               `rsub <https://iteration-utilities.readthedocs.io/en/latest/generated/rsub.html>`_                         `second <https://iteration-utilities.readthedocs.io/en/latest/generated/second.html>`_                     `Seen <https://iteration-utilities.readthedocs.io/en/latest/generated/Seen.html>`_
          `sideeffects <https://iteration-utilities.readthedocs.io/en/latest/generated/sideeffects.html>`_                                             `split <https://iteration-utilities.readthedocs.io/en/latest/generated/split.html>`_                         `square <https://iteration-utilities.readthedocs.io/en/latest/generated/square.html>`_         `starfilter <https://iteration-utilities.readthedocs.io/en/latest/generated/starfilter.html>`_
            `successive <https://iteration-utilities.readthedocs.io/en/latest/generated/successive.html>`_                                       `tabulate <https://iteration-utilities.readthedocs.io/en/latest/generated/tabulate.html>`_                             `tail <https://iteration-utilities.readthedocs.io/en/latest/generated/tail.html>`_   `tee_lookahead <https://iteration-utilities.readthedocs.io/en/latest/generated/tee_lookahead.html>`_
                      `third <https://iteration-utilities.readthedocs.io/en/latest/generated/third.html>`_                         `unique_everseen <https://iteration-utilities.readthedocs.io/en/latest/generated/unique_everseen.html>`_       `unique_justseen <https://iteration-utilities.readthedocs.io/en/latest/generated/unique_justseen.html>`_
========================================================================================================== ================================================================================================================================ ============================================================================================================== ======================================================================================================

But also some convenience classes providing a lazy and chainable interface for
function evaluation:

- `Iterable <https://iteration-utilities.readthedocs.io/en/latest/generated/Iterable.html>`_
- `InfiniteIterable <https://iteration-utilities.readthedocs.io/en/latest/generated/InfiniteIterable.html>`_
- `ManyIterables <https://iteration-utilities.readthedocs.io/en/latest/generated/ManyIterables.html>`_


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

.. image:: https://ci.appveyor.com/api/projects/status/7dcitqxmh82d0x0m?svg=true
   :target: https://ci.appveyor.com/project/MSeifert04/iteration-utilities
   :alt: AppVeyor Status

.. image:: https://codecov.io/gh/MSeifert04/iteration_utilities/branch/master/graph/badge.svg
   :target: https://codecov.io/gh/MSeifert04/iteration_utilities
   :alt: Coverage Status

.. image:: https://img.shields.io/badge/benchmarked%20by-asv-green.svg?style=flat
   :target: https://mseifert04.github.io/iutils_benchmarks/
   :alt: Benchmarks
