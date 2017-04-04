/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 * return_identity : lambda o: o
 * return_called : lambda o: o()
 * return_first_arg : (roughly) lambda *args, **kwargs: args[0]
 *****************************************************************************/

static PyObject *
PyIU_ReturnIdentity(PyObject *m,
                    PyObject *o)
{
    Py_INCREF(o);
    return o;
}

static PyObject *
PyIU_ReturnCalled(PyObject *m,
                  PyObject *o)
{
    return PyObject_CallFunctionObjArgs(o, NULL);
}

static PyObject *
PyIU_ReturnFirstArg(PyObject *m,
                    PyObject *args,
                    PyObject *kwargs)
{
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
