Iteration utilities
-------------------

Utilities based on Pythons iterators and generators.

The full list of functions and classes included in this package:

============================================================================================================================================================================= ================================================================================================================================================ =============================================================================================================================================== ================================================================================================================================================
                                                           `accumulate <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.accumulate>`_                   `all_distinct <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.all_distinct>`_                        `all_equal <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.all_equal>`_ `all_isinstance <http://iteration-utilities.readthedocs.io/en/latest/api/helper.html#iteration_utilities._helpers._performance.all_isinstance>`_
                                               `all_monotone <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.all_monotone>`_ `any_isinstance <http://iteration-utilities.readthedocs.io/en/latest/api/helper.html#iteration_utilities._helpers._performance.any_isinstance>`_                                `applyfunc <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.applyfunc>`_                               `argmax <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.argmax>`_
                                                           `argmin <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.argmin>`_        `argsorted <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.argsorted>`_                                    `chained <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.chained>`_                                         `clamp <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.clamp>`_
`combinations_from_relations <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.combinations_from_relations>`_                               `complement <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.complement>`_                                  `constant <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.constant>`_                        `consume <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.consume>`_
                                                 `count_items <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.count_items>`_                             `deepflatten <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.deepflatten>`_                      `dotproduct <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.dotproduct>`_                               `double <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.double>`_
                                                           `duplicates <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.duplicates>`_                        `flatten <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.flatten>`_                                          `flip <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.flip>`_            `getitem <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.getitem>`_
                                                     `groupedby <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.groupedby>`_                                     `grouper <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.grouper>`_             `insert <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.insert>`_                             `intersperse <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.intersperse>`_
                                              `ipartition <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.ipartition>`_                             `is_even <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.is_even>`_                    `is_iterable <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.is_iterable>`_                             `is_None <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.is_None>`_
                                                 `is_not_None <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.is_not_None>`_                               `is_odd <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.is_odd>`_                              `ItemIdxKey <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.ItemIdxKey>`_                             `iter_except <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.iter_except>`_
                          `itersubclasses <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.itersubclasses>`_                                         `merge <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.merge>`_                              `minmax <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.minmax>`_                        `ncycles <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.ncycles>`_
                                                                         `nth <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.nth>`_                                     `one <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.one>`_                   `pad <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.pad>`_                         `partition <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.partition>`_
                                                  `powerset <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.powerset>`_                                   `radd <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.radd>`_ `random_combination <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.random_combination>`_  `random_permutation <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.random_permutation>`_
                                      `random_product <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.random_product>`_                                   `rdiv <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.rdiv>`_                      `reciprocal <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.reciprocal>`_              `remove <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.remove>`_
                                              `repeatfunc <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.repeatfunc>`_            `replace <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.replace>`_       `replicate <http://iteration-utilities.readthedocs.io/en/latest/api/additional.html#iteration_utilities._recipes._additional.replicate>`_                 `return_called <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.return_called>`_
                                       `return_first_arg <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.return_first_arg>`_             `return_identity <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.return_identity>`_                                `rfdiv <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.rfdiv>`_                                   `rmod <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.rmod>`_
                                                               `rmul <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.rmul>`_                               `roundrobin <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.roundrobin>`_                                  `rpow <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.rpow>`_                                   `rsub <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.rsub>`_
                                                                       `Seen <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.Seen>`_                             `sideeffects <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.sideeffects>`_                                        `split <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.split>`_                               `square <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities._cfuncs.square>`_
                                                           `successive <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.successive>`_                                   `tabulate <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.tabulate>`_                             `tail <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.tail>`_            `tee_lookahead <http://iteration-utilities.readthedocs.io/en/latest/api/core.html#iteration_utilities._recipes._core.tee_lookahead>`_
                                                 `unique_everseen <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.unique_everseen>`_                     `unique_justseen <http://iteration-utilities.readthedocs.io/en/latest/api/cfuncs.html#iteration_utilities.unique_justseen>`_
============================================================================================================================================================================= ================================================================================================================================================ =============================================================================================================================================== ================================================================================================================================================

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
