/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "iterexcept.h"
#include "helper.h"
#include <structmember.h>
#include "docs_reduce.h"

PyDoc_STRVAR(
    iterexcept_prop_func_doc,
    "(any type) The function that is called by `iter_except` (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    iterexcept_prop_exception_doc,
    "(any type) The exception that ends `iter_except` (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    iterexcept_prop_first_doc,
    "(any type) The function that is called once (as setup) by `iter_except` "
    "(readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    iterexcept_doc,
    "iter_except(func, exception, first=None)\n"
    "--\n\n"
    "Call a function repeatedly until an `exception` is raised.\n"
    "\n"
    "Converts a call-until-exception interface to an iterator interface.\n"
    "Like ``iter(func, sentinel)`` but uses an `exception` instead of a sentinel\n"
    "to end the loop.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "func : callable\n"
    "    The function that is called until `exception` is raised.\n"
    "\n"
    "exception : Exception\n"
    "    The `exception` which terminates the iteration.\n"
    "\n"
    "first : callable or None, optional\n"
    "    If not given (or not ``None``) this function is called once before the \n"
    "    `func` is executed.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "result : generator\n"
    "    The result of the `func` calls as generator.\n"
    "\n"
    "Examples\n"
    "--------\n"
    ">>> from iteration_utilities import iter_except\n"
    ">>> from collections import OrderedDict\n"
    "\n"
    ">>> d = OrderedDict([('a', 1), ('b', 2)])\n"
    ">>> list(iter_except(d.popitem, KeyError))\n"
    "[('b', 2), ('a', 1)]\n"
    "\n"
    ".. note::\n"
    "    ``d.items()`` would yield the same result.\n"
    "\n"
    ">>> from math import sqrt\n"
    "\n"
    ">>> g = (sqrt(i) for i in [5, 4, 3, 2, 1, 0, -1, -2, -3])\n"
    ">>> def say_go():\n"
    "...     return 'go'\n"
    ">>> list(iter_except(g.__next__, ValueError, say_go))\n"
    "['go', 2.23606797749979, 2.0, 1.7320508075688772, 1.4142135623730951, 1.0, 0.0]\n"
    "\n"
    "Notes\n"
    "-----\n"
    "Further examples:\n"
    "\n"
    "- ``bsd_db_iter = iter_except(db.next, bsddb.error, db.first)``\n"
    "- ``heap_iter = iter_except(functools.partial(heappop, h), IndexError)``\n"
    "- ``dict_iter = iter_except(d.popitem, KeyError)``\n"
    "- ``deque_iter = iter_except(d.popleft, IndexError)``\n"
    "- ``queue_iter = iter_except(q.get_nowait, Queue.Empty)``\n"
    "- ``set_iter = iter_except(s.pop, KeyError)``\n");

static PyObject *
iterexcept_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"func", "exception", "first", NULL};
    PyIUObject_Iterexcept *self;
    PyObject *func;
    PyObject *except;
    PyObject *first = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O:iter_except", kwlist,
                                     &func, &except, &first)) {
        return NULL;
    }
    self = (PyIUObject_Iterexcept *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    Py_INCREF(except);
    self->except = except;
    self->first = first == Py_None ? NULL : first;
    Py_XINCREF(self->first);
    return (PyObject *)self;
}

static void
iterexcept_dealloc(PyIUObject_Iterexcept *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->except);
    Py_XDECREF(self->first);
    Py_TYPE(self)->tp_free(self);
}

static int
iterexcept_traverse(PyIUObject_Iterexcept *self, visitproc visit, void *arg) {
    Py_VISIT(self->func);
    Py_VISIT(self->except);
    Py_VISIT(self->first);
    return 0;
}

static int
iterexcept_clear(PyIUObject_Iterexcept *self) {
    Py_CLEAR(self->func);
    Py_CLEAR(self->except);
    Py_CLEAR(self->first);
    return 0;
}

static PyObject *
iterexcept_next(PyIUObject_Iterexcept *self) {
    PyObject *result;

    /* Call the first if it's set (nulling it thereafter) or the func if not. */
    if (self->first == NULL) {
        result = PyIU_CallWithNoArgument(self->func);
    } else {
        result = PyIU_CallWithNoArgument(self->first);
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

static PyObject *
iterexcept_reduce(PyIUObject_Iterexcept *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OOO)", Py_TYPE(self),
                         self->func,
                         self->except,
                         self->first ? self->first : Py_None);
}

static PyMethodDef iterexcept_methods[] = {
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)iterexcept_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef iterexcept_memberlist[] = {
    {
        "func",                                /* name */
        T_OBJECT,                              /* type */
        offsetof(PyIUObject_Iterexcept, func), /* offset */
        READONLY,                              /* flags */
        iterexcept_prop_func_doc               /* doc */
    },
    {
        "exception",                             /* name */
        T_OBJECT,                                /* type */
        offsetof(PyIUObject_Iterexcept, except), /* offset */
        READONLY,                                /* flags */
        iterexcept_prop_exception_doc            /* doc */
    },
    {
        "first",                                /* name */
        T_OBJECT_EX,                            /* type */
        offsetof(PyIUObject_Iterexcept, first), /* offset */
        READONLY,                               /* flags */
        iterexcept_prop_first_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Iterexcept = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.iter_except", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Iterexcept),                                      /* tp_basicsize */
    (Py_ssize_t)0,                                                                  /* tp_itemsize */
    /* methods */
    (destructor)iterexcept_dealloc,        /* tp_dealloc */
    (printfunc)0,                          /* tp_print */
    (getattrfunc)0,                        /* tp_getattr */
    (setattrfunc)0,                        /* tp_setattr */
    0,                                     /* tp_reserved */
    (reprfunc)0,                           /* tp_repr */
    (PyNumberMethods *)0,                  /* tp_as_number */
    (PySequenceMethods *)0,                /* tp_as_sequence */
    (PyMappingMethods *)0,                 /* tp_as_mapping */
    (hashfunc)0,                           /* tp_hash */
    (ternaryfunc)0,                        /* tp_call */
    (reprfunc)0,                           /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc)0,                       /* tp_setattro */
    (PyBufferProcs *)0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,           /* tp_flags */
    (const char *)iterexcept_doc,      /* tp_doc */
    (traverseproc)iterexcept_traverse, /* tp_traverse */
    (inquiry)iterexcept_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)iterexcept_next,     /* tp_iternext */
    iterexcept_methods,                /* tp_methods */
    iterexcept_memberlist,             /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)PyType_GenericAlloc,    /* tp_alloc */
    (newfunc)iterexcept_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
