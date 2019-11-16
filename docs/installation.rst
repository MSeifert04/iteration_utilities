Installation
------------

The :py:mod:`iteration_utilities` package is a C extension python package
supporting the Python versions:

- Python 3.5+


Using pip
^^^^^^^^^

and can be installed with ``pip`` [0]_::

    python -m pip install iteration_utilities

or to install the development version::

    python -m pip install git+https://github.com/MSeifert04/iteration_utilities.git@master


Using conda
^^^^^^^^^^^

It can by installed with ``conda`` [2]_ from the ``conda-forge`` channel::

    conda install -c conda-forge iteration_utilities


Manual installation
^^^^^^^^^^^^^^^^^^^

To manually install the package you need to download the development version
from ``git`` [1]_ and install it::

    git clone https://github.com/MSeifert04/iteration_utilities.git
    cd iteration_utilities
    python -m pip install .

with the clone from ``git`` one can also run the tests after the installation::

    python -m pytest tests/

Or build the documentation::

    sphinx-build -b html -W -a -n docs/ build/sphinx/html/  # local documentation build


Testing debug installation
^^^^^^^^^^^^^^^^^^^^^^^^^^

To run the tests using the internal assertions one has to define the ``PyIU_DEBUG``
macro while building the library::

   python -m pip install . --global-option=build_ext --global-option="--define=PyIU_DEBUG"

Then run the tests without capturing ``stderr``::

   python -m pytest tests/ -s

.. note::
   This should not be confused with **real** debug builds. The ``PyIU_DEBUG``
   macro only enables the library specific assertions.


Dependencies
^^^^^^^^^^^^

Installation:

- Python 3.5+
- setuptools
- C compiler


Developer Dependencies
^^^^^^^^^^^^^^^^^^^^^^

All dependencies can be installed using::

    python -m pip install iteration_utilities[all]

Or individual dependencies::

    python -m pip install iteration_utilities[test]
    python -m pip install iteration_utilities[doc]

Tests:

- pytest

Documentation:

- sphinx
- numpydoc


References
~~~~~~~~~~

.. [0] https://github.com/MSeifert04/iteration_utilities
.. [1] https://pypi.python.org/pypi/iteration_utilities
.. [2] https://www.continuum.io/
