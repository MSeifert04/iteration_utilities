from setuptools import setup, Extension, find_packages

from glob import glob
from os import path

project_name = "iteration_utilities"


def readme():
    with open('README.rst') as f:
        return f.read()


def version():
    with open('{}/__init__.py'.format(project_name)) as f:
        for line in f:
            if line.startswith('__version__'):
                return line.split(r"'")[1]


cfuncs_module = Extension('_iteration_utilities',
                          sources=[path.join('src', '_module.c')],
                          depends=glob(path.join('src', '*.c'))
                          )

setup(name=project_name,
      version=version(),

      description='Utilities based on Pythons iterators and generators.',
      long_description=readme(),
      # Somehow the keywords get lost if I use a list of strings so this is
      # just a longish string...
      keywords='functional functools generator itertools iteration iterator '
               'operators performance reduce utility',
      platforms=["Windows Linux Mac OS-X"],

      classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Operating System :: MacOS :: MacOS X',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: Implementation :: CPython',
        'Topic :: Utilities',
      ],

      license='Apache License Version 2.0',

      url='https://github.com/MSeifert04/iteration_utilities',

      author='Michael Seifert',
      author_email='michaelseifert04@yahoo.de',

      packages=find_packages(),

      ext_modules=[cfuncs_module],

      include_package_data=True,
      zip_safe=False,
      )
