/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/


/******************************************************************************
 * This file contains functions that are meant as helpers, they are especially
 * written to speed up parts of the Python code, they shouldn't be considered
 * safe to use elsewhere.
 *****************************************************************************/

static PyObject *
PyIU_TupleToList_and_InsertItemAtIndex(PyObject *m,
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
PyIU_RemoveFromDictWhereValueIs(PyObject *m,
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


PyDoc_STRVAR(PyIU_TupleToList_and_InsertItemAtIndex_doc, "_parse_args(tup, item, pos, /)\n\
--\n\
\n\
Converts the `tup` to a new `tuple` and inserts `item` at `pos`.\n\
\n\
.. warning::\n\
   This function is especially made for internal use, **DO NOT USE THIS \n\
   FUNCTION** anywhere else.\n\
\n\
Parameters\n\
----------\n\
tup : tuple\n\
    The tuple to convert.\n\
    \n\
    .. warning::\n\
       This function will encounter a segmentation fault if `tup` is not\n\
       a tuple.\n\
\n\
item : any type\n\
    The item to insert.\n\
\n\
pos : int\n\
    The position where to insert the `item`. \n\
    \n\
    .. warning::\n\
       No bounds checking - If `pos` is not carefully chosen the function \n\
       will segfault!\n\
\n\
Returns\n\
-------\n\
lst : tuple\n\
    The converted `tup` with `item` inserted.\n\
\n\
Notes\n\
-----\n\
This is equivelant to:\n\
\n\
.. code::\n\
\n\
    def _parse_args(args, item, pos):\n\
        return tuple(args[:pos]) + (item, ) + tuple(args[pos:])");


PyDoc_STRVAR(PyIU_RemoveFromDictWhereValueIs_doc, "_parse_kwargs(dct, item, /)\n\
--\n\
\n\
Removes every key from the `dct` where the ``dct[key] is item``.\n\
\n\
.. warning::\n\
   This function is especially made for internal use, **DO NOT USE THIS \n\
   FUNCTION** anywhere else.\n\
\n\
Parameters\n\
----------\n\
dct : dict\n\
    The dictionary from which to remove the keys.\n\
\n\
item : any type\n\
    The item to check for.\n\
\n\
Returns\n\
-------\n\
nothing. This function works in-place.\n\
\n\
Notes\n\
-----\n\
This is equivelant to:\n\
\n\
.. code::\n\
\n\
    def _parse_kwargs(dct, item):\n\
        keys_to_remove = [key for key in dct if dct[key] is item]\n\
        for key in keys_to_remove:\n\
            del dct[key]");
