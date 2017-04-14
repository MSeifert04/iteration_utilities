/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject *
PyIU_One(PyObject *m,
         PyObject *iterable)
{
    PyObject *iterator, *item1, *item2;

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    item1 = Py_TYPE(iterator)->tp_iternext(iterator);
    if (item1 == NULL) {
        Py_DECREF(iterator);
        if (PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
            } else {
                return NULL;
            }
        }
        PyErr_SetString(PyExc_ValueError,
                        "not enough values to unpack in `one` (expected 1, got 0)");
        return NULL;
    }

    item2 = Py_TYPE(iterator)->tp_iternext(iterator);
    if (item2 != NULL) {
        Py_DECREF(iterator);
        Py_DECREF(item1);
        Py_DECREF(item2);
        PyErr_SetString(PyExc_ValueError,
                        "too many values to unpack in `one` (expected 1).");
        return NULL;
    }

    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            Py_DECREF(item1);
            return NULL;
        }
    }

    return item1;
}
