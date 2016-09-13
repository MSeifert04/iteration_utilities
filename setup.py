from setuptools import setup


setup(name='iteration_utilities',
      version='0.0.2',

      description='some utilities for iterations.',
      long_description=('Itertools recipes from the python package docs and '
                        'additional optimized utilities for iterations.'),
      keywords='itertools performance iteration',

      classifiers=[
        'Development Status :: 3 - Alpha',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
      ],

      license='MIT',

      url='https://github.com/MSeifert04/iteration_utilities',

      author='Michael Seifert',
      author_email='michaelseifert04@yahoo.de',

      packages=[
          'iteration_utilities',
          'iteration_utilities.recipes',
          ],

      install_requires=[
          ],

      setup_requires=[
          'pytest-runner',
          ],

      tests_require=[
          'pytest',
          ],

      include_package_data=True,
      zip_safe=False,
      )
