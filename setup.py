from setuptools import setup, Extension, find_packages

from glob import glob
from os import path

import sys


def version():
    with open('src/iteration_utilities/__init__.py') as f:
        for line in f:
            if line.startswith('__version__'):
                return line.split(r"'")[1]


_iteration_utilities_module = Extension(
    'iteration_utilities._iteration_utilities',
    sources=[path.join('src', 'iteration_utilities', '_iteration_utilities', '_module.c')],
    depends=glob(path.join('src', 'iteration_utilities', '_iteration_utilities', '*.c'))
    )

setup(
    packages=find_packages('src'),
    package_dir={'': 'src'},
    py_modules=[path.splitext(path.basename(p))[0] for p in glob('src/*.py')],
    version=version(),
    ext_modules=[_iteration_utilities_module],
)
