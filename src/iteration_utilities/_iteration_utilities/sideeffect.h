#ifndef PYIU_SIDEEFFECT_H
#define PYIU_SIDEEFFECT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iterator;  /* iterator over data */
    PyObject *func;      /* Function to call */
    Py_ssize_t times;    /* Call side effects each x items */
    Py_ssize_t count;    /* Current counter when to call func */
    PyObject *collected; /* Collect items to pass to side-effects */
} PyIUObject_Sideeffects;

extern PyTypeObject PyIUType_Sideeffects;

#ifdef __cplusplus
}
#endif

#endif
