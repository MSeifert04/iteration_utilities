/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "constant.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    constant_prop_item_doc,
    "(any type) The value that is returned each time the instance is called "
    "(readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    constant_doc,
    "constant(item, /)\n"
    "--\n\n"
    "Class that always returns a constant value when called.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "item : any type\n"
    "    The item that should be returned when called.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "Creating :py:class:`~iteration_utilities.constant` instances::\n"
    "\n"
    "    >>> from iteration_utilities import constant\n"
    "    >>> five = constant(5)\n"
    "    >>> five()\n"
    "    5\n"
    "    >>> ten = constant(10)\n"
    "    >>> # Any parameters are ignored\n"
    "    >>> ten(5, give_me=100)\n"
    "    10\n"
    "\n"
    "There are already three predefined instances:\n"
    "\n"
    "- :py:func:`~iteration_utilities.return_True`: equivalent to ``constant(True)``.\n"
    "- :py:func:`~iteration_utilities.return_False`: equivalent to ``constant(False)``.\n"
    "- :py:func:`~iteration_utilities.return_None`: equivalent to ``constant(None)``.\n"
    "\n"
    "For example::\n"
    "\n"
    "    >>> from iteration_utilities import return_True, return_False, return_None\n"
    "    >>> return_True()\n"
    "    True\n"
    "    >>> return_False()\n"
    "    False\n"
    "    >>> return_None()\n"
    "    >>> return_None() is None\n"
    "    True\n");

#if PyIU_USE_VECTORCALL
static PyObject *constant_vectorcall(PyObject *obj, PyObject *const *args, size_t nargsf, PyObject *kwnames);
#endif

PyObject *
PyIUConstant_New(PyObject *value) {
    assert(value != NULL);
    PyIUObject_Constant *self;

    self = PyObject_GC_New(PyIUObject_Constant, &PyIUType_Constant);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(value);
    self->item = value;
#if PyIU_USE_VECTORCALL
    self->vectorcall = constant_vectorcall;
#endif
    PyObject_GC_Track(self);
    return (PyObject *)self;
}

static PyObject *
constant_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    PyIUObject_Constant *self;
    PyObject *item;

    if (!PyArg_UnpackTuple(args, "constant", 1, 1, &item)) {
        return NULL;
    }
    self = (PyIUObject_Constant *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(item);
    self->item = item;
#if PyIU_USE_VECTORCALL
    self->vectorcall = constant_vectorcall;
#endif
    return (PyObject *)self;
}

static void
constant_dealloc(PyIUObject_Constant *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->item);
    Py_TYPE(self)->tp_free(self);
}

static int
constant_traverse(PyIUObject_Constant *self, visitproc visit, void *arg) {
    Py_VISIT(self->item);
    return 0;
}

static int
constant_clear(PyIUObject_Constant *self) {
    Py_CLEAR(self->item);
    return 0;
}

#if PyIU_USE_VECTORCALL

static PyObject *
constant_vectorcall(PyObject *obj, PyObject *const *args, size_t nargsf, PyObject *kwnames) {
    PyObject *tmp = ((PyIUObject_Constant *)obj)->item;
    Py_INCREF(tmp);
    return tmp;
}

#else

static PyObject *
constant_call(PyIUObject_Constant *self, PyObject *args, PyObject *kwargs) {
    Py_INCREF(self->item);
    return self->item;
}
#endif

static PyObject *
constant_repr(PyIUObject_Constant *self) {
    PyObject *result = NULL;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    result = PyUnicode_FromFormat("%s(%R)",
                                  Py_TYPE(self)->tp_name,
                                  self->item);

    Py_ReprLeave((PyObject *)self);
    return result;
}

static PyObject *
constant_reduce(PyIUObject_Constant *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(O)", Py_TYPE(self), self->item);
}

static PyMethodDef constant_methods[] = {
    {
        "__reduce__",                 /* ml_name */
        (PyCFunction)constant_reduce, /* ml_meth */
        METH_NOARGS,                  /* ml_flags */
        PYIU_reduce_doc               /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef constant_memberlist[] = {
    {
        "item",                              /* name */
        T_OBJECT,                            /* type */
        offsetof(PyIUObject_Constant, item), /* offset */
        READONLY,                            /* flags */
        constant_prop_item_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Constant = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.constant", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Constant),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                               /* tp_itemsize */
    /* methods */
    (destructor)constant_dealloc, /* tp_dealloc */
#if PyIU_USE_VECTORCALL
    offsetof(PyIUObject_Constant, vectorcall), /* tp_vectorcall_offset */
#else
    (printfunc)0,               /* tp_print */
#endif
    (getattrfunc)0,          /* tp_getattr */
    (setattrfunc)0,          /* tp_setattr */
    0,                       /* tp_reserved */
    (reprfunc)constant_repr, /* tp_repr */
    (PyNumberMethods *)0,    /* tp_as_number */
    (PySequenceMethods *)0,  /* tp_as_sequence */
    (PyMappingMethods *)0,   /* tp_as_mapping */
    (hashfunc)0,             /* tp_hash */
#if PyIU_USE_VECTORCALL
    (ternaryfunc)PyVectorcall_Call, /* tp_call */
#else
    (ternaryfunc)constant_call, /* tp_call */
#endif
    (reprfunc)0,                           /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc)0,                       /* tp_setattro */
    (PyBufferProcs *)0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE
#if PyIU_USE_VECTORCALL
    #if PyIU_USE_UNDERSCORE_VECTORCALL
        | _Py_TPFLAGS_HAVE_VECTORCALL
    #else
        | Py_TPFLAGS_HAVE_VECTORCALL
    #endif
#endif
    ,                                /* tp_flags */
    (const char *)constant_doc,      /* tp_doc */
    (traverseproc)constant_traverse, /* tp_traverse */
    (inquiry)constant_clear,         /* tp_clear */
    (richcmpfunc)0,                  /* tp_richcompare */
    (Py_ssize_t)0,                   /* tp_weaklistoffset */
    (getiterfunc)0,                  /* tp_iter */
    (iternextfunc)0,                 /* tp_iternext */
    constant_methods,                /* tp_methods */
    constant_memberlist,             /* tp_members */
    0,                               /* tp_getset */
    0,                               /* tp_base */
    0,                               /* tp_dict */
    (descrgetfunc)0,                 /* tp_descr_get */
    (descrsetfunc)0,                 /* tp_descr_set */
    (Py_ssize_t)0,                   /* tp_dictoffset */
    (initproc)0,                     /* tp_init */
    (allocfunc)0,                    /* tp_alloc */
    (newfunc)constant_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,       /* tp_free */
};
