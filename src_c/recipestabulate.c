/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *cnt;
} PyIUObject_Tabulate;

static PyTypeObject PyIUType_Tabulate;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * tabulate_new(PyTypeObject *type, PyObject *args,
                               PyObject *kwargs) {
    static char *kwlist[] = {"function", "start", NULL};
    PyIUObject_Tabulate *lz;

    PyObject *func, *cnt=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:tabulate", kwlist,
                                     &func, &cnt)) {
        return NULL;
    }
    if (cnt == NULL) {
#if PY_MAJOR_VERSION == 2
        cnt = PyInt_FromLong(0);
#else
        cnt = PyLong_FromLong(0);
#endif
    } else {
        Py_INCREF(cnt);
    }


    /* Create and fill struct */
    lz = (PyIUObject_Tabulate *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    lz->func = func;
    lz->cnt = cnt;

    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void tabulate_dealloc(PyIUObject_Tabulate *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_XDECREF(lz->cnt);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int tabulate_traverse(PyIUObject_Tabulate *lz, visitproc visit,
                             void *arg) {
    Py_VISIT(lz->func);
    Py_VISIT(lz->cnt);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * tabulate_next(PyIUObject_Tabulate *lz) {
    PyObject *result, *tmp=NULL;

    if (lz->cnt == NULL) {
        PyErr_Format(PyExc_TypeError, "Something went wrong and `cnt` was lost.");
        return NULL;
    }

    // Call the function with the current value as argument
    result = PyObject_CallFunctionObjArgs(lz->func, lz->cnt, NULL);
    if (result == NULL) {
        return NULL;
    }

    tmp = lz->cnt;
    lz->cnt = PyNumber_Add(tmp, PyIU_Long_1());
    Py_DECREF(tmp);
    if (lz->cnt == NULL) {
        return NULL;
    }

    return result;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * tabulate_reduce(PyIUObject_Tabulate *lz) {
    return Py_BuildValue("O(OO)",
                         Py_TYPE(lz),
                         lz->func,
                         lz->cnt);
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef tabulate_methods[] = {
    {"__reduce__", (PyCFunction)tabulate_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(tabulate_doc, "tabulate(function, start=0)");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Tabulate = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.tabulate2",    /* tp_name */
    sizeof(PyIUObject_Tabulate),        /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)tabulate_dealloc,       /* tp_dealloc */
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
    tabulate_doc,                       /* tp_doc */
    (traverseproc)tabulate_traverse,    /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)tabulate_next,        /* tp_iternext */
    tabulate_methods,                   /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    tabulate_new,                       /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
