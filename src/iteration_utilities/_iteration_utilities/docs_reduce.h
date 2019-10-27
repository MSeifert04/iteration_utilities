#ifndef PYIU_DOCSREDUCE_H
#define PYIU_DOCSREDUCE_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyDoc_STRVAR(
    PYIU_reduce_doc,
    "__reduce__($self, /)\n"
    "--\n\n"
    "Return a `tuple` containing the state information for pickling.\n");

#ifdef __cplusplus
}
#endif

#endif
