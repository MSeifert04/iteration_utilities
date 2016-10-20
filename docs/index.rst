Welcome to iteration_utilities's documentation!
===============================================

``iterations_utilities`` is a general purpose collection around the concept of
functional programming. Most of the functions presented here are inspired by
the ``itertools`` [0]_ module, especially the "recipes" section, but also by
the ``toolz`` [1]_ package. It should be noted that there are lots more
packages with similar functions, for example ``more-itertools`` [2]_,
``pydash`` [3]_ and many many more.

Large fractions of the code are implemented in C to achieve very good
performance. However this library cannot compete (intentionally) with
specialized libraries like ``NumPy``, ``pandas`` or ``SciPy``.

.. note::
   The documentation does not only present functionality from the
   ``iteration_utilities`` library but also includes links to several
   builtin modules and functions.

.. warning::
   This library is under ongoing development and may **not** be stable!

Contents:

.. toctree::
   :maxdepth: 2

   installation
   CHANGES
   iterable
   generators
   reduce
   functools
   random
   callbacks
   operators
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
