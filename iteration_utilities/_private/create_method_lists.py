# Licensed under Apache License Version 2.0 - see LICENSE

"""
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Not working !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Shouldn't be used. :-)
"""

def _get_methods(cls, indentation_level):
    """To get all methods for a class. Useful only if the methods-list in
    either Iterable needs updates :-)
    """
    from iteration_utilities import Iterable, InfiniteIterable, ManyIterables
    from operator import itemgetter

    if cls is Iterable:
        name = "Iterable"
    elif cls is InfiniteIterable:
        name = "InfiniteIterable"
    elif cls is ManyIterables:
        name = "ManyIterables"
    else:
        raise TypeError()

    indent = ' ' * (4 * indentation_level)

    def name_and_doc(tup):
        # First item is the function name
        name = tup[0]
        # The second item is the function, we need to get the first line from
        # it. For not-staticmethods this is easy:
        doc = tup[1].__doc__.split('\n')[0]
        # Fortunatly staticmethods start their docstring with staticmethod so
        # this can be used to identify them.
        if doc.startswith('staticmethod'):
            # Just use the doc of the __func__ attribute of the staticmethod.
            doc = tup[1].__func__.__doc__.split('\n')[0]
        return name, doc

    # All methods
    res = []
    it = Iterable(i for c in cls.__mro__ for i in c.__dict__.items()).as_dict()
    for k, d in (Iterable(it.items())
                 .filterfalse(lambda x: x[0].startswith('_'))
                 .map(name_and_doc)
                 .get_sorted(key=itemgetter(0))):
        res.append('{}:py:meth:`~{}.{}`'.format(indent, name, k))
        res.append('{}    {}'.format(indent, d))
    return '\n'.join(res)

"""
This only works for Python3 :-(
Iterable.__init__.__doc__ = Iterable.__init__.__doc__.format(
    _get_methods(Iterable, 2))
InfiniteIterable.__init__.__doc__ = InfiniteIterable.__init__.__doc__.format(
    _get_methods(InfiniteIterable, 2))
ManyIterables.__init__.__doc__ = ManyIterables.__init__.__doc__.format(
    'itertools.imap' if PY2 else 'map', _get_methods(ManyIterables, 2))

# Currently inserting these manually.... :-(
"""
