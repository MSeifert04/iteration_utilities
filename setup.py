from setuptools import setup, Extension, find_packages

from glob import glob
from os import path

import sys


def version():
    with open('src/iteration_utilities/__init__.py') as f:
        for line in f:
            if line.startswith('__version__'):
                return line.split(r"'")[1]


files = [
    'accumulate.c',
    'alldistinct.c',
    'allequal.c',
    'allisinstance.c',
    'allmonotone.c',
    'always_iterable.c',
    'anyisinstance.c',
    'applyfunc.c',
    'argminmax.c',
    'chained.c',
    'clamp.c',
    'complement.c',
    'constant.c',
    'countitems.c',
    'deepflatten.c',
    'dotproduct.c',
    'duplicates.c',
    'empty.c',
    'exported_helper.c',
    'flip.c',
    'groupedby.c',
    'grouper.c',
    'helper.c',
    'intersperse.c',
    'isx.c',
    'itemidxkey.c',
    'iterexcept.c',
    'mathematical.c',
    'merge.c',
    'minmax.c',
    'nth.c',
    'one.c',
    'packed.c',
    'partial.c',
    'partition.c',
    'placeholder.c',
    'replicate.c',
    'returnx.c',
    'roundrobin.c',
    'seen.c',
    'sideeffect.c',
    'split.c',
    'starfilter.c',
    'successive.c',
    'tabulate.c',
    'uniqueever.c',
    'uniquejust.c',
    '_iteration_utilities.c'
]


_iteration_utilities_module = Extension(
    'iteration_utilities._iteration_utilities',
    sources=[path.join('src', 'iteration_utilities', '_iteration_utilities', filename) for filename in files]
    )

setup(
    packages=find_packages('src'),
    package_dir={'': 'src'},
    py_modules=[path.splitext(path.basename(p))[0] for p in glob('src/*.py')],
    version=version(),
    ext_modules=[_iteration_utilities_module],
)
