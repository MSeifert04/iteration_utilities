#ifndef PYIU_PLACEHOLDER_H
#define PYIU_PLACEHOLDER_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

extern PyTypeObject Placeholder_Type;

PyObject * PyIUPlaceholder_New(void);
int PyIUPlaceholder_IsPlaceholder(PyObject *o);

#ifdef __cplusplus
}
#endif

#endif
