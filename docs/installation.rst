Installation
------------

The :py:mod:`iteration_utilities` package is a C extension python package
supporting the Python versions:

- Python 2.7
- Python 3.5+ (older Python 3 versions may work but are not actively supported)


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

    python setup.py build_sphinx  # local documentation build


Dependencies
^^^^^^^^^^^^

Installation:

- python2 2.7 or python3 3.5+
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
