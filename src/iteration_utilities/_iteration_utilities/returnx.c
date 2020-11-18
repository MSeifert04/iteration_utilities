/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "returnx.h"
#include "helper.h"

/******************************************************************************
 * return_identity : lambda o: o
 * return_called : lambda o: o()
 * return_first_arg : (roughly) lambda *args, **kwargs: args[0]
 *****************************************************************************/

PyObject *
PyIU_ReturnIdentity(PyObject *Py_UNUSED(m), PyObject *o) {
    Py_INCREF(o);
    return o;
}

PyObject *
PyIU_ReturnCalled(PyObject *Py_UNUSED(m), PyObject *o) {
    return PyIU_CallWithNoArgument(o);
}

PyObject *
PyIU_ReturnFirstArg(PyObject *Py_UNUSED(m), PyObject *args, PyObject *Py_UNUSED(kwargs)) {
    PyObject *first;

    if (!PyTuple_CheckExact(args) || PyTuple_GET_SIZE(args) == 0) {
        PyErr_SetString(PyExc_TypeError,
                        "`return_first_arg` expected at least one positional "
                        "argument.");
        return NULL;
    }

    first = PyTuple_GET_ITEM(args, 0);
    Py_INCREF(first);
    return first;
}
