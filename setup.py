from setuptools import setup, Extension

from glob import glob
from os import path

import sys


def readme():
    with open('README.rst') as f:
        return f.read()


def version():
    with open('iteration_utilities/__init__.py') as f:
        for line in f:
            if line.startswith('__version__'):
                return line.split(r"'")[1]


cfuncs_module = Extension('iteration_utilities._cfuncs',
                          sources=[path.join('src_c', '_module.c')],
                          depends=glob(path.join('src_c', '*.c'))
                          )

needs_pytest = {'pytest', 'test', 'ptr'}.intersection(sys.argv)
pytest_runner = ['pytest-runner'] if needs_pytest else []

setup(name='iteration_utilities',
      version=version(),

      description='Functional programming utilities for Python 2 and 3.',
      long_description=readme(),
      keywords=('functional functools generator itertools iteration iterator '
                'operators performance reduce utility').split(' '),
      platforms=["Windows Linux"],  # maybe sometime also "Mac OS-X", "Unix"

      classifiers=[
        'Development Status :: 4 - Beta',
        'Programming Language :: Python :: 2'
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3'
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: Implementation :: CPython',
      ],

      license='Apache License Version 2.0',

      url='https://github.com/MSeifert04/iteration_utilities',

      author='Michael Seifert',
      author_email='michaelseifert04@yahoo.de',

      packages=[
          'iteration_utilities',
          'iteration_utilities._recipes',
          'iteration_utilities._helpers',
          ],

      install_requires=[
          ],

      setup_requires=[
          ] + pytest_runner,

      tests_require=[
          'pytest',
          ],

      ext_modules=[cfuncs_module],

      include_package_data=True,
      zip_safe=False,
      )
