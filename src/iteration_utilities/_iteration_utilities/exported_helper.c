/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "exported_helper.h"
#include "helper.h"

/******************************************************************************
 * This file contains functions that are meant as helpers, they are especially
 * written to speed up parts of the Python code, they shouldn't be considered
 * safe to use elsewhere.
 *****************************************************************************/

static PyObject *
PyIU_parse_args(PyObject *tuple, PyObject *item, Py_ssize_t index) {
    assert(tuple != NULL && PyTuple_CheckExact(tuple));
    assert(item != NULL);
    assert(index >= 0 && index <= PyTuple_GET_SIZE(tuple));

    PyObject *new_tuple;
    Py_ssize_t i;
    Py_ssize_t tuple_size = PyTuple_GET_SIZE(tuple);

    new_tuple = PyTuple_New(tuple_size + 1);
    if (new_tuple == NULL) {
        return NULL;
    }

    Py_INCREF(item);
    PyTuple_SET_ITEM(new_tuple, index, item);

    for (i = 0; i < tuple_size + 1; i++) {
        PyObject *tmp;
        if (i < index) {
            tmp = PyTuple_GET_ITEM(tuple, i);
            Py_INCREF(tmp);
            PyTuple_SET_ITEM(new_tuple, i, tmp);
        } else if (i == index) {
            continue;
        } else {
            tmp = PyTuple_GET_ITEM(tuple, i - 1);
            Py_INCREF(tmp);
            PyTuple_SET_ITEM(new_tuple, i, tmp);
        }
    }
    return new_tuple;
}

static PyObject *
PyIU_parse_kwargs(PyObject *dct, PyObject *remvalue) {
    assert(dct != NULL && PyDict_CheckExact(dct));
    assert(remvalue != NULL);

    PyObject *key;
    PyObject *value;

    PyObject *small_stack[PyIU_SMALL_ARG_STACK_SIZE];
    PyObject **stack = small_stack;

    Py_ssize_t pos;
    Py_ssize_t dict_size;
    Py_ssize_t i;
    Py_ssize_t j;

    dict_size = PyDict_Size(dct);
    if (dict_size == 0) {
        Py_RETURN_NONE;
    }

    if (dict_size > PyIU_SMALL_ARG_STACK_SIZE) {
        stack = PyIU_AllocatePyObjectArray(dict_size);
        if (stack == NULL) {
            return PyErr_NoMemory();
        }
    }

    pos = 0;
    i = 0;
    while (PyDict_Next(dct, &pos, &key, &value)) {
        /* Compare the "value is remvalue" (this is not "value == remvalue"
           at least in the python-sense). */
        if (value == remvalue) {
            stack[i] = key;
            i++;
        }
    }

    if (i == dict_size) {
        PyDict_Clear(dct);
    } else {
        for (j = 0; j < i; j++) {
            /* Error checking is intentionally omitted since we know that the
               items in the stack are not-NULL and hashable. */
            PyDict_DelItem(dct, stack[j]);
        }
    }

    if (stack != small_stack) {
        PyMem_Free(stack);
    }
    Py_RETURN_NONE;
}

#if PyIU_USE_VECTORCALL
PyObject *
PyIU_TupleToList_and_InsertItemAtIndex(PyObject *Py_UNUSED(m), PyObject *const *args, size_t nargs) {
    PyObject *tup;
    PyObject *item;
    Py_ssize_t index;
    if (!_PyArg_ParseStack(args, nargs, "OOn:_parse_args", &tup, &item, &index)) {
        return NULL;
    }
    return PyIU_parse_args(tup, item, index);
}

PyObject *
PyIU_RemoveFromDictWhereValueIs(PyObject *Py_UNUSED(m), PyObject *const *args, size_t nargs) {
    PyObject *dct;
    PyObject *remvalue;
    if (!_PyArg_ParseStack(args, nargs, "OO:_parse_kwargs", &dct, &remvalue)) {
        return NULL;
    }
    return PyIU_parse_kwargs(dct, remvalue);
}

#else
PyObject *
PyIU_TupleToList_and_InsertItemAtIndex(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *tup;
    PyObject *item;
    Py_ssize_t index;
    if (!PyArg_ParseTuple(args, "OOn:_parse_args", &tup, &item, &index)) {
        return NULL;
    }
    return PyIU_parse_args(tup, item, index);
}

PyObject *
PyIU_RemoveFromDictWhereValueIs(PyObject *Py_UNUSED(m), PyObject *args) {
    PyObject *dct;
    PyObject *remvalue;
    if (!PyArg_ParseTuple(args, "OO:_parse_kwargs", &dct, &remvalue)) {
        return NULL;
    }
    return PyIU_parse_kwargs(dct, remvalue);
}
#endif
