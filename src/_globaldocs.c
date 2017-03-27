/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

/******************************************************************************
 *
 * Documentation for __reduce__
 *
 * This section has a differing license because this was copied from the
 * itertoolsmodule.c file of CPython.
 *
 * Licensed under PSF license - see licenses/LICENSE_PYTHON.rst
 *
 *****************************************************************************/

PyDoc_STRVAR(PYIU_reduce_doc,
             "__reduce__(/)\n--\n\n"
             "Return a `tuple` containing the state information for pickling.");

PyDoc_STRVAR(PYIU_setstate_doc,
             "__setstate__(state, /)\n--\n\n"
             "Set state for unpickling. "
             "The `state` argument must be `tuple`-like.");

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
PyDoc_STRVAR(PYIU_lenhint_doc,
             "__length_hint__(/)\n--\n\n"
             "Return an *estimate* for the length of the iterator or zero.");
#endif
