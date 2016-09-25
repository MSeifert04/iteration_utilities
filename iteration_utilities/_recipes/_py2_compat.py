"""
Compatibility layer
^^^^^^^^^^^^^^^^^^^
"""
# Built-ins
from __future__ import absolute_import, division, print_function
import operator

# This module
from .. import PY2

__all__ = ['accumulate']


if PY2:
    def accumulate(iterable, func=operator.add):
        """Make an iterator that returns accumulated sums, or accumulated
        results of other binary functions (specified via the optional `func`
        argument). Taken from [0]_.

        Parameters
        ----------
        iterable : iterable
            The `iterable` to accumulate.

        func : callable, optional
            The function with which to accumulate. Should be a function of two
            arguments.
            Default is :py:func:`operator.add`.

        Returns
        -------
        accumulated : generator
            The accumulated results as generator.

        Notes
        -----
        Elements of the input `iterable` may be any type that can be
        accepted as arguments to `func`. (For example, with the default
        operation of addition, elements may be any addable type including
        Decimal or Fraction.) If the input `iterable` is empty, the output
        iterable will also be empty.

        Examples
        --------
        There are a number of uses for the `func` argument. It can be set to
        :py:func:`min` for a running minimum, :py:func:`max` for a running
        maximum, or :py:func:`operator.mul` for a running product. Amortization
        tables can be built by accumulating interest and applying payments.
        First-order recurrence relations can be modeled by supplying the
        initial value in the `iterable` and using only the accumulated total in
        `func` argument::

            >>> from iteration_utilities import accumulate
            >>> from itertools import repeat
            >>> import operator

            >>> data = [3, 4, 6, 2, 1, 9, 0, 7, 5, 8]
            >>> list(accumulate(data, operator.mul))     # running product
            [3, 12, 72, 144, 144, 1296, 0, 0, 0, 0]
            >>> list(accumulate(data, max))              # running maximum
            [3, 4, 6, 6, 6, 9, 9, 9, 9, 9]

        Amortize a 5% loan of 1000 with 4 annual payments of 90::

            >>> cashflows = [1000, -90, -90, -90, -90]
            >>> list(accumulate(cashflows, lambda bal, pmt: bal*1.05 + pmt))
            [1000, 960.0, 918.0, 873.9000000000001, 827.5950000000001]

        Chaotic recurrence relation [1]_::

            >>> logistic_map = lambda x, _:  r * x * (1 - x)
            >>> r = 3.8
            >>> x0 = 0.4
            >>> inputs = repeat(x0, 36)     # only the initial value is used
            >>> [format(x, '.2f') for x in accumulate(inputs, logistic_map)]
            ['0.40', '0.91', '0.30', '0.81', '0.60', '0.92', '0.29', '0.79', \
'0.63', '0.88', '0.39', '0.90', '0.33', '0.84', '0.52', '0.95', '0.18', \
'0.57', '0.93', '0.25', '0.71', '0.79', '0.63', '0.88', '0.39', '0.91', \
'0.32', '0.83', '0.54', '0.95', '0.20', '0.60', '0.91', '0.30', '0.80', '0.60']

        References
        ----------
        .. [0] https://docs.python.org/3/library/itertools.html#itertools.accumulate
        .. [1] https://en.wikipedia.org/wiki/Logistic_map
        """
        it = iter(iterable)
        try:
            total = next(it)
        except StopIteration:
            return
        yield total
        for element in it:
            total = func(total, element)
            yield total
else:
    from itertools import accumulate
