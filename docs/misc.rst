Miscellanea
-----------

:py:mod:`iteration_utilities` provides some general utilities that were
useful in some of the implementations. Some of these might even be useful in
other contexts, so these are summarized here.

- :py:func:`~iteration_utilities.ItemIdxKey`, a class to facilitate stable
  sorting supporting `reverse` and `key`.
- :py:func:`~iteration_utilities.Seen`, a class that wraps a :py:class:`set`
  and a :py:class:`list` supporting
  :py:meth:`in <iteration_utilities.Seen.__contains__>` operations and a
  :py:meth:`~iteration_utilities.Seen.contains_add`
  method to facilitate keeping track of already seen objects (even if they are
  unhashable).
