/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 * This file contains functions that are meant as helpers, they are especially
 * written to speed up parts of the Python code, they shouldn't be considered
 * safe to use elsewhere.
 *****************************************************************************/

static PyObject *
PyIU_TupleToList_and_InsertItemAtIndex(PyObject *Py_UNUSED(m),
                                       PyObject *args)
{
    PyObject *tup;
    PyObject *item;
    Py_ssize_t index;

    Py_ssize_t tupsize;
    Py_ssize_t i;
    PyObject *newtup;

    if (!PyArg_ParseTuple(args, "OOn:_parse_args", &tup, &item, &index)) {
        return NULL;
    }

    tupsize = PyTuple_GET_SIZE(tup);

    newtup = PyTuple_New(tupsize + 1);
    if (newtup == NULL) {
        return NULL;
    }

    Py_INCREF(item);
    PyTuple_SET_ITEM(newtup, index, item);

    for (i = 0 ; i < tupsize + 1 ; i++ ) {
        PyObject *tmp;
        if (i < index) {
            tmp = PyTuple_GET_ITEM(tup, i);
            Py_INCREF(tmp);
            PyTuple_SET_ITEM(newtup, i, tmp);
        } else if (i == index) {
            continue;
        } else {
            tmp = PyTuple_GET_ITEM(tup, i - 1);
            Py_INCREF(tmp);
            PyTuple_SET_ITEM(newtup, i, tmp);
        }
    }
    return newtup;
}


static PyObject *
PyIU_RemoveFromDictWhereValueIs(PyObject *Py_UNUSED(m),
                                PyObject *args)
{
    PyObject *dct;
    PyObject *remvalue;
    PyObject *key;
    PyObject *value;

    PyObject **toberemoved = NULL;

    Py_ssize_t pos;
    Py_ssize_t dctsize;
    Py_ssize_t i;
    Py_ssize_t j;

    if (!PyArg_ParseTuple(args, "OO:_parse_kwargs", &dct, &remvalue)) {
        return NULL;
    }
    dctsize = PyDict_Size(dct);
    if (dctsize == 0) {
        Py_RETURN_NONE;
    }

    toberemoved = malloc((size_t)dctsize * sizeof(PyObject*));

    if (toberemoved == NULL) {
        PyErr_SetString(PyExc_MemoryError,
                        "Memory Error when trying to allocate temporary array.");
        return NULL;
    }

    pos = 0;
    i = 0;
    while (PyDict_Next(dct, &pos, &key, &value)) {
        /* Compare the "value is remvalue" (this is not "value == remvalue"
           at least in the python-sense). */
        if (value == remvalue) {
            toberemoved[i] = key;
            i++;
        }
    }

    if (i == dctsize) {
        PyDict_Clear(dct);
    } else {
        for (j = 0 ; j < i ; j++) {
            PyDict_DelItem(dct, toberemoved[j]);
        }
    }
    free(toberemoved);
    Py_RETURN_NONE;
}
