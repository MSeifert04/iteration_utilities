#ifndef PYIU_EMPTY_H
#define PYIU_EMPTY_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern PyTypeObject PyIUType_Empty;

PyObject * PyIUEmpty_New(void);

#ifdef __cplusplus
}
#endif

#endif
