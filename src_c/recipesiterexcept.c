typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *except;
    PyObject *first;
} PyIUObject_Iterexcept;

static PyTypeObject PyIUType_Iterexcept;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * iterexcept_new(PyTypeObject *type, PyObject *args,
                                 PyObject *kwargs) {
    static char *kwlist[] = {"func", "exception", "first", NULL};
    PyIUObject_Iterexcept *lz;

    PyObject *func, *except, *first=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O:iter_except", kwlist,
                                     &func, &except, &first)) {
        return NULL;
    }
    if (first == Py_None) {
        first = NULL;
    }

    /* Create and fill struct */
    lz = (PyIUObject_Iterexcept *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    Py_INCREF(except);
    Py_XINCREF(first);
    lz->func = func;
    lz->except = except;
    lz->first = first;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void iterexcept_dealloc(PyIUObject_Iterexcept *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->func);
    Py_XDECREF(lz->except);
    Py_XDECREF(lz->first);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int iterexcept_traverse(PyIUObject_Iterexcept *lz, visitproc visit,
                               void *arg) {
    Py_VISIT(lz->func);
    Py_VISIT(lz->except);
    Py_VISIT(lz->first);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * iterexcept_next(PyIUObject_Iterexcept *lz) {
    PyObject *result;

    // Call the first if it's set (nulling it thereafter) or the func if not.
    if (lz->first == NULL) {
        result = PyObject_CallObject(lz->func, NULL);
    } else {
        result = PyObject_CallObject(lz->first, NULL);
        Py_DECREF(lz->first);
        lz->first = NULL;
    }

    // Stop if the result is NULL but only clear the exception if the expected
    // exception happened otherwise just return the result (thereby bubbling
    // up other exceptions).
    if (result == NULL && PyErr_Occurred() && PyErr_ExceptionMatches(lz->except)) {
        PyErr_Clear();
        return NULL;
    } else {
        return result;
    }
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * iterexcept_reduce(PyIUObject_Iterexcept *lz) {
    if (lz->first == NULL) {
        return Py_BuildValue("O(OO)", Py_TYPE(lz),
                             lz->func,
                             lz->except);
    } else {
        return Py_BuildValue("O(OOO)", Py_TYPE(lz),
                             lz->func,
                             lz->except,
                             lz->first);
    }
}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef iterexcept_methods[] = {
    {"__reduce__", (PyCFunction)iterexcept_reduce, METH_NOARGS, ""},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(iterexcept_doc, "iter_except(func, exception[, first])\n\
\n\
Call a function repeatedly until an `exception` is raised.\n\
\n\
Converts a call-until-exception interface to an iterator interface.\n\
Like ``iter(func, sentinel)`` but uses an `exception` instead of a sentinel\n\
to end the loop.\n\
\n\
Parameters\n\
----------\n\
func : callable\n\
    The function that is called until `exception` is raised.\n\
\n\
exception : Exception\n\
    The `exception` which terminates the iteration.\n\
\n\
first : callable or None, optional\n\
    If not ``None`` this function is called once before the `func` is\n\
    executed.\n\
\n\
Returns\n\
-------\n\
result : generator\n\
    The result of the `func` calls as generator.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import iter_except\n\
>>> from collections import OrderedDict\n\
\n\
>>> d = OrderedDict([('a', 1), ('b', 2)])\n\
>>> list(iter_except(d.popitem, KeyError))\n\
[('b', 2), ('a', 1)]\n\
\n\
.. note::\n\
    ``d.items()`` would yield the same result. At least with Python3.\n\
\n\
>>> from math import sqrt\n\
>>> import sys\n\
\n\
>>> g = (sqrt(i) for i in [5, 4, 3, 2, 1, 0, -1, -2, -3])\n\
>>> func = g.next if sys.version_info.major == 2 else g.__next__\n\
>>> def say_go():\n\
...     return 'go'\n\
>>> list(iter_except(func, ValueError, say_go))\n\
['go', 2.23606797749979, 2.0, 1.7320508075688772, 1.4142135623730951, 1.0, 0.0]\n\
\n\
Notes\n\
-----\n\
Further examples:\n\
\n\
- ``bsd_db_iter = iter_except(db.next, bsddb.error, db.first)``\n\
- ``heap_iter = iter_except(functools.partial(heappop, h), IndexError)``\n\
- ``dict_iter = iter_except(d.popitem, KeyError)``\n\
- ``deque_iter = iter_except(d.popleft, IndexError)``\n\
- ``queue_iter = iter_except(q.get_nowait, Queue.Empty)``\n\
- ``set_iter = iter_except(s.pop, KeyError)``\n\
");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Iterexcept = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.iter_except",  /* tp_name */
    sizeof(PyIUObject_Iterexcept),      /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)iterexcept_dealloc,     /* tp_dealloc */
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
    iterexcept_doc,                     /* tp_doc */
    (traverseproc)iterexcept_traverse,  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)iterexcept_next,      /* tp_iternext */
    iterexcept_methods,                 /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    PyType_GenericAlloc,                /* tp_alloc */
    iterexcept_new,                     /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
