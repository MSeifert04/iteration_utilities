#ifndef PYIU_DOCSSIZEOF_H
#define PYIU_DOCSSIZEOF_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyDoc_STRVAR(
    PYIU_sizeof_doc,
    "__sizeof__($self, /)\n"
    "--\n\n"
    "Returns size in memory, in bytes.\n");

#ifdef __cplusplus
}
#endif

#endif
