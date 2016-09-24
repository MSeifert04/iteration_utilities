from setuptools import setup, Extension


callbacks_module = Extension(
        'iteration_utilities.callbacks.simplecallbacks',
        sources=['src_c/simplecallbacks.c']
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
          'iteration_utilities.recipes',
          'iteration_utilities.helpers',
          'iteration_utilities.callbacks',
          ],

      install_requires=[
          ],

      setup_requires=[
          'pytest-runner',
          ],

      tests_require=[
          'pytest',
          ],

      ext_modules=[callbacks_module],

      include_package_data=True,
      zip_safe=False,
      )
