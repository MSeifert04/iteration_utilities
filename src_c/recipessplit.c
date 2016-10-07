typedef struct {
    PyObject_HEAD
    PyObject *it;
    PyObject *delimiter;
    Py_ssize_t maxsplit;
    int keep_delimiter;
    int cmp;

    PyObject *next;

} recipes_split_object;


static PyObject *
recipes_split_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    recipes_split_object *lz;
    static char *kwargs[] = {"iterable", "key", "maxsplit", "keep", "eq", NULL};

    // mandatory arguments
    PyObject *iterable, *delimiter;

    // optional arguments
    Py_ssize_t maxsplit = -1;  // -1 means no maxsplit!
    int keep_delimiter = 0;
    int cmp = 0;

    PyObject *it;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OO|nii:split", kwargs,
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

    /* create recipes_split_object structure */
    lz = (recipes_split_object *)type->tp_alloc(type, 0);
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


static void
recipes_split_dealloc(recipes_split_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->it);
    Py_XDECREF(lz->delimiter);
    Py_XDECREF(lz->next);
    Py_TYPE(lz)->tp_free(lz);
}


static int
recipes_split_traverse(recipes_split_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->it);
    Py_VISIT(lz->delimiter);
    Py_VISIT(lz->next);
    return 0;
}


static PyObject *
recipes_split_next(recipes_split_object *lz)
{
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

    // Prevent to return a pending StopIteration exception from tp_iternext.
    helper_ExceptionClearStopIter();

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


static PyObject *
recipes_split_reduce(recipes_split_object *lz)
{
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

static PyObject *
recipes_split_setstate(recipes_split_object *lz, PyObject *state)
{
    PyObject *next;

    if (!PyArg_ParseTuple(state, "O", &next)) {
        return NULL;
    }

    Py_CLEAR(lz->next);
    Py_INCREF(next);
    lz->next = next;

    Py_RETURN_NONE;
}


static PyMethodDef recipes_split_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_split_reduce,
     METH_NOARGS,
     ""},

    {"__setstate__",
     (PyCFunction)recipes_split_setstate,
     METH_O,
     ""},

    {NULL,           NULL}           /* sentinel */
};


PyDoc_STRVAR(recipes_split_doc,
"split(iterable, key[, maxsplit, keep, eq])\n\
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

PyTypeObject recipes_split_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.split",      /* tp_name */
    sizeof(recipes_split_object),     /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_split_dealloc, /* tp_dealloc */
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
    recipes_split_doc,                /* tp_doc */
    (traverseproc)recipes_split_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_split_next, /* tp_iternext */
    recipes_split_methods,            /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    recipes_split_new,                /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
