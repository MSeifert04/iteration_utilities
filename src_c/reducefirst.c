static PyObject *
reduce_first(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *sequence, *iterator;
    PyObject *defaultitem = NULL, *func = NULL;
    PyObject *item = NULL, *val = NULL;
    long ok;

    static char *kwlist[] = {"iterable", "default", "pred", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OO:first", kwlist,
                                     &sequence, &defaultitem, &func)) {
        return NULL;
    }

    iterator = PyObject_GetIter(sequence);
    if (iterator == NULL) {
        return NULL;
    }

    // Iterate over the sequence
    while (( item = PyIter_Next(iterator) )) {

        // Sequence contains an element and func is None: return it.
        if (func == NULL) {
            Py_DECREF(iterator);
            return item;

        } else if (func == Py_None || func == (PyObject *)&PyBool_Type) {
            ok = PyObject_IsTrue(item);

        } else {
            val = PyObject_CallFunctionObjArgs(func, item, NULL);
            if (val == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(item);
                return NULL;
            }
            ok = PyObject_IsTrue(val);
            Py_DECREF(val);
        }

        if (ok == 1) {
            Py_DECREF(iterator);
            return item;

        } else if (ok < 0) {
            Py_DECREF(iterator);
            Py_DECREF(item);
            return NULL;
        }
        Py_DECREF(item);
    }

    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        return NULL;
    }

    if (defaultitem == NULL) {
        PyErr_Format(PyExc_TypeError, "no first element found.");
        return NULL;
    // Otherwise return the default item
    } else {
        // Does it need to be incref'd?
        Py_INCREF(defaultitem);
        return defaultitem;
    }
}


PyDoc_STRVAR(reduce_first_doc, "first(iterable[, default, pred])");
