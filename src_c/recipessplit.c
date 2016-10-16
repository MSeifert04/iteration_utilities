typedef struct {
    PyObject_HEAD
    PyObject *it;
    PyObject *delimiter;
    Py_ssize_t maxsplit;
    int keep_delimiter;
    int cmp;
    PyObject *next;
} PyIUObject_Split;

static PyTypeObject PyIUType_Split;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * split_new(PyTypeObject *type, PyObject *args,
                            PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "key", "maxsplit", "keep", "eq", NULL};
    PyIUObject_Split *lz;

    PyObject *iterable, *delimiter;
    Py_ssize_t maxsplit = -1;  // -1 means no maxsplit!
    int keep_delimiter = 0;
    int cmp = 0;
    PyObject *it;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|nii:split", kwlist,
                                     &iterable, &delimiter,
                                     &maxsplit, &keep_delimiter, &cmp)) {
        return NULL;
    }

    if (maxsplit <= -2) {
        PyErr_Format(PyExc_ValueError,
                     "`maxsplit` must be -1 or greater.");
        return NULL;
    }

    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        return NULL;
    }

    lz = (PyIUObject_Split *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(it);
        return NULL;
    }

    lz->it = it;
    Py_INCREF(delimiter);
    lz->delimiter = delimiter;
    lz->maxsplit = maxsplit;
    lz->keep_delimiter = keep_delimiter;
    lz->cmp = cmp;

    lz->next = NULL;

    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void split_dealloc(PyIUObject_Split *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->it);
    Py_XDECREF(lz->delimiter);
    Py_XDECREF(lz->next);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int split_traverse(PyIUObject_Split *lz, visitproc visit, void *arg) {
    Py_VISIT(lz->it);
    Py_VISIT(lz->delimiter);
    Py_VISIT(lz->next);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * split_next(PyIUObject_Split *lz) {
    PyObject *(*iternext)(PyObject *);
    PyObject *result, *item=NULL, *val=NULL;
    PyObject *it = lz->it;
    PyObject *next = lz->next;
    int ok;

    result = PyList_New(0);
    if (result == NULL) {
        goto Fail;
    }

    if (next != NULL) {
        ok = PyList_Append(result, next);
        Py_DECREF(lz->next);
        lz->next = NULL;
        if (ok == 0) {
            return result;
        } else {
            goto Fail;
        }
    }

    iternext = *Py_TYPE(it)->tp_iternext;
    while ( (item = iternext(it)) ) {
        if (lz->cmp) {
            ok = PyObject_RichCompareBool(lz->delimiter, item, Py_EQ);

        } else {
            val = PyObject_CallFunctionObjArgs(lz->delimiter, item, NULL);
            if (val == NULL) {
                goto Fail;
            }

            ok = PyObject_IsTrue(val);
        }

        if (ok == -1) {
            goto Fail;

        } else if (ok == 0 || lz->maxsplit == 0) {
            ok = PyList_Append(result, item);
            if (ok != 0) {
                goto Fail;
            }
            Py_DECREF(item);
            Py_XDECREF(val);

        } else if (ok == 1) {
            if (lz->maxsplit != -1) {
                lz->maxsplit--;
            }
            Py_XDECREF(val);
            if (lz->keep_delimiter) {
                lz->next = item;
            } else {
                Py_DECREF(item);
            }
            return result;

        }
    }

    PYIU_CLEAR_STOPITERATION;

    if (PyList_GET_SIZE(result) == 0) {
        Py_DECREF(result);
        return NULL;
    } else {
        return result;
    }


Fail:
    Py_XDECREF(result);
    Py_XDECREF(item);
    Py_XDECREF(val);
    return NULL;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * split_reduce(PyIUObject_Split *lz) {
    if (lz->next == NULL) {
        return Py_BuildValue("O(OOnii)", Py_TYPE(lz),
                             lz->it,
                             lz->delimiter,
                             lz->maxsplit,
                             lz->keep_delimiter,
                             lz->cmp);
    } else {
        return Py_BuildValue("O(OOnii)(O)", Py_TYPE(lz),
                             lz->it,
                             lz->delimiter,
                             lz->maxsplit,
                             lz->keep_delimiter,
                             lz->cmp,
                             lz->next);
    }
}

/******************************************************************************
 *
 * Setstate
 *
 *****************************************************************************/

static PyObject * split_setstate(PyIUObject_Split *lz, PyObject *state) {
    PyObject *next;

    if (!PyArg_ParseTuple(state, "O", &next)) {
        return NULL;
    }

    Py_CLEAR(lz->next);
    Py_INCREF(next);
    lz->next = next;

    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef split_methods[] = {
    {"__reduce__", (PyCFunction)split_reduce, METH_NOARGS, ""},
    {"__setstate__", (PyCFunction)split_setstate, METH_O, ""},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(split_doc, "split(iterable, key[, maxsplit, keep, eq])\n\
\n\
Splits an `iterable` by a `key`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` to split.\n\
\n\
key : callable\n\
    The function by which to split the `iterable` (split where\n\
    ``key(item) == True``).\n\
\n\
maxsplit : int, optional\n\
    The number of maximal splits. If ``maxsplit=-1`` then there is no limit.\n\
    Default is ``-1``.\n\
\n\
keep : bool\n\
    If ``True`` also include the items where ``key(item)=True``.\n\
    Default is ``False``.\n\
\n\
eq : bool\n\
    If ``True`` split the `iterable` where ``key == item`` instead of\n\
    ``key(item) == True``. This can significantly speedup the function is a\n\
    simple delimiter is used.\n\
    Default is ``False``.\n\
\n\
Returns\n\
-------\n\
splitted_iterable : generator\n\
    Generator containing the split `iterable` as lists.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import split\n\
>>> list(split(range(1, 10), lambda x: x%3==0))\n\
[[1, 2], [4, 5], [7, 8]]\n\
\n\
>>> list(split(range(1, 10), lambda x: x%3==0, keep=True))\n\
[[1, 2], [3], [4, 5], [6], [7, 8], [9]]\n\
\n\
>>> list(split(range(1, 10), lambda x: x%3==0, maxsplit=1))\n\
[[1, 2], [4, 5, 6, 7, 8, 9]]\n\
\n\
>>> list(split([1,2,3,4,5,3,7,8,3], 3, eq=True))\n\
[[1, 2], [4, 5], [7, 8]]\n\
");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Split = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.split",      /* tp_name */
    sizeof(PyIUObject_Split),     /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)split_dealloc, /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    split_doc,                /* tp_doc */
    (traverseproc)split_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)split_next, /* tp_iternext */
    split_methods,            /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    split_new,                /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
