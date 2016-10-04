static PyObject *
reduce_groupby(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"iterable", "groupbykey", "keepkey", NULL};

    PyObject *iterable, *key1;  // mandatory
    PyObject *key2=NULL;        // optional

    PyObject *iterator, *item, *val, *lst, *keep;
    int ok;

    PyObject *resdict;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO|O:groupby",
                                     kwargs, &iterable, &key1, &key2)) {
        return NULL;
    }

    /* Get iterator. */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    resdict = PyDict_New();
    if (resdict == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }

    while ( (item = PyIter_Next(iterator)) ) {
        val = PyObject_CallFunctionObjArgs(key1, item, NULL);
        if (val == NULL) {
            Py_DECREF(iterator);
            Py_DECREF(resdict);
            Py_DECREF(item);
            return NULL;
        }

        if (key2 == NULL) {
            keep = item;
        } else {
            keep = PyObject_CallFunctionObjArgs(key2, item, NULL);
            if (val == NULL) {
                Py_DECREF(iterator);
                Py_DECREF(resdict);
                Py_DECREF(item);
                Py_DECREF(val);
                return NULL;
            }
            Py_DECREF(item);
        }

        lst = PyDict_GetItem(resdict, val);  // ignores any exception!!!
        if (lst == NULL) {
            lst = PyList_New(1);
            PyList_SET_ITEM(lst, 0, keep);
            ok = PyDict_SetItem(resdict, val, lst);
            if (ok < 0) {
                Py_DECREF(iterator);
                Py_DECREF(resdict);
                Py_DECREF(lst);
                return NULL;
            }
        } else {
            ok = PyList_Append(lst, keep);
            if (ok < 0) {
                Py_DECREF(iterator);
                Py_DECREF(resdict);
                Py_DECREF(keep);
                return NULL;
            }
        }
    }
    Py_DECREF(iterator);

    if (PyErr_Occurred()) {
        Py_DECREF(resdict);
        return NULL;
    }

    return resdict;
}


PyDoc_STRVAR(reduce_groupby_doc, "groupby(iterable)\n\
\n\
");
