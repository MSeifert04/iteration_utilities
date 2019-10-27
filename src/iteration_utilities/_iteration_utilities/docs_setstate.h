#ifndef PYIU_DOCSSETSTATE_H
#define PYIU_DOCSSETSTATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyDoc_STRVAR(
    PYIU_setstate_doc,
    "__setstate__($self, state, /)\n"
    "--\n\n"
    "Set state for unpickling. "
    "The `state` argument must be `tuple`-like.\n");

#ifdef __cplusplus
}
#endif

#endif
