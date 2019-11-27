#ifndef PYIU_PLACEHOLDER_H
#define PYIU_PLACEHOLDER_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

extern PyObject PlaceholderStruct;
extern PyTypeObject Placeholder_Type;
#define PYIU_Placeholder (&PlaceholderStruct)

#ifdef __cplusplus
}
#endif

#endif
