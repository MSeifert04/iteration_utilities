#ifndef PYIU_HELPER_H
#define PYIU_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "helpercompat.h"

#define PyIU_SMALL_ARG_STACK_SIZE 5

extern PyObject *PyIU_global_zero;
extern PyObject *PyIU_global_one;
extern PyObject *PyIU_global_two;
extern PyObject *PyIU_global_0tuple;

PyObject *PyIU_CreateIteratorTuple(PyObject *tuple);
PyObject *PyIU_TupleReverse(PyObject *tuple);
PyObject *PyIU_TupleCopy(PyObject *tuple);
void PyIU_TupleInsert(PyObject *tuple, Py_ssize_t where, PyObject *v, Py_ssize_t num);
void PyIU_TupleRemove(PyObject *tuple, Py_ssize_t where, Py_ssize_t num);
PyObject *PyIU_TupleGetSlice(PyObject *tuple, Py_ssize_t num);
void PyIU_InitializeConstants(void);

static inline int
PyIU_IsTypeExact(PyObject *obj, PyTypeObject *type) {
    #if PyIU_USE_BUILTIN_IS_TYPE
        return Py_IS_TYPE(obj, type);
    #else
        return Py_TYPE(obj) == type;
    #endif
}

static inline PyObject**
PyIU_AllocatePyObjectArray(Py_ssize_t num) {
    assert(num >= 0);
    return PyMem_Malloc((size_t)num * sizeof(PyObject *));
}

static inline int
PyIU_ErrorOccurredClearStopIteration() {
    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            return 1;
        }
    }
    return 0;
}

/******************************************************************************
 * Function call abstractions
 *
 * To support the different calling conventions across Python versions
 *****************************************************************************/

#if PyIU_USE_VECTORCALL
static inline PyObject*
PyIU_PyObject_Vectorcall(PyObject *callable, PyObject *const *args, size_t nargsf, PyObject *kwnames) {
    #if PyIU_USE_UNDERSCORE_VECTORCALL
        return _PyObject_Vectorcall(callable, args, nargsf, kwnames);
    #else
        return PyObject_Vectorcall(callable, args, nargsf, kwnames);
    #endif
}
#endif

static inline PyObject*
PyIU_CallWithNoArgument(PyObject *callable) {
    assert(callable != NULL);

    #if PyIU_USE_VECTORCALL && !PyIU_USE_UNDERSCORE_VECTORCALL
        return PyObject_CallNoArgs(callable);
    #else
        /* Or maybe PyObject_CallObject ... not sure*/
        return PyObject_CallFunctionObjArgs(callable, NULL);
    #endif
}

static inline PyObject*
PyIU_CallWithOneArgument(PyObject *callable, PyObject *arg1) {
    assert(callable != NULL);
    assert(arg1 != NULL);

    #if PyIU_USE_VECTORCALL
        #if PyIU_USE_UNDERSCORE_VECTORCALL
            PyObject *args[1];
            args[0] = arg1;
            return _PyObject_Vectorcall(callable, args, 1, NULL);
        #else
            return PyObject_CallOneArg(callable, arg1);
        #endif
    #elif PyIU_USE_FASTCALL
        PyObject *args[1];
        args[0] = arg1;
        return _PyObject_FastCall(callable, args, 1);
    #else
        PyObject *result;
        PyObject *args = PyTuple_New(1);
        if (args == NULL) {
            return NULL;
        }
        Py_INCREF(arg1);
        PyTuple_SET_ITEM(args, 0, arg1);
        result = PyObject_Call(callable, args, NULL);
        Py_DECREF(args);
        return result;
    #endif
}

static inline PyObject*
PyIU_CallWithTwoArguments(PyObject *callable, PyObject *arg1, PyObject *arg2) {
    assert(callable != NULL);
    assert(arg1 != NULL);
    assert(arg2 != NULL);

    #if PyIU_USE_VECTORCALL
        PyObject *args[2];
        args[0] = arg1;
        args[1] = arg2;
        return PyIU_PyObject_Vectorcall(callable, args, 2, NULL);
    #elif PyIU_USE_FASTCALL
        PyObject *args[2];
        args[0] = arg1;
        args[1] = arg2;
        return _PyObject_FastCall(callable, args, 2);
    #else
        PyObject *result;
        PyObject *args = PyTuple_New(2);
        if (args == NULL) {
            return NULL;
        }
        Py_INCREF(arg1);
        Py_INCREF(arg2);
        PyTuple_SET_ITEM(args, 0, arg1);
        PyTuple_SET_ITEM(args, 1, arg2);
        result = PyObject_Call(callable, args, NULL);
        Py_DECREF(args);
        return result;
    #endif
}

#define PyIU_USE_CPYTHON_INTERNALS PYIU_CPYTHON

static inline void
PyIU_CopyTupleToArray(PyObject *tuple, PyObject **array, size_t n_objects) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));
    assert(array != NULL);
    assert(PyTuple_GET_SIZE(tuple) >= (Py_ssize_t)n_objects);

    #if PyIU_USE_CPYTHON_INTERNALS
        memcpy(array, ((PyTupleObject *)tuple)->ob_item, n_objects * sizeof(PyObject *));
    #else
        Py_ssize_t i;
        for (i = 0; i < n_objects; i++) {
            array[i] = PyTuple_GET_ITEM(tuple, i);
        }
    #endif
}

static inline void
PyIU_CopyListToArray(PyObject *list, PyObject **array, size_t n_objects) {
    assert(list != NULL && PyList_CheckExact(list));
    assert(array != NULL);
    assert(PyList_GET_SIZE(list) >= (Py_ssize_t)n_objects);

    #if PyIU_USE_CPYTHON_INTERNALS
        memcpy(array, ((PyListObject *)list)->ob_item, n_objects * sizeof(PyObject *));
    #else
        Py_ssize_t i;
        for (i = 0; i < n_objects; i++) {
            array[i] = PyList_GET_ITEM(list, i);
        }
    #endif
}

#undef PyIU_USE_TUPLE_INTERNALS

#ifdef __cplusplus
}
#endif

#endif
