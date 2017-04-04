# Licensed under Apache License Version 2.0 - see LICENSE


def create_included_function_list(repo_path):
    """Creates an RST table to insert in the "docs/index.rst" file for the
    complete overview of the package.

    Requires `astropy`!
    """
    from iteration_utilities import Iterable
    from astropy.table import Table
    from astropy.io.ascii import RST
    import pathlib
    from os import path

    p = pathlib.Path(path.join(repo_path, 'docs', 'generated'))

    funcs = sorted([file.name.split('.rst')[0] for file in p.glob('*.rst')],
                   key=str.lower)

    it = (Iterable(funcs)
          # Create a Sphinx link from function name and module
          .map(lambda i: ':py:func:`~iteration_utilities.{}`'.format(i))
          # Group into 4s so we get a 4 column Table
          .grouper(4, fillvalue='')
          # Convert to list because Table expects it.
          .as_list())

    print('\n'.join(RST().write(Table(rows=it))))


def create_included_function_list_readme(repo_path):
    """Creates an RST table to insert in the "Readme.rst" file for the
    complete overview of the package.

    Requires `astropy`!
    """
    from iteration_utilities import Iterable
    from astropy.table import Table
    from astropy.io.ascii import RST
    import pathlib
    from os import path

    p = pathlib.Path(path.join(repo_path, 'docs', 'generated'))

    funcs = sorted([file.name.split('.rst')[0] for file in p.glob('*.rst')],
                   key=str.lower)

    rtd_link = '`{0} <http://iteration-utilities.readthedocs.io/en/latest/generated/{0}.html>`_'

    it = (Iterable(funcs)
          # Create a Sphinx link from function name and module
          .map(rtd_link.format)
          # Group into 4s so we get a 4 column Table
          .grouper(4, fillvalue='')
          # Convert to list because Table expects it.
          .as_list())

    print('\n'.join(RST().write(Table(rows=it))))
