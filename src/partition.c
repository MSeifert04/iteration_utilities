/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
PyIU_Partition(PyObject *m,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "pred", NULL};
    PyObject *iterable = NULL;
    PyObject *func = NULL;
    PyObject *iterator = NULL;
    PyObject *result1 = NULL;
    PyObject *result2 = NULL;
    PyObject *funcargs = NULL;
    PyObject *result = NULL;
    PyObject *(*iternext)(PyObject *);

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:partition", kwlist,
                                     &iterable, &func)) {
        goto Fail;
    }

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    iternext = *Py_TYPE(iterator)->tp_iternext;

    result1 = PyList_New(0);
    if (result1 == NULL) {
        goto Fail;
    }
    result2 = PyList_New(0);
    if (result2 == NULL) {
        goto Fail;
    }

    if (func == Py_None || func == (PyObject *)&PyBool_Type) {
        func = NULL;
    }

    if (func != NULL) {
        funcargs = PyTuple_New(1);
        if (funcargs == NULL) {
            goto Fail;
        }
    }

    for (;;) {
        PyObject *item;
        PyObject *temp;
        int ok;

        item = iternext(iterator);
        if (item == NULL) {
            break;
        }

        if (func == NULL) {
            temp = item;
            Py_INCREF(temp);
        } else {
            PYIU_RECYCLE_ARG_TUPLE(funcargs, item, Py_DECREF(item);
                                                   goto Fail);
            temp = PyObject_Call(func, funcargs, NULL);
            if (temp == NULL) {
                Py_DECREF(item);
                goto Fail;
            }
        }

        ok = PyObject_IsTrue(temp);
        Py_DECREF(temp);
        temp = NULL;

        if (ok == 1) {
            ok = PyList_Append(result2, item);
        } else if (ok == 0) {
            ok = PyList_Append(result1, item);
        }
        /* No need to check here if the "IsTrue" failed here. The "ok" variable
           is reused and the case where "IsTrue" failed and the case where
           "PyList_Append" failed can be handled in one go after decrementing
           the item!

        else {
            Py_DECREF(item);
            goto Fail;
        }
        */

        Py_DECREF(item);
        item = NULL;

        if (ok == -1) {
            goto Fail;
        }
    }

    Py_XDECREF(funcargs);
    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            Py_DECREF(result1);
            Py_DECREF(result2);
            return NULL;
        }
    }

    result = PyTuple_Pack(2, result1, result2);
    Py_DECREF(result1);
    Py_DECREF(result2);

    return result;

Fail:
    Py_XDECREF(funcargs);
    Py_XDECREF(result1);
    Py_XDECREF(result2);
    Py_XDECREF(iterator);
    return NULL;
}
