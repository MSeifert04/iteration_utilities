from setuptools import setup, Extension


returnx_module = Extension(
        'iteration_utilities._returnx',
        sources=['src_c/returnx.c']
    )

isx_module = Extension(
        'iteration_utilities._isx',
        sources=['src_c/isx.c']
    )

mathematical_module = Extension(
        'iteration_utilities._mathematical',
        sources=['src_c/mathematical.c']
    )

reduce_module = Extension(
        'iteration_utilities._reduce',
        sources=['src_c/reduce.c']
    )

crecipes_module = Extension(
        'iteration_utilities._crecipes',
        sources=['src_c/recipes.c']
    )


def readme():
    with open('README.rst') as f:
        return f.read()

# Must be a seperate variable because conf.py reads the version from here.
version = "0.0.4"


setup(name='iteration_utilities',
      version=version,

      description='some utilities for iterations.',
      long_description=readme(),
      keywords=['itertools', 'performance', 'iteration'],

      classifiers=[
        'Development Status :: 4 - Beta',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
      ],

      license='MIT',

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
          'pytest-runner',
          ],

      tests_require=[
          'pytest',
          ],

      ext_modules=[returnx_module,
                   isx_module,
                   mathematical_module,
                   reduce_module,
                   crecipes_module],

      include_package_data=True,
      zip_safe=False,
      )
