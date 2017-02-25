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

PyDoc_STRVAR(PYIU_reduce_doc, "Return state information for pickling.");

PyDoc_STRVAR(PYIU_setstate_doc, "Set state information for unpickling.");

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
PyDoc_STRVAR(PYIU_lenhint_doc, "Return a length hint if possible.");
#endif
