/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "tabulate.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    tabulate_prop_func_doc,
    "(callable) The function to tabulate (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    tabulate_prop_current_doc,
    "(any type) The current value to tabulate (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    tabulate_doc,
    "tabulate(func, start=0)\n"
    "--\n\n"
    "Return ``function(0)``, ``function(1)``, ...\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "func : callable\n"
    "    The `function` to apply.\n"
    "\n"
    "start : any type, optional\n"
    "    The starting value to apply the `function` on. Each time `tabulate` is\n"
    "    called this value will be incremented by one.\n"
    "    Default is ``0``.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "tabulated : generator\n"
    "    An infinite generator containing the results of the `function` applied\n"
    "    on the values beginning by `start`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "Since the return is an infinite generator you need some other function\n"
    "to extract only the needed values. For example\n"
    ":py:func:`~iteration_utilities.getitem`::\n"
    "\n"
    "    >>> from iteration_utilities import tabulate, getitem\n"
    "    >>> from math import sqrt\n"
    "    >>> t = tabulate(sqrt, 0)\n"
    "    >>> list(getitem(t, stop=3))\n"
    "    [0.0, 1.0, 1.4142135623730951]\n"
    "\n"
    ".. warning::\n"
    "    This will return an infinitely long generator so do **not** try to do\n"
    "    something like ``list(tabulate())``!\n"
    "\n"
    "This is equivalent to:\n"
    "\n"
    ".. code::\n"
    "\n"
    "    import itertools\n"
    "    \n"
    "    def tabulate(function, start=0)\n"
    "        return map(function, itertools.count(start))\n");

static PyObject *
tabulate_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"func", "start", NULL};
    PyIUObject_Tabulate *self;
    PyObject *func;
    PyObject *cnt = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:tabulate", kwlist,
                                     &func, &cnt)) {
        return NULL;
    }
    self = (PyIUObject_Tabulate *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    self->cnt = cnt == NULL ? PyIU_global_zero : cnt;
    Py_XINCREF(self->cnt);
    return (PyObject *)self;
}

static void
tabulate_dealloc(PyIUObject_Tabulate *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->cnt);
    Py_TYPE(self)->tp_free(self);
}

static int
tabulate_traverse(PyIUObject_Tabulate *self, visitproc visit, void *arg) {
    Py_VISIT(self->func);
    Py_VISIT(self->cnt);
    return 0;
}

static int
tabulate_clear(PyIUObject_Tabulate *self) {
    Py_CLEAR(self->func);
    Py_CLEAR(self->cnt);
    return 0;
}

static PyObject *
tabulate_next(PyIUObject_Tabulate *self) {
    PyObject *result = NULL;
    PyObject *new_count;

    if (self->cnt == NULL) {
        return NULL;
    }
    /* Call the function with the current value as argument. */
    result = PyIU_CallWithOneArgument(self->func, self->cnt);
    if (result == NULL) {
        Py_CLEAR(self->cnt);
        return NULL;
    }
    /* Increment the counter. */
    new_count = PyNumber_Add(self->cnt, PyIU_global_one);
    Py_SETREF(self->cnt, new_count);
    if (self->cnt == NULL) {
        Py_DECREF(result);
        return NULL;
    }
    return result;
}

static PyObject *
tabulate_reduce(PyIUObject_Tabulate *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OO)",
                         Py_TYPE(self),
                         self->func,
                         self->cnt);
}

static PyMethodDef tabulate_methods[] = {
    {
        "__reduce__",                 /* ml_name */
        (PyCFunction)tabulate_reduce, /* ml_meth */
        METH_NOARGS,                  /* ml_flags */
        PYIU_reduce_doc               /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef tabulate_memberlist[] = {
    {
        "func",                /* name */
        T_OBJECT,              /* type */
        offsetof(PyIUObject_Tabulate, func),             /* offset */
        READONLY,              /* flags */
        tabulate_prop_func_doc /* doc */
    },
    {
        "current",                /* name */
        T_OBJECT,                 /* type */
        offsetof(PyIUObject_Tabulate, cnt),                 /* offset */
        READONLY,                 /* flags */
        tabulate_prop_current_doc /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Tabulate = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.tabulate", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Tabulate),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                               /* tp_itemsize */
    /* methods */
    (destructor)tabulate_dealloc,          /* tp_dealloc */
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
        Py_TPFLAGS_BASETYPE,         /* tp_flags */
    (const char *)tabulate_doc,      /* tp_doc */
    (traverseproc)tabulate_traverse, /* tp_traverse */
    (inquiry)tabulate_clear,         /* tp_clear */
    (richcmpfunc)0,                  /* tp_richcompare */
    (Py_ssize_t)0,                   /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,  /* tp_iter */
    (iternextfunc)tabulate_next,     /* tp_iternext */
    tabulate_methods,                /* tp_methods */
    tabulate_memberlist,             /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    (descrgetfunc)0,                 /* tp_descr_get */
    (descrsetfunc)0,                 /* tp_descr_set */
    (Py_ssize_t)0,                   /* tp_dictoffset */
    (initproc)0,                     /* tp_init */
    (allocfunc)0,                    /* tp_alloc */
    (newfunc)tabulate_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,       /* tp_free */
};
