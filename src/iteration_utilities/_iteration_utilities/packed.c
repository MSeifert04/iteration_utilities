/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "packed.h"
#include "docs_reduce.h"
#include <structmember.h>

PyDoc_STRVAR(packed_prop_func_doc,
    "(callable) The function with packed arguments (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(packed_doc,
    "packed(func, /)\n"
    "--\n\n"
    "Class that always returns ``func(*x)`` when called with ``packed(func)(x)``.\n"
    "\n"
    ".. versionadded:: 0.3\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "func : callable\n"
    "    The function that should be called when the packed-instance is called.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "Creating :py:class:`~iteration_utilities.packed` instances::\n"
    "\n"
    "    >>> from iteration_utilities import packed\n"
    "    >>> from operator import eq\n"
    "    >>> five = packed(eq)\n"
    "    >>> five((2, 2))\n"
    "    True\n"
    "\n"
    "This is a convenience class that emulates the behaviour of \n"
    ":py:func:`itertools.starmap` (compared to :py:func:`map`)::\n"
    "\n"
    "    >>> from itertools import starmap\n"
    "    >>> list(map(packed(eq), [(2, 2), (3, 3), (2, 3)]))\n"
    "    [True, True, False]\n"
    "    >>> list(starmap(eq, [(2, 2), (3, 3), (2, 3)]))\n"
    "    [True, True, False]\n"
    "\n"
    "and :py:func:`~iteration_utilities.starfilter` (compared to \n"
    ":py:func:`filter`)::\n"
    "\n"
    "    >>> from iteration_utilities import starfilter\n"
    "    >>> list(filter(packed(eq), [(2, 2), (3, 3), (2, 3)]))\n"
    "    [(2, 2), (3, 3)]\n"
    "    >>> list(starfilter(eq, [(2, 2), (3, 3), (2, 3)]))\n"
    "    [(2, 2), (3, 3)]\n"
    "\n"
    "Of course in these cases the appropriate `star`-function can be used but \n"
    "in case a function does not have such a convenience function already \n"
    ":py:class:`~iteration_utilities.packed` can be used.\n"
);

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
packed_new(PyTypeObject *type,
           PyObject *args,
           PyObject *kwargs)
{
    PyIUObject_Packed *self;

    PyObject *func;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "packed", 1, 1, &func)) {
        return NULL;
    }

    /* Create struct */
    self = (PyIUObject_Packed *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
packed_dealloc(PyIUObject_Packed *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
packed_traverse(PyIUObject_Packed *self,
                visitproc visit,
                void *arg)
{
    Py_VISIT(self->func);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
packed_clear(PyIUObject_Packed *self)
{
    Py_CLEAR(self->func);
    return 0;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
packed_call(PyIUObject_Packed *self,
            PyObject *args,
            PyObject *kwargs)
{
    PyObject *packed, *newpacked, *res;
    if (!PyArg_UnpackTuple(args, "packed.__call__", 1, 1, &packed)) {
        return NULL;
    }

    Py_INCREF(packed);

    if (!PyTuple_CheckExact(packed)) {
        newpacked = PySequence_Tuple(packed);
        Py_DECREF(packed);
        if (newpacked == NULL) {
            return NULL;
        }
        packed = newpacked;
    }

    res = PyObject_Call(self->func, packed, kwargs);
    Py_DECREF(packed);
    return res;
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
packed_repr(PyIUObject_Packed *self)
{
    PyObject *result = NULL;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    result = PyUnicode_FromFormat("%s(%R)",
                                  Py_TYPE(self)->tp_name,
                                  self->func);

    Py_ReprLeave((PyObject *)self);
    return result;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
packed_reduce(PyIUObject_Packed *self, PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(O)", Py_TYPE(self), self->func);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef packed_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)packed_reduce,                        /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Packed, x)
static PyMemberDef packed_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     packed_prop_func_doc                               /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Packed = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.packed",         /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Packed),              /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)packed_dealloc,                         /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)packed_repr,                              /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)packed_call,                           /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)packed_doc,                           /* tp_doc */
    (traverseproc)packed_traverse,                      /* tp_traverse */
    (inquiry)packed_clear,                              /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    packed_methods,                                     /* tp_methods */
    packed_memberlist,                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)packed_new,                                /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
