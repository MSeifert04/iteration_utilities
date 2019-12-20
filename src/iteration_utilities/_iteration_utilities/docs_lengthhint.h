#ifndef PYIU_DOCSLENGTHHINT_H
#define PYIU_DOCSLENGTHHINT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyDoc_STRVAR(
    PYIU_lenhint_doc,
    "__length_hint__(/)\n"
    "--\n\n"
    "Return an *estimate* for the length of the iterator or zero.\n");

#ifdef __cplusplus
}
#endif

#endif
