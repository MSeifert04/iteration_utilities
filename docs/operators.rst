Operators
---------

The Python operator module [0]_ contains a large variety of operators and
:py:mod:`iteration_utilities` only tries to fill in some missing ones:

Reverse arithmetic operators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.reciprocal`, equivalent to ``lambda x: 1 / x``

+-------------------------------------------+--------------------------+
| Function                                  |        Equivalent        |
+===========================================+==========================+
| :py:func:`~iteration_utilities.radd`      | ``lambda x, y: y + x``   |
+-------------------------------------------+--------------------------+
| :py:func:`~iteration_utilities.rsub`      | ``lambda x, y: y - x``   |
+-------------------------------------------+--------------------------+
| :py:func:`~iteration_utilities.rmul`      | ``lambda x, y: y * x``   |
+-------------------------------------------+--------------------------+
| :py:func:`~iteration_utilities.rdiv`      | ``lambda x, y: y / x``   |
+-------------------------------------------+--------------------------+
| :py:func:`~iteration_utilities.rfdiv`     | ``lambda x, y: y // x``  |
+-------------------------------------------+--------------------------+
| :py:func:`~iteration_utilities.rpow`      | ``lambda x, y: y ** x``  |
+-------------------------------------------+--------------------------+
| :py:func:`~iteration_utilities.rmod`      | ``lambda x, y: y % x``   |
+-------------------------------------------+--------------------------+

Math operators
^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.double`, equivalent to ``lambda x: x * 2``
- :py:func:`~iteration_utilities.square`, equivalent to ``lambda x: x ** 2``
- :py:func:`~iteration_utilities.reciprocal`, equivalent to ``lambda x: 1 / x``

And of course the standard operators from the operator module:

+------------------------------------------+--------------------------+
|          Function                        |        Equivalent        |
+==========================================+==========================+
| :py:func:`operator.add` (iadd)           | ``lambda x, y: x + y``   |
+------------------------------------------+--------------------------+
| :py:func:`operator.sub` (isub)           | ``lambda x, y: x - y``   |
+------------------------------------------+--------------------------+
| :py:func:`operator.mul` (imul)           | ``lambda x, y: x * y``   |
+------------------------------------------+--------------------------+
| :py:func:`operator.truediv` (itruediv)   | ``lambda x, y: x / y``   |
+------------------------------------------+--------------------------+
| :py:func:`operator.floordiv` (ifloordiv) | ``lambda x, y: x // y``  |
+------------------------------------------+--------------------------+
| :py:func:`operator.pow` (ipow)           | ``lambda x, y: x ** y``  |
+------------------------------------------+--------------------------+
| :py:func:`operator.mod` (imod)           | ``lambda x, y: x % y``   |
+------------------------------------------+--------------------------+
| :py:func:`operator.matmul` (imatmul)     | ``lambda x, y: x @ y``   |
+------------------------------------------+--------------------------+
| :py:func:`operator.abs`                  | ``lambda x: abs(x)``     |
+------------------------------------------+--------------------------+
| :py:func:`operator.pos`                  | ``lambda x: +x``         |
+------------------------------------------+--------------------------+
| :py:func:`operator.neg`                  | ``lambda x: -x``         |
+------------------------------------------+--------------------------+

And the bitwise operators:

+--------------------------------------+--------------------------+
|          Function                    |        Equivalent        |
+======================================+==========================+
| :py:func:`operator.lshift` (ilshift) | ``lambda x, y: x << y``  |
+--------------------------------------+--------------------------+
| :py:func:`operator.rshift` (irshift) | ``lambda x, y: x >> y``  |
+--------------------------------------+--------------------------+
| :py:func:`operator.and_` (iand)      | ``lambda x, y: x & y``   |
+--------------------------------------+--------------------------+
| :py:func:`operator.or_` (ior)        | ``lambda x, y: x | y``   |
+--------------------------------------+--------------------------+
| :py:func:`operator.xor` (ixor)       | ``lambda x, y: x ^ y``   |
+--------------------------------------+--------------------------+
| :py:func:`operator.inv`              | ``lambda x: ~x``         |
+--------------------------------------+--------------------------+

.. note::
   The :mod:`math` module contains several more!


Comparison operators
^^^^^^^^^^^^^^^^^^^^

- :py:func:`~iteration_utilities.is_even`, equivalent to
  ``lambda x: (x % 2) == 0``.
- :py:func:`~iteration_utilities.is_odd`, equivalent to
  ``lambda x: (x % 2) != 0``.
- :py:func:`~iteration_utilities.is_None`, equivalent to
  ``lambda x: x is None``.
- :py:func:`~iteration_utilities.is_not_None`, equivalent to
  ``lambda x: x is not None``.
- :py:func:`~iteration_utilities.is_iterable`, roughly equivalent to
  ``lambda x: isinstance(x, collections.Iterable)``.

And the comparison operators from the Python library:

+-----------------------------------+-----------------------------+
|          Function                 |        Equivalent           |
+===================================+=============================+
| :py:func:`operator.lt`            | ``lambda x, y: x < y``      |
+-----------------------------------+-----------------------------+
| :py:func:`operator.le`            | ``lambda x, y: x <= y``     |
+-----------------------------------+-----------------------------+
| :py:func:`operator.eq`            | ``lambda x, y: x == y``     |
+-----------------------------------+-----------------------------+
| :py:func:`operator.ne`            | ``lambda x, y: x |= y``     |
+-----------------------------------+-----------------------------+
| :py:func:`operator.ge`            | ``lambda x, y: x >= y``     |
+-----------------------------------+-----------------------------+
| :py:func:`operator.gt`            | ``lambda x, y: x < y``      |
+-----------------------------------+-----------------------------+
| :py:func:`operator.is_`           | ``lambda x, y: x is y``     |
+-----------------------------------+-----------------------------+
| :py:func:`operator.is_not`        | ``lambda x, y: x is not y`` |
+-----------------------------------+-----------------------------+
| :py:func:`operator.truth`         | ``lambda x: not not x``     |
+-----------------------------------+-----------------------------+
| :py:func:`operator.not_`          | ``lambda x: not x``         |
+-----------------------------------+-----------------------------+


Misc
^^^^

And some misc operators

+------------------------------------------+----------------------------------------------------+
|          Function                        |        Equivalent                                  |
+==========================================+====================================================+
| :py:func:`operator.index` *              | ``lambda x: x__index__()``                         |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.concat` (iconcat) *   | ``lambda x, y: x + y``                             |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.contains`             | ``lambda x, y: y in x``                            |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.countOf` *            | ``lambda x, y: y.count(x)``                        |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.indexOf` *            | ``lambda x, y: y.index(x)``                        |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.getitem`              | ``lambda x, y: x[y]``                              |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.setitem`              | ``lambda x, y, z: x[y] = z``                       |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.delitem`              | ``lambda x, y: del x[y]``                          |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.itemgetter` *         | ``lambda x, lambda y: y[x]``                       |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.attrgetter` *         | ``lambda x, lambda y: y.x``                        |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.methodcaller` *       | ``lambda x, lambda y: y.x()``                      |
+------------------------------------------+----------------------------------------------------+
| :py:func:`operator.length_hint` *        | ``lambda x, y: len(x) or x.__length_hint__() or y``|
+------------------------------------------+----------------------------------------------------+

Marked (``*``) functions only have a rough equivalent and may be more
sophisticated!


References
~~~~~~~~~~

.. [0] https://docs.python.org/library/operator.html
