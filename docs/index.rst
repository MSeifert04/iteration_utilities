Welcome to iteration_utilities's documentation!
===============================================

Installation
^^^^^^^^^^^^

The `iteration_utilities` package is a pure python package supporting the
Python versions:

- Python 2.7
- Python 3.3+

and can be installed with ``pip`` [3]_:

- ``pip install iteration_utilities``

or download the development version from ``git`` [2]_ and install it:

- ``python setup.py install``

Python built-ins
================

Most common iteration based problems can be solved by using the already
existing functionality in Python:

Short-circuit functions
^^^^^^^^^^^^^^^^^^^^^^^

These functions exit as soon as the condition is not fulfilled.

- :py:func:`all`
- :py:func:`any`

Wrap the iterable with a generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These can be applied to any iterable and return a generator so the evaluation
of these is postponed until it is consumed.

- :py:func:`~itertools.accumulate` (see below for Python 2 compatible version)
- :py:func:`~itertools.chain`
- :py:meth:`~itertools.chain.from_iterable`
- :py:func:`~itertools.combinations`
- :py:func:`~itertools.combinations_with_replacement`
- :py:func:`~itertools.compress`
- :py:func:`~itertools.count`
- :py:func:`~itertools.cycle`
- :py:func:`~itertools.dropwhile`
- :py:func:`enumerate`
- :py:func:`filter` (``itertools.ifilter`` in Python 2)
- :py:func:`~itertools.filterfalse` (or ``itertools.ifilterfalse`` in Python 2)
- :py:func:`~itertools.groupby`
- :py:func:`~itertools.islice`
- :py:func:`map` (``itertools.imap`` in Python 2)
- :py:func:`~itertools.permutations`
- :py:func:`~itertools.product`
- :py:func:`~itertools.repeat`
- :py:func:`reversed`
- :py:func:`~itertools.starmap`
- :py:func:`~itertools.takewhile`
- :py:func:`~itertools.tee`
- :py:func:`zip` (``itertools.izip`` in Python 2)
- :py:func:`~itertools.zip_longest` (or ``itertools.izip_longest`` in Python 2)


Builtin recipes
^^^^^^^^^^^^^^^

These recipes are avaiable in Python.

- :py:func:`min`
- :py:func:`max`
- :py:func:`sum`


Recipes
=======

Official recipes
^^^^^^^^^^^^^^^^
The functions and documentations were taken from the official Python
documentation [0]_. But the functions were slightly changed to make them
Python2 and Python3 compatible. The documentation was added and/or modified.

.. py:currentmodule:: iteration_utilities.recipes.core

- :py:func:`all_equal`
- :py:func:`consume`
- :py:func:`dotproduct`
- :py:func:`first_true`
- :py:func:`flatten`
- :py:func:`grouper`
- :py:func:`ipartition`
- :py:func:`iter_except`
- :py:func:`ncycles`
- :py:func:`nth`
- :py:func:`padnone`
- :py:func:`pairwise`
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

- :py:func:`applyfunc`
- :py:func:`deepflatten`
- :py:func:`itersubclasses`
- :py:func:`last_true`
- :py:func:`merge`
- :py:func:`minmax`
- :py:func:`partition`


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
.. [2] https://github.com/MSeifert04/iteration_utilities
.. [3] https://pypi.python.org/pypi/iteration_utilities


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

