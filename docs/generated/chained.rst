chained
=======

.. currentmodule:: iteration_utilities

.. autoclass:: chained

   .. method:: __call__(*args, **kwargs)

      Depending on the `reverse` and `all` argument the function returns:

      ======= ===== ===========================================================
      reverse all   returns
      ======= ===== ===========================================================
      False   False ``func_1(...(func_n(*args, **kwargs)))``
      True    False ``func_n(...(func_1(*args, **kwargs)))``
      False   True  ``(func_1(*args, **kwargs), ..., func_n(*args, **kwargs))``
      True    True  ``(func_n(*args, **kwargs), ..., func_1(*args, **kwargs))``
      ======= ===== ===========================================================