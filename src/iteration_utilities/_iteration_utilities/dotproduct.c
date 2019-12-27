/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "dotproduct.h"
#include "helper.h"

static PyObject *
dot_product(PyObject *iterator1, PyObject *iterator2) {
    PyObject *item1;
    PyObject *result = NULL;

    while ((item1 = Py_TYPE(iterator1)->tp_iternext(iterator1))) {
        PyObject *item2;
        PyObject *product;

        item2 = Py_TYPE(iterator2)->tp_iternext(iterator2);
        if (item2 == NULL) {
            Py_DECREF(item1);
            Py_XDECREF(result);
            return NULL;
        }
        product = PyNumber_Multiply(item1, item2);
        Py_DECREF(item1);
        Py_DECREF(item2);
        if (product == NULL) {
            Py_XDECREF(result);
            return NULL;
        }

        if (result == NULL) {
            result = product;
        } else {
            PyObject *tmp = result;
            result = PyNumber_Add(result, product);
            Py_DECREF(product);
            Py_DECREF(tmp);
            if (result == NULL) {
                return NULL;
            }
        }
    }
    if (PyIU_ErrorOccurredClearStopIteration()) {
        Py_XDECREF(result);
        return NULL;
    }
    if (result == NULL) {
        result = PyLong_FromLong((long)0);
    }
    return result;
}

PyObject *
PyIU_DotProduct(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *vec1;
    PyObject *vec2;
    PyObject *iterator1;
    PyObject *iterator2;
    PyObject *result;

    if (!PyArg_ParseTuple(args, "OO", &vec1, &vec2)) {
        return NULL;
    }
    iterator1 = PyObject_GetIter(vec1);
    if (iterator1 == NULL) {
        return NULL;
    }
    iterator2 = PyObject_GetIter(vec2);
    if (iterator2 == NULL) {
        Py_DECREF(iterator1);
        return NULL;
    }
    result = dot_product(iterator1, iterator2);
    Py_DECREF(iterator1);
    Py_DECREF(iterator2);
    return result;
}
