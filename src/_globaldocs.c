/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 *
 * Documentation for __reduce__, __setstate__ and __length_hint__
 *
 * Parts are taken from the CPython package (PSF licensed).
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
