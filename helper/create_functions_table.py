# Licensed under Apache License Version 2.0 - see LICENSE
"""This is a helper that prints the content of the function overview tables .

- docs/index.rst
- README.rst

Both contain a table of functions defined in iteration_utilities and
manually updating them is a pain. Therefore this file can be executed and the
contents can be copy pasted there. Just use::

    >>> python helper/create_functions_table.py

Unfortunately the header lines of these tables have to be removed manually,
I haven't found a way to remove them programmatically using the
astropy.io.ascii.RST class.

It's actually important to call this helper from the main repo directory
so the file resolution works correctly.
"""


def _create_overview_table(repo_path, readme=False):
    """Creates an RST table to insert in the "Readme.rst" file for the
    complete overview of the package.

    Requires `astropy`!
    """
    from iteration_utilities import Iterable
    from astropy.table import Table
    from astropy.io.ascii import RST
    import pathlib

    p = pathlib.Path(repo_path).joinpath('docs', 'generated')

    funcs = sorted([file.name.split('.rst')[0] for file in p.glob('*.rst')],
                   key=str.lower)

    if readme:
        rtd_link = ('`{0} <https://iteration-utilities.readthedocs.io/'
                    'en/latest/generated/{0}.html>`_')
    else:
        rtd_link = ':py:func:`~iteration_utilities.{0}`'

    it = (Iterable(funcs)
          # Create a Sphinx link from function name and module
          .map(rtd_link.format)
          # Group into 4s so we get a 4 column Table
          .grouper(4, fillvalue='')
          # Convert to list because Table expects it.
          .as_list())

    print('\n'.join(RST().write(Table(rows=it))))


if __name__ == '__main__':
    import pathlib
    repo_path = pathlib.Path.cwd()
    _create_overview_table(repo_path=repo_path, readme=False)
    print('\n\n\n')
    _create_overview_table(repo_path=repo_path, readme=True)
