/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "applyfunc.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    applyfunc_prop_func_doc,
    "(callable) The function used (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    applyfunc_prop_current_doc,
    "(any type) The current value for the function (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    applyfunc_doc,
    "applyfunc(func, initial)\n"
    "--\n\n"
    "Successively apply `func` on `value`.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "func : callable\n"
    "    The function to apply. The `value` is given as first argument to the \n"
    "    `func`, no other arguments will be passed during the function call.\n"
    "\n"
    "initial : any type\n"
    "    The `initial` `value` for the function.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "results : generator\n"
    "    The result of the successively applied `func`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "The first element is the initial `value` and the next elements are\n"
    "the result of ``func(value)``, then ``func(func(value))``, ...::\n"
    "\n"
    "    >>> from iteration_utilities import applyfunc, getitem\n"
    "    >>> import math\n"
    "    >>> list(getitem(applyfunc(math.sqrt, 10), stop=4))\n"
    "    [3.1622776601683795, 1.7782794100389228, 1.333521432163324, 1.1547819846894583]\n"
    "\n"
    ".. warning::\n"
    "    This will return an infinitely long generator so do **not** try to do\n"
    "    something like ``list(applyfunc())``!\n");

static PyObject *
applyfunc_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"func", "initial", NULL};
    PyIUObject_Applyfunc *self;
    PyObject *func;
    PyObject *initial;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:applyfunc", kwlist,
                                     &func, &initial)) {
        return NULL;
    }
    self = (PyIUObject_Applyfunc *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    Py_INCREF(initial);
    self->value = initial;
    return (PyObject *)self;
}

static void
applyfunc_dealloc(PyIUObject_Applyfunc *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->value);
    Py_TYPE(self)->tp_free(self);
}

static int
applyfunc_traverse(PyIUObject_Applyfunc *self, visitproc visit, void *arg) {
    Py_VISIT(self->func);
    Py_VISIT(self->value);
    return 0;
}

static int
applyfunc_clear(PyIUObject_Applyfunc *self) {
    Py_CLEAR(self->func);
    Py_CLEAR(self->value);
    return 0;
}

static PyObject *
applyfunc_next(PyIUObject_Applyfunc *self) {
    PyObject *newval;

    /* Call the function with the current value as argument.  */
    newval = PyIU_CallWithOneArgument(self->func, self->value);
    if (newval == NULL) {
        return NULL;
    }

    /* Save the new value and also return it. */
    Py_INCREF(newval);
    Py_SETREF(self->value, newval);
    return newval;
}

static PyObject *
applyfunc_reduce(PyIUObject_Applyfunc *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OO)",
                         Py_TYPE(self),
                         self->func,
                         self->value);
}

static PyMethodDef applyfunc_methods[] = {
    {
        "__reduce__",                  /* ml_name */
        (PyCFunction)applyfunc_reduce, /* ml_meth */
        METH_NOARGS,                   /* ml_flags */
        PYIU_reduce_doc                /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef applyfunc_memberlist[] = {
    {
        "func",                               /* name */
        T_OBJECT,                             /* type */
        offsetof(PyIUObject_Applyfunc, func), /* offset */
        READONLY,                             /* flags */
        applyfunc_prop_func_doc               /* doc */
    },
    {
        "current",                             /* name */
        T_OBJECT,                              /* type */
        offsetof(PyIUObject_Applyfunc, value), /* offset */
        READONLY,                              /* flags */
        applyfunc_prop_current_doc             /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Applyfunc = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.applyfunc", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Applyfunc),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                /* tp_itemsize */
    /* methods */
    (destructor)applyfunc_dealloc,         /* tp_dealloc */
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
        Py_TPFLAGS_BASETYPE,          /* tp_flags */
    (const char *)applyfunc_doc,      /* tp_doc */
    (traverseproc)applyfunc_traverse, /* tp_traverse */
    (inquiry)applyfunc_clear,         /* tp_clear */
    (richcmpfunc)0,                   /* tp_richcompare */
    (Py_ssize_t)0,                    /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,   /* tp_iter */
    (iternextfunc)applyfunc_next,     /* tp_iternext */
    applyfunc_methods,                /* tp_methods */
    applyfunc_memberlist,             /* tp_members */
    0,                                /* tp_getset */
    0,                                /* tp_base */
    0,                                /* tp_dict */
    (descrgetfunc)0,                  /* tp_descr_get */
    (descrsetfunc)0,                  /* tp_descr_set */
    (Py_ssize_t)0,                    /* tp_dictoffset */
    (initproc)0,                      /* tp_init */
    (allocfunc)0,                     /* tp_alloc */
    (newfunc)applyfunc_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,        /* tp_free */
};
