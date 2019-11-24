#ifndef PYIU_DEEPFLATTEN_H
#define PYIU_DEEPFLATTEN_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

typedef struct {
    PyObject_HEAD
    PyObject *iteratorlist;
    PyObject *types;
    PyObject *ignore;
    Py_ssize_t depth;
    Py_ssize_t currentdepth;
    int isstring;
} PyIUObject_DeepFlatten;

extern PyTypeObject PyIUType_DeepFlatten;

#ifdef __cplusplus
}
#endif

#endif
