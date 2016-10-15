typedef struct {
    PyObject_HEAD
    PyObject *it;
    Py_ssize_t times;
    PyObject *fillvalue;
    int truncate;

    PyObject *result;
} PyIUObject_Grouper;

static PyTypeObject PyIUType_Grouper;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * grouper_new(PyTypeObject *type, PyObject *args,
                              PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "n", "fillvalue", "truncate", NULL};
    PyIUObject_Grouper *lz;

    PyObject *iterable;
    Py_ssize_t times;
    PyObject *fillvalue = NULL;
    int truncate = 0;
    PyObject *it;
    PyObject *result = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "On|Oi:grouper", kwlist,
                                     &iterable, &times,
                                     &fillvalue, &truncate)) {
        return NULL;
    }

    if (fillvalue != NULL && truncate != 0) {
        PyErr_Format(PyExc_TypeError,
                     "wither `truncate` or `fillvalue` can be set.");
        return NULL;
    }

    if (times <= 0) {
        PyErr_Format(PyExc_ValueError,
                     "`n` must be greater than 0.");
        return NULL;
    }

    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        return NULL;
    }

    lz = (PyIUObject_Grouper *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(it);
        return NULL;
    }

    lz->it = it;
    lz->times = times;
    Py_XINCREF(fillvalue);
    lz->fillvalue = fillvalue;
    lz->truncate = truncate;
    lz->result = result;

    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void grouper_dealloc(PyIUObject_Grouper *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->it);
    Py_XDECREF(lz->fillvalue);
    Py_XDECREF(lz->result);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int grouper_traverse(PyIUObject_Grouper *lz, visitproc visit,
                            void *arg) {
    Py_VISIT(lz->it);
    Py_VISIT(lz->fillvalue);
    Py_VISIT(lz->result);
    return 0;
}


