Welcome to iteration_utilities's documentation!
===============================================

``iterations_utilities`` is a general purpose collection around the concept of
functional programming. Most of the functions presented here are inspired by
the ``itertools`` [0]_ module, especially the "recipes" section, but also by
the ``toolz`` [1]_ package.

Large fractions of the code are implemented in C to achieve very good
performance. However this library cannot compete (intentionally) with
specialized libraries like ``NumPy``, ``pandas`` or ``SciPy``.

.. note::
   The documentation does not only present functionality from the
   ``iteration_utilities`` library but also includes links to several
   builtin modules and functions.

.. warning::
   This library is under ongoing development and should not be considered
   stable!

Contents:

.. toctree::
   :maxdepth: 2

   installation
   shortcircuit
   reduce
   generators
   functools
   random
   callbacks
   operators
   recipes/core
   recipes/additional
   helpers/performance
   cfuncs/cfuncs

.. [0] https://docs.python.org/library/itertools.html
.. [1] https://toolz.readthedocs.io

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

