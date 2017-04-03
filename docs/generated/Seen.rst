Seen
====

.. currentmodule:: iteration_utilities

.. autoclass:: Seen

   .. method:: __contains__(x)

      Returns if :py:class:`~iteration_utilities.Seen` contains `x`; either in
      :py:attr:`.seenset` if `x` is hashable or :py:attr:`.seenlist` if not.

   .. method:: __len__()

      Returns the number of items in :py:attr:`.seenset` and
      :py:attr:`.seenlist`.

   .. method:: __eq__(other)

      Check if the `other` :py:class:`~iteration_utilities.Seen` instance
      contains the same elements.

   .. method:: __ne__(other)

      Check if the `other` :py:class:`~iteration_utilities.Seen` instance
      contains different elements.
