def create_included_function_list():
    """Creates an RST table to insert in the "docs/index.rst" file for the
    complete overview of the package.

    Requires `astropy`!
    """
    import iteration_utilities
    from iteration_utilities import Iterable
    from itertools import chain
    from operator import itemgetter
    from astropy.table import Table
    from astropy.io.ascii import RST

    it = Iterable(chain(iteration_utilities._cfuncs.__dict__.items(),
                        iteration_utilities._helpers._performance.__dict__.items(),
                        iteration_utilities._recipes._core.__dict__.items(),
                        iteration_utilities._recipes._additional.__dict__.items())
                  # Exclude PY2 variable and private functions
                  ).filterfalse(lambda x: x[0].startswith(('PY2', '_'))
                  # Exclude everything that has no __module__
                  ).filter(lambda x: hasattr(x[1], '__module__')
                  # Only include functions that come from the package
                  ).filter(lambda x: x[1].__module__.startswith('iteration_utilities')
                  # Remove duplicate names
                  ).unique_everseen(itemgetter(0)
                  # Sort lexically
                  ).get_sorted(key=lambda x: x[0].lower())

    it = Iterable(it
                  # Create a Sphinx link from function name and module
                  ).map(lambda i: ':py:func:`~{}.{}`'.format(i[1].__module__, i[0])
                  # Group into 4s so we get a 4 column Table
                  ).grouper(4, fillvalue=''
                  # Convert to list because Table expects it.
                  ).as_list()

    return '\n'.join(RST().write(Table(rows=it)))


def create_included_function_list_readme():
    """Creates an RST table to insert in the "Readme.rst" file for the
    complete overview of the package.

    Requires `astropy`!
    """
    import iteration_utilities
    from iteration_utilities import Iterable
    from itertools import chain
    from operator import itemgetter
    from astropy.table import Table
    from astropy.io.ascii import RST

    rtd_link = '`{name} <http://iteration-utilities.readthedocs.io/en/latest/api/{file}.html#{module}.{name}>`_'

    module_to_file = {'iteration_utilities': 'cfuncs',
                      'iteration_utilities._cfuncs': 'cfuncs',
                      'iteration_utilities._helpers._performance': 'helper',
                      'iteration_utilities._recipes._additional': 'additional',
                      'iteration_utilities._recipes._core': 'core',
                     }

    it = Iterable(chain(iteration_utilities._cfuncs.__dict__.items(),
                        iteration_utilities._helpers._performance.__dict__.items(),
                        iteration_utilities._recipes._core.__dict__.items(),
                        iteration_utilities._recipes._additional.__dict__.items())
                  # Exclude PY2 variable and private functions
                  ).filterfalse(lambda x: x[0].startswith(('PY2', '_'))
                  # Exclude everything that has no __module__
                  ).filter(lambda x: hasattr(x[1], '__module__')
                  # Only include functions that come from the package
                  ).filter(lambda x: x[1].__module__.startswith('iteration_utilities')
                  # Remove duplicate names
                  ).unique_everseen(itemgetter(0)
                  # Sort lexically
                  ).get_sorted(key=lambda x: x[0].lower())

    it = Iterable(it
                  # Create a Sphinx link from function name and module
                  ).map(lambda i: rtd_link.format(file = module_to_file[i[1].__module__],
                                                  module = i[1].__module__,
                                                  name = i[0])
                  # Group into 4s so we get a 4 column Table
                  ).grouper(4, fillvalue=''
                  # Convert to list because Table expects it.
                  ).as_list()

    return '\n'.join(RST().write(Table(rows=it)))
