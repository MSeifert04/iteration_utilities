/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "complement.h"
#include "docs_reduce.h"
#include <structmember.h>

PyDoc_STRVAR(complement_prop_func_doc,
    "(callable) The function that is complemented (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(complement_doc,
    "complement(func)\n"
    "--\n\n"
    "Invert a predicate function. There is a homonymous function in the `toolz` \n"
    "package ([0]_) but significantly modified.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "func : callable\n"
    "    The function to complement.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "complemented_func : callable\n"
    "    The complement to `func`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "`complement` is equivalent to ``lambda x: not x()`` but significantly faster::\n"
    "\n"
    "    >>> from iteration_utilities import complement\n"
    "    >>> from iteration_utilities import is_None\n"
    "    >>> is_not_None = complement(is_None)\n"
    "    >>> list(filter(is_not_None, [1,2,None,3,4,None]))\n"
    "    [1, 2, 3, 4]\n"
    "\n"
    ".. note::\n"
    "    The example code could also be done with :py:func:`itertools.filterfalse` \n"
    "    or :py:func:`iteration_utilities.is_not_None`.\n"
    "\n"
    "References\n"
    "----------\n"
    ".. [0] https://toolz.readthedocs.io/en/latest/index.html\n"
);

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
complement_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    PyIUObject_Complement *self;

    PyObject *func;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "complement", 1, 1, &func)) {
        return NULL;
    }

    /* Create struct */
    self = (PyIUObject_Complement *)type->tp_alloc(type, 0);
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
complement_dealloc(PyIUObject_Complement *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
complement_traverse(PyIUObject_Complement *self,
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
complement_clear(PyIUObject_Complement *self)
{
    Py_CLEAR(self->func);
    return 0;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
complement_call(PyIUObject_Complement *self,
                PyObject *args,
                PyObject *kwargs)
{
    PyObject *temp;
    int res;

    /* "not func(*args, **kwargs)" */
    temp = PyObject_Call(self->func, args, kwargs);
    if (temp == NULL) {
        return NULL;
    }
    res = PyObject_Not(temp);
    Py_DECREF(temp);

    if (res == 1) {
        Py_RETURN_TRUE;
    } else if (res == 0) {
        Py_RETURN_FALSE;
    } else {
        return NULL;
    }
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
complement_repr(PyIUObject_Complement *self)
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
complement_reduce(PyIUObject_Complement *self, PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(O)", Py_TYPE(self), self->func);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef complement_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)complement_reduce,                    /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Complement, x)
static PyMemberDef complement_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     complement_prop_func_doc                           /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Complement = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.complement",     /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Complement),          /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)complement_dealloc,                     /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)complement_repr,                          /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)complement_call,                       /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)complement_doc,                       /* tp_doc */
    (traverseproc)complement_traverse,                  /* tp_traverse */
    (inquiry)complement_clear,                          /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    complement_methods,                                 /* tp_methods */
    complement_memberlist,                              /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)complement_new,                            /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
