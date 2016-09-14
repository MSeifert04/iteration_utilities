Welcome to iteration_utilities's documentation!
===============================================

Installation
^^^^^^^^^^^^

The `iteration_utilities` package is a pure python package supporting the
Python versions:

- Python 2.7
- Python 3.3+

and can be installed with ``pip``:

- ``pip install iteration_utilities``

Recipes
=======

Official recipes
^^^^^^^^^^^^^^^^
The functions and documentations were taken from the official Python
documentation [0]_ and function were slightly changed to make them Python2 and
Python3 compatible. The documentation was added and modified.

.. py:currentmodule:: iteration_utilities.recipes.core

- :py:func:`all_equal`
- :py:func:`consume`
- :py:func:`dotproduct`
- :py:func:`first_true`
- :py:func:`flatten`
- :py:func:`grouper`
- :py:func:`iter_except`
- :py:func:`ncycles`
- :py:func:`nth`
- :py:func:`padnone`
- :py:func:`pairwise`
- :py:func:`partition`
- :py:func:`powerset`
- :py:func:`quantify`
- :py:func:`random_combination`
- :py:func:`random_combination_with_replacement`
- :py:func:`random_product`
- :py:func:`random_permutation`
- :py:func:`repeatfunc`
- :py:func:`roundrobin`
- :py:func:`tabulate`
- :py:func:`take`
- :py:func:`tail`
- :py:func:`tee_lookahead`
- :py:func:`unique_everseen`
- :py:func:`unique_justseen`


Additional recipes
^^^^^^^^^^^^^^^^^^
Additional recipes for iterations.

.. py:currentmodule:: iteration_utilities.recipes.additional

- :py:func:`last_true`
- :py:func:`merge`


Compatibility layer
^^^^^^^^^^^^^^^^^^^
Compatibility layer for :py:mod:`itertools` functions not avaiable in
Python2. The function and documentation was taken from the official Python
documentation [1]_ and the documentation was slightly changed.

.. py:currentmodule:: iteration_utilities.recipes.py2_compat

- :py:func:`accumulate`


Performance Helpers
===================

Performant iteration based functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These functions are not very special or complicated but considerably more
performant than the intuitiv approach. Consider them some tools to copy and
paste in your code.

.. py:currentmodule:: iteration_utilities.helpers.performance

- :py:func:`all_isinstance`
- :py:func:`any_isinstance`



References
~~~~~~~~~~
.. [0] https://docs.python.org/library/itertools.html#itertools-recipes
.. [1] https://docs.python.org/3/library/itertools.html#itertools.accumulate


API Documentation
=================

Contents:

.. toctree::
   :maxdepth: 2

   recipes/core
   recipes/additional
   recipes/py2_compat
   helpers/performance

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

