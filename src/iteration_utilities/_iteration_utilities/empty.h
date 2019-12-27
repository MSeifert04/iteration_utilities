#ifndef PYIU_EMPTY_H
#define PYIU_EMPTY_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern PyObject EmptyStruct;
#define PYIU_Empty (&EmptyStruct)
extern PyTypeObject PyIUType_Empty;

#ifdef __cplusplus
}
#endif

#endif
