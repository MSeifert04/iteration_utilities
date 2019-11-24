#ifndef PYIU_SPLIT_H
#define PYIU_SPLIT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

enum PyIU_SplitKeepOption {
    PyIU_Split_KeepNone,
    PyIU_Split_Keep,
    PyIU_Split_KeepAfter,
    PyIU_Split_KeepBefore
};

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *delimiter;
    Py_ssize_t maxsplit;
    enum PyIU_SplitKeepOption keep;
    int cmp;
    PyObject *next;
} PyIUObject_Split;

extern PyTypeObject PyIUType_Split;

#ifdef __cplusplus
}
#endif

#endif
