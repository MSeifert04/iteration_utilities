from setuptools import setup, Extension

from glob import glob
from os import path

import sys


def version():
    with open('iteration_utilities/__init__.py') as f:
        for line in f:
            if line.startswith('__version__'):
                return line.split(r"'")[1]


cfuncs_module = Extension('_iteration_utilities',
                          sources=[path.join('src', '_module.c')],
                          depends=glob(path.join('src', '*.c'))
                          )

setup(
    version=version(),
    ext_modules=[cfuncs_module],
)
