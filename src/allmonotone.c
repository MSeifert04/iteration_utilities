/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
PyIU_Monotone(PyObject *m,
              PyObject *args,
              PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "decreasing", "strict", NULL};

    PyObject *iterable, *iterator, *item, *last=NULL;
    int decreasing=0, strict=0, op, ok;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ii:all_monotone", kwlist,
                                     &iterable, &decreasing, &strict)) {
        return NULL;
    }
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    op = decreasing ? (strict ? Py_GT : Py_GE) : (strict ? Py_LT : Py_LE);

    while ( (item = Py_TYPE(iterator)->tp_iternext(iterator)) ) {
        if (last == NULL) {
            last = item;
            continue;
        }
        ok = PyObject_RichCompareBool(last, item, op);
        Py_DECREF(last);
        last = item;
        if (ok == 0) {
            goto Found;
        } else if (ok == -1) {
            goto Fail;
        }
    }

    Py_XDECREF(iterator);
    Py_XDECREF(last);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            return NULL;
        }
    }
    Py_RETURN_TRUE;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(last);
    return NULL;

Found:
    Py_XDECREF(iterator);
    Py_XDECREF(last);
    Py_RETURN_FALSE;
}
