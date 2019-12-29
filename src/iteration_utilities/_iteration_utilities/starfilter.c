/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "starfilter.h"
#include <structmember.h>
#include "docs_reduce.h"

PyDoc_STRVAR(
    starfilter_prop_pred_doc,
    "(callable) The function by which to filter (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    starfilter_doc,
    "starfilter(pred, iterable)\n"
    "--\n\n"
    "Like :py:func:`filter` but unpacks the current item in `iterable` when \n"
    "calling `pred`. This is similar to the difference between :py:func:`map` and \n"
    ":py:func:`itertools.starmap`.\n"
    "\n"
    ".. versionadded:: 0.3\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "pred : callable\n"
    "    The predicate function that is called to determine if the items should\n"
    "    be kept.\n"
    "\n"
    "    .. note::\n"
    "       Unlike :py:func:`filter` the `pred` cannot be ``None``.\n"
    "\n"
    "iterable : iterable\n"
    "    `Iterable` containing the elements.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "iterator : generator\n"
    "    A normal iterator over `iterable` containing only the items where \n"
    "    ``pred(*item)`` is ``True``.\n"
    "\n"
    "Notes\n"
    "-----\n"
    "This is identical to ``filter(lambda x: pred(*x), iterable)`` but faster.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "A simple example::\n"
    "\n"
    "    >>> from iteration_utilities import starfilter\n"
    "    >>> from operator import eq\n"
    "    >>> list(starfilter(eq, zip([1,2,3], [2,2,2])))\n"
    "    [(2, 2)]\n"
    "\n"
    "See also\n"
    "--------\n"
    "filter\n"
    "iteration_utilities.packed\n");

static PyObject *
starfilter_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"pred", "iterable", NULL};
    PyIUObject_Starfilter *self;
    PyObject *iterable;
    PyObject *func;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:starfilter", kwlist,
                                     &func, &iterable)) {
        return NULL;
    }
    self = (PyIUObject_Starfilter *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    return (PyObject *)self;
}

static void
starfilter_dealloc(PyIUObject_Starfilter *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

static int
starfilter_traverse(PyIUObject_Starfilter *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->func);
    return 0;
}

static int
starfilter_clear(PyIUObject_Starfilter *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->func);
    return 0;
}

static PyObject *
starfilter_next(PyIUObject_Starfilter *self) {
    PyObject *item;

    while ((item = Py_TYPE(self->iterator)->tp_iternext(self->iterator))) {
        PyObject *newargs;
        PyObject *val;
        int ok;

        if (!PyTuple_CheckExact(item)) {
            newargs = PySequence_Tuple(item);
            if (newargs == NULL) {
                Py_DECREF(item);
                return NULL;
            }
        } else {
            Py_INCREF(item);
            newargs = item;
        }
        val = PyObject_Call(self->func, newargs, NULL);
        Py_DECREF(newargs);
        if (val == NULL) {
            Py_DECREF(item);
            return NULL;
        }
        ok = PyObject_IsTrue(val);
        Py_DECREF(val);

        if (ok > 0) {
            return item;
        }
        Py_DECREF(item);
        if (ok < 0) {
            return NULL;
        }
    }
    return NULL;
}

static PyObject *
starfilter_reduce(PyIUObject_Starfilter *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OO)", Py_TYPE(self), self->func, self->iterator);
}

static PyMethodDef starfilter_methods[] = {
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)starfilter_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef starfilter_memberlist[] = {
    {
        "pred",                                /* name */
        T_OBJECT,                              /* type */
        offsetof(PyIUObject_Starfilter, func), /* offset */
        READONLY,                              /* flags */
        starfilter_prop_pred_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Starfilter = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.starfilter", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Starfilter),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                 /* tp_itemsize */
    /* methods */
    (destructor)starfilter_dealloc,        /* tp_dealloc */
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
    (const char *)starfilter_doc,      /* tp_doc */
    (traverseproc)starfilter_traverse, /* tp_traverse */
    (inquiry)starfilter_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)starfilter_next,     /* tp_iternext */
    starfilter_methods,                /* tp_methods */
    starfilter_memberlist,             /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)PyType_GenericAlloc,    /* tp_alloc */
    (newfunc)starfilter_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
