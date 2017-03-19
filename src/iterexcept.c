/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *except;
    PyObject *first;
} PyIUObject_Iterexcept;

PyTypeObject PyIUType_Iterexcept;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
iterexcept_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"func", "exception", "first", NULL};
    PyIUObject_Iterexcept *self;

    PyObject *func, *except, *first=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O:iter_except", kwlist,
                                     &func, &except, &first)) {
        return NULL;
    }
    PYIU_NULL_IF_NONE(first);

    /* Create and fill struct */
    self = (PyIUObject_Iterexcept *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    Py_INCREF(except);
    Py_XINCREF(first);
    self->func = func;
    self->except = except;
    self->first = first;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
iterexcept_dealloc(PyIUObject_Iterexcept *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->except);
    Py_XDECREF(self->first);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
iterexcept_traverse(PyIUObject_Iterexcept *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->func);
    Py_VISIT(self->except);
    Py_VISIT(self->first);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
iterexcept_next(PyIUObject_Iterexcept *self)
{
    PyObject *result;

    /* Call the first if it's set (nulling it thereafter) or the func if not. */
    if (self->first == NULL) {
        result = PyObject_CallObject(self->func, NULL);
    } else {
        result = PyObject_CallObject(self->first, NULL);
        Py_CLEAR(self->first);
    }

    /* Stop if the result is NULL but only clear the exception if the expected
       exception happened otherwise just return the result (thereby bubbling
       up other exceptions).
       */
    if (result == NULL && PyErr_Occurred() && PyErr_ExceptionMatches(self->except)) {
        PyErr_Clear();
        return NULL;
    } else {
        return result;
    }
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
iterexcept_reduce(PyIUObject_Iterexcept *self)
{
    return Py_BuildValue("O(OOO)", Py_TYPE(self),
                         self->func,
                         self->except,
                         self->first ? self->first : Py_None);
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef iterexcept_methods[] = {
    {"__reduce__", (PyCFunction)iterexcept_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(iterexcept_doc, "iter_except(func, exception, first=None)\n\
--\n\
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
    If not given (or not ``None``) this function is called once before the \n\
    `func` is executed.\n\
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
- ``set_iter = iter_except(s.pop, KeyError)``");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Iterexcept = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.iter_except",                  /* tp_name */
    sizeof(PyIUObject_Iterexcept),                      /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)iterexcept_dealloc,                     /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    0,                                                  /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    0,                                                  /* tp_call */
    0,                                                  /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    iterexcept_doc,                                     /* tp_doc */
    (traverseproc)iterexcept_traverse,                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)iterexcept_next,                      /* tp_iternext */
    iterexcept_methods,                                 /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    iterexcept_new,                                     /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
