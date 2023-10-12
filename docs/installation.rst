Installation
------------

The :py:mod:`iteration_utilities` package is a C extension python package
supporting the Python versions:

- Python 3.7+


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

The best way to test against a debug build is to use a Python installation that
has been compiled in debug mode. One could use such a ``Dockerfile``.

.. code-block:: docker

    FROM gcc:latest

    RUN \
        wget https://www.python.org/ftp/python/3.8.0/Python-3.8.0.tgz -q && \
        tar xzf Python-3.8.0.tgz && \
        cd Python-3.8.0 && \
        ./configure --with-pydebug && \
        make altinstall -s -j4 && \
        cd .. && \
        python3.8 -c "import os; os.remove('./Python-3.8.0.tgz'); import shutil; shutil.rmtree('./Python-3.8.0/')" && \
        python3.8 -m pip install pip --upgrade --user && \
        python3.8 -m pip install setuptools wheel --upgrade --user && \
        python3.8 -m pip install pytest --user

This uses Python 3.8.0, you can obviously sdapt this for the actual Python
version you want to build.

Building the image, the library, and the tests.

.. code-block:: none

   docker build -t pythondebug .
   docker run -it --rm -v INSERTDIRECTORYHERE:/io pythondebug
   python3.8 -m pip install /io
   python3.8 -m pytest /io/tests -s -v


Dependencies
^^^^^^^^^^^^

Installation:

- Python 3.7+
- setuptools
- C compiler


Developer Dependencies
^^^^^^^^^^^^^^^^^^^^^^

All dependencies can be installed using::

    python -m pip install iteration_utilities[all]

Or individual dependencies::

    python -m pip install iteration_utilities[test]
    python -m pip install iteration_utilities[documentation]

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
