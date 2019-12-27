/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "empty.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    empty_doc,
    "_EmptyType(/)\n"
    "--\n\n"
    "An empty iterator.\n"
    "\n"
    "Notes\n"
    "-------\n"
    "There is only one instance of this class. And this class cannot be subclassed.\n");

static PyObject *
empty_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    if (PyTuple_GET_SIZE(args) || (kwargs != NULL && PyDict_Size(kwargs))) {
        PyErr_Format(PyExc_TypeError, "`%.200s.__new__` takes no arguments.", PyIUType_Empty.tp_name);
        return NULL;
    }
    Py_INCREF(&EmptyStruct);
    return &EmptyStruct;
}

static PyObject *
empty_next(PyObject *self) {
    return NULL;
}

static PyObject *
empty_lengthhint(PyObject *self, PyObject *Py_UNUSED(args)) {
    Py_INCREF(PyIU_global_zero);
    return PyIU_global_zero;
}

static PyObject *
empty_reduce(PyObject *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O()", Py_TYPE(self));
}

static PyMethodDef empty_methods[] = {
    {
        "__length_hint__",             /* ml_name */
        (PyCFunction)empty_lengthhint, /* ml_meth */
        METH_NOARGS,                   /* ml_flags */
        PYIU_lenhint_doc               /* ml_doc */
    },
    {
        "__reduce__",              /* ml_name */
        (PyCFunction)empty_reduce, /* ml_meth */
        METH_NOARGS,               /* ml_flags */
        PYIU_reduce_doc            /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

PyTypeObject PyIUType_Empty = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities._iteration_utilities._EmptyType", /* tp_name */
    (Py_ssize_t)0,                                                                                      /* tp_basicsize */
    (Py_ssize_t)0,                                                                                      /* tp_itemsize */
    /* methods */
    (destructor)0,                         /* tp_dealloc */
    (printfunc)0,                          /* tp_print */
    (getattrfunc)0,                        /* tp_getattr */
    (setattrfunc)0,                        /* tp_setattr */
    0,                                     /* tp_reserved */
    (reprfunc)0,                           /* tp_repr */
    (PyNumberMethods *)0,                  /* tp_as_number */
    (PySequenceMethods *)0,                /* tp_as_sequence */
    (PyMappingMethods *)0,                 /* tp_as_mapping */
    (hashfunc)0,                           /* tp_hash  */
    (ternaryfunc)0,                        /* tp_call */
    (reprfunc)0,                           /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc)0,                       /* tp_setattro */
    (PyBufferProcs *)0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                    /* tp_flags */
    (const char *)empty_doc,               /* tp_doc */
    (traverseproc)0,                       /* tp_traverse */
    (inquiry)0,                            /* tp_clear */
    (richcmpfunc)0,                        /* tp_richcompare */
    (Py_ssize_t)0,                         /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,        /* tp_iter */
    (iternextfunc)empty_next,              /* tp_iternext */
    empty_methods,                         /* tp_methods */
    0,                                     /* tp_members */
    0,                                     /* tp_getset */
    0,                                     /* tp_base */
    0,                                     /* tp_dict */
    (descrgetfunc)0,                       /* tp_descr_get */
    (descrsetfunc)0,                       /* tp_descr_set */
    (Py_ssize_t)0,                         /* tp_dictoffset */
    (initproc)0,                           /* tp_init */
    (allocfunc)0,                          /* tp_alloc */
    (newfunc)empty_new,                    /* tp_new */
    (freefunc)PyObject_Del,                /* tp_free */
};

PyObject EmptyStruct = PYIU_CREATE_SINGLETON_INSTANCE(PyIUType_Empty);
