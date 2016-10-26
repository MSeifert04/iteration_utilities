Tipps and Tricks
----------------

Sometimes it is not hard to speed up some simple tasks. This page shows some
templates that might help to improve the performance of your code.

.. note::
   This page is more or less a stub right now. If you have any interesting
   facts to share please open a Pull Request or Issue.


Map
^^^

:py:func:`map` can be much faster than list comprehensions or generator
expressions *if and only if* the `function` is implemented in C without
Python attribute lookup.

All Python builtins are written in C and some (there are exceptions like
:py:func:`abs`) that perform really fast with :py:func:`map`:

.. code:: python

    >>> import random
    >>> l1 = [random.randint(0, 1000) for _ in range(20000)]
    >>> l2 = [random.randint(0, 1000) for _ in range(20000)]
    >>> l3 = [random.randint(0, 1000) for _ in range(20000)]
    >>> %timeit [min(i) for i in zip(l1, l2, l3)]
    100 loops, best of 3: 4.94 ms per loop

    >>> %timeit list(map(min, l1, l2, l3))
    100 loops, best of 3: 3.24 ms per loop

Sometimes it is not possible to use such a function directly with
:py:func:`map` but before you use :py:func:`functools.partial` you can always
use :py:func:`itertools.repeat`!

.. code:: python

    >>> from itertools import repeat
    >>> lst = [0]*100000
    >>> %timeit [isinstance(i, int) for i in lst]
    100 loops, best of 3: 17.4 ms per loop

    >>> %timeit list(map(isinstance, lst, repeat(int)))
    100 loops, best of 3: 7.99 ms per loop

.. note::
    Using :py:func:`itertools.repeat` is only faster for very **few**
    functions. :py:func:`isinstance` is one of those!


Predicate functions
^^^^^^^^^^^^^^^^^^^

Sometimes one needs a predicate function or filter out some items. One little
(although sometimes impossible!) trick is to use methods as predicate:

.. code:: python

    >>> import random
    >>> from iteration_utilities import consume
    >>> lst = [random.random() for _ in range(200000)]
    >>> %timeit consume((i for i in lst if i > 0.5), None)
    100 loops, best of 3: 9.51 ms per loop

    >>> %timeit consume(filter((0.5).__lt__, lst), None)
    100 loops, best of 3: 8.03 ms per loop

This shows only a slight improvement but it's not always possible to use a
generator expression or list comprehension. If you do the same with
:py:func:`operator.lt` and :py:func:`functools.partial` or with a custom
function you'll see the performance increase:

.. code:: python

    >>> from functools import partial
    >>> from operator import lt
    >>> partial_gt_05 = partial(lt, 0.5)
    >>> %timeit consume(filter(lambda x: x > 0.5, lst), None)
    10 loops, best of 3: 22.3 ms per loop

    >>> %timeit consume(filter(partial_gt_05, lst), None)
    100 loops, best of 3: 17 ms per loop

.. warning::
    Using the ``__lt__`` and equivalent methods is not always possible, for
    example it's not possible in Python2 and this bypasses Pythons operator
    evaluation. For example the following will fail: ``(5).__lt__(10.2)``
    because integer don't compare to floats. In that case you need to use:
    ``(5.0).__lt__(10.2)``.

However public methods are always avaiable as well as several special methods
like: ``__len__``, ``__contains__``, ...
