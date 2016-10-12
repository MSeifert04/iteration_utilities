typedef struct {
    PyObject_HEAD
    PyObject *funcs;
    int reverse;
    int all;
} functions_compose_object;

static PyTypeObject functions_compose_type;


static PyObject *
functions_compose_new(PyTypeObject *type, PyObject *funcs, PyObject *kwds)
{
    functions_compose_object *lz;
    PyObject *reversekw;
    int reverse = 0;
    PyObject *allkw;
    int all = 0;
    Py_ssize_t nkwds = 0;

    if (funcs == NULL || !PyTuple_Check(funcs) || PyTuple_Size(funcs) <= 0) {
        PyErr_Format(PyExc_TypeError, "at least 1 function must be given.");
        return NULL;
    }

    if (kwds != NULL && PyDict_Check(kwds) && PyDict_Size(kwds)) {
        reversekw = PyDict_GetItemString(kwds, "reverse");
        if (reversekw != NULL) {
            reverse = PyLong_AsLong(reversekw);
            nkwds++;
        }
        allkw = PyDict_GetItemString(kwds, "all");
        if (allkw != NULL) {
            all = PyLong_AsLong(allkw);
            nkwds++;
        }
        if (PyDict_Size(kwds) - nkwds != 0) {
            PyErr_Format(PyExc_TypeError,
                         "`compose` got an unexpected keyword argument");
            return NULL;
        }
    }

    lz = (functions_compose_object *)type->tp_alloc(type, 0);
    if (lz == NULL)
        return NULL;

    Py_INCREF(funcs);
    lz->funcs = funcs;
    lz->reverse = reverse;
    lz->all = all;

    return (PyObject *)lz;
}


static void
functions_compose_dealloc(functions_compose_object *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->funcs);
    Py_TYPE(lz)->tp_free(lz);
}


static int
functions_compose_traverse(functions_compose_object *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->funcs);
    return 0;
}


static PyObject *
functions_compose_call(functions_compose_object *lz, PyObject *args, PyObject *kw)
{
    PyObject *funcs = lz->funcs;
    int reverse = lz->reverse;
    PyObject *func = NULL;
    PyObject *temp = NULL;
    PyObject *oldtemp = NULL;
    PyObject *result = NULL;
    Py_ssize_t i;
    Py_ssize_t tuplesize = PyTuple_Size(funcs);

    if (lz->all) {
        result = PyTuple_New(tuplesize);
    }

    for (i=0 ; i<tuplesize ; i++) {

        if (reverse) {
            func = PyTuple_GET_ITEM(funcs, tuplesize-i-1);
        } else {
            func = PyTuple_GET_ITEM(funcs, i);
        }

        if (temp == NULL || lz->all) {
            temp = PyObject_Call(func, args, kw);
            if (lz->all) {
                PyTuple_SET_ITEM(result, i, temp);
            }
        } else {
            oldtemp = temp;
            temp = PyObject_CallFunctionObjArgs(func, temp, NULL);
            Py_DECREF(oldtemp);
        }

        if (temp == NULL) {
            return NULL;
        }
    }

    if (lz->all) {
        return result;
    }
    return temp;
}


static PyObject *
functions_compose_reduce(functions_compose_object *lz, PyObject *unused)
{
    return Py_BuildValue("OO(ii)", Py_TYPE(lz),
                         lz->funcs,
                         lz->reverse, lz->all);
}

static PyObject *
functions_compose_setstate(functions_compose_object *lz, PyObject *state)
{
    int reverse, all;

    if (!PyArg_ParseTuple(state, "ii", &reverse, &all)) {
        return NULL;
    }

    lz->reverse = reverse;
    lz->all = all;

    Py_RETURN_NONE;
}

static PyMethodDef functions_compose_methods[] = {
    {"__reduce__", (PyCFunction)functions_compose_reduce, METH_NOARGS, ""},
    {"__setstate__", (PyCFunction)functions_compose_setstate, METH_O, ""},

    {NULL,              NULL}
};



PyDoc_STRVAR(functions_compose_doc,
"compose(*funcs[, reverse, all])\n\
\n\
Chains several function calls.\n\
\n\
Parameters\n\
----------\n\
funcs\n\
    Any number of callables.\n\
\n\
reverse : bool, optional\n\
    If ``True`` apply the the `funcs` in reversed order.\n\
    Default is ``False``.\n\
\n\
all : bool, optional\n\
    If ``True`` apply each of the `funcs` seperatly and return a tuple\n\
    containing the individual results when calling the instance.\n\
\n\
Returns\n\
-------\n\
composed_func : callable\n\
    The chained `funcs`.\n\
\n\
Methods\n\
-------\n\
__call__(\\*args, \\*\\*kwargs)\n\
   Returns ``func1(...(funcn(*args, **kwargs)))``.\n\
   or ``funcn(...(func1(*args, **kwargs)))`` (if `reverse` is True).\n\
   or ``func1(*args, **kwargs), ..., funcn(*args, **kwargs)`` if `all` is True.\n\
\n\
Examples\n\
--------\n\
`compose` simple calls all `funcs` on the result of the previous one::\n\
\n\
    >>> from iteration_utilities import compose\n\
    >>> double = lambda x: x*2\n\
    >>> increment = lambda x: x+1\n\
    >>> double_then_increment = compose(double, increment)\n\
    >>> double_then_increment(10)\n\
    21\n\
\n\
Or apply them in reversed order::\n\
\n\
    >>> increment_then_double = compose(double, increment, reverse=True)\n\
    >>> increment_then_double(10)\n\
    22\n\
\n\
Or apply all of them on the input::\n\
\n\
    >>> double_and_increment = compose(double, increment, all=True)\n\
    >>> double_and_increment(10)\n\
    (20, 11)\n\
");


static PyTypeObject functions_compose_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.compose",      /* tp_name */
    sizeof(functions_compose_object),   /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)functions_compose_dealloc, /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    (ternaryfunc)functions_compose_call, /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    functions_compose_doc,              /* tp_doc */
    (traverseproc)functions_compose_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    0,                                  /* tp_iter */
    0,                                  /* tp_iternext */
    functions_compose_methods,          /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    functions_compose_new,              /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