static PyObject * grouper_next(PyIUObject_Grouper *lz) {
    PyObject *it = lz->it;
    Py_ssize_t times = lz->times;
    PyObject *fillvalue = lz->fillvalue;
    int truncate = lz->truncate;
    PyObject *result = lz->result;

    PyObject *newresult, *lastresult;
    PyObject *item, *olditem;

    Py_ssize_t i, j;

    // First call needs to create a tuple for the result.
    if (result == NULL) {
        result = PyTuple_New(times);
        lz->result = result;
    }

    // Recycle old tuple
    if (Py_REFCNT(result) == 1) {

        for (i=0 ; i < times ; i++) {
            item = PyIter_Next(it);
            if (item == NULL) {
                if (i == 0 || truncate != 0) {
                    return NULL;
                } else if (fillvalue != NULL) {
                    Py_INCREF(fillvalue);
                    item = fillvalue;
                } else {
                    // Create a new tuple
                    lastresult = PyTuple_New(i);
                    if (lastresult == NULL) {
                        return NULL;
                    }
                    // Fill already found values. (Incref them is save because)
                    // they will be decref'd when dealloced.
                    for (j=0 ; j<i ; j++) {
                        olditem = PyTuple_GET_ITEM(result, j);
                        Py_INCREF(olditem);
                        PyTuple_SET_ITEM(lastresult, j, olditem);
                    }
                    // Return the new tuple.
                    return lastresult;
                }
            }
            olditem = PyTuple_GET_ITEM(result, i);
            PyTuple_SET_ITEM(result, i, item);
            Py_XDECREF(olditem);
        }
        Py_INCREF(result);
        return result;

    } else {
        newresult = PyTuple_New(times);
        if (newresult == NULL) {
            return NULL;
        }

        for (i=0 ; i < times ; i++) {
            item = PyIter_Next(it);
            if (item == NULL) {
                if (i == 0 || truncate != 0) {
                    Py_DECREF(newresult);
                    return NULL;
                } else if (fillvalue != NULL) {
                    Py_INCREF(fillvalue);
                    item = fillvalue;
                } else {
                    // Create a new tuple
                    lastresult = PyTuple_New(i);
                    if (lastresult == NULL) {
                        Py_DECREF(newresult);
                        return NULL;
                    }
                    // Fill already found values. (Incref them is save because)
                    // they will be decref'd when dealloced.
                    for (j=0 ; j<i ; j++) {
                        olditem = PyTuple_GET_ITEM(newresult, j);
                        Py_INCREF(olditem);
                        PyTuple_SET_ITEM(lastresult, j, olditem);
                    }
                    Py_DECREF(newresult);
                    // Return the new tuple.
                    return lastresult;
                }
            }
            PyTuple_SET_ITEM(newresult, i, item);
        }
        return newresult;
    }
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * grouper_reduce(PyIUObject_Grouper *lz) {
    if (lz->fillvalue == NULL) {
        return Py_BuildValue("O(On)(Oi)", Py_TYPE(lz),
                             lz->it,
                             lz->times,
                             lz->result ? lz->result : Py_None,
                             lz->truncate);
    } else {
        return Py_BuildValue("O(OnO)(Oi)", Py_TYPE(lz),
                             lz->it,
                             lz->times,
                             lz->fillvalue,
                             lz->result ? lz->result : Py_None,
                             lz->truncate);
    }
}

/******************************************************************************
 *
 * Setstate
 *
 *****************************************************************************/

static PyObject * grouper_setstate(PyIUObject_Grouper *lz, PyObject *state) {
    PyObject *result;
    int truncate;

    if (!PyArg_ParseTuple(state, "Oi", &result, &truncate)) {
        return NULL;
    }

    Py_CLEAR(lz->result);

    if (result == Py_None) {
        lz->result = NULL;
    } else {
        Py_INCREF(result);
        lz->result = result;
    }

    lz->truncate = truncate;

    Py_RETURN_NONE;
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef grouper_methods[] = {
    {"__reduce__", (PyCFunction)grouper_reduce, METH_NOARGS, ""},
    {"__setstate__", (PyCFunction)grouper_setstate, METH_O, ""},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(grouper_doc, "grouper(iterable, times[, fillvalue, truncate])\n\
\n\
Collect data into fixed-length chunks or blocks.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Any `iterable` to group.\n\
\n\
n : :py:class:`int`\n\
    The number of elements in each chunk.\n\
\n\
fillvalue : any type, optional\n\
    The `fillvalue` if the `iterable` is consumed and the last yielded group\n\
    should be filled. If not given the last yielded group may be shorter\n\
    than the group before.\n\
\n\
truncate : bool, optional\n\
    As alternative to `fillvalue` the last group is discarded if it is\n\
    shorter than `n` and `truncate` is ``True``.\n\
    Default is ``False``.\n\
\n\
Raises\n\
------\n\
TypeError\n\
    If `truncate` is ``True`` and a `fillvalue` is given.\n\
\n\
Returns\n\
-------\n\
groups : generator\n\
    An `iterable` containing the groups/chunks as ``tuple``.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import grouper\n\
\n\
>>> list(grouper('ABCDEFG', 3))\n\
[('A', 'B', 'C'), ('D', 'E', 'F'), ('G',)]\n\
\n\
>>> list(grouper('ABCDEFG', 3, fillvalue='x'))\n\
[('A', 'B', 'C'), ('D', 'E', 'F'), ('G', 'x', 'x')]\n\
\n\
>>> list(grouper('ABCDEFG', 3, truncate=True))\n\
[('A', 'B', 'C'), ('D', 'E', 'F')]\n\
");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

PyTypeObject PyIUType_Grouper = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.grouper",      /* tp_name */
    sizeof(PyIUObject_Grouper),     /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)grouper_dealloc, /* tp_dealloc */
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
    grouper_doc,                /* tp_doc */
    (traverseproc)grouper_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)grouper_next, /* tp_iternext */
    grouper_methods,            /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    grouper_new,                /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
