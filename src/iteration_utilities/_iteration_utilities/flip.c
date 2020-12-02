/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "flip.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    flip_prop_func_doc,
    "(callable) The function with flipped arguments (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    flip_doc,
    "flip(x, /)\n"
    "--\n\n"
    "Class that reverses the positional arguments to a `func` when called.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "func : callable\n"
    "    The function that should be called with the flipped (reversed) arguments.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "This can be used to alter the call to a function::\n"
    "\n"
    "    >>> from iteration_utilities import flip\n"
    "    >>> from functools import partial\n"
    "    >>> flipped = flip(isinstance)\n"
    "    >>> isfloat = partial(flipped, float)\n"
    "\n"
    "    >>> isfloat(10)\n"
    "    False\n"
    "    >>> isfloat(11.25)\n"
    "    True\n");

#if PyIU_USE_VECTORCALL
static PyObject *flip_vectorcall(PyObject *obj, PyObject *const *args, size_t nargsf, PyObject *kwnames);
#endif

static PyObject *
flip_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    PyIUObject_Flip *self;
    PyObject *func;
    if (!PyArg_UnpackTuple(args, "flip", 1, 1, &func)) {
        return NULL;
    }

    /* If the object is another flip we can simply return it's function because
       two flips are equivalent to no flip. However subclasses should be
       excluded from this behaviour so also check that the first argument is
       in fact "flip" and not a subclass.
       */
    if (PyIU_IsTypeExact(func, &PyIUType_Flip) && type == &PyIUType_Flip) {
        PyObject *ret = ((PyIUObject_Flip *)func)->func;
        Py_INCREF(ret);
        return ret;
    }
    self = (PyIUObject_Flip *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
#if PyIU_USE_VECTORCALL
    self->vectorcall = flip_vectorcall;
#endif
    return (PyObject *)self;
}

static void
flip_dealloc(PyIUObject_Flip *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

static int
flip_traverse(PyIUObject_Flip *self, visitproc visit, void *arg) {
    Py_VISIT(self->func);
    return 0;
}

static int
flip_clear(PyIUObject_Flip *self) {
    Py_CLEAR(self->func);
    return 0;
}

#if PyIU_USE_VECTORCALL

static PyObject *
flip_vectorcall(PyObject *obj, PyObject *const *args, size_t nargsf, PyObject *kwnames) {
    PyObject *result;
    PyObject *small_stack[PyIU_SMALL_ARG_STACK_SIZE];
    PyObject **stack = small_stack;
    Py_ssize_t i;
    Py_ssize_t j;
    PyIUObject_Flip *self = (PyIUObject_Flip *)obj;

    Py_ssize_t n_pos_args = PyVectorcall_NARGS(nargsf);
    Py_ssize_t n_args = n_pos_args + (kwnames == NULL ? 0 : PyTuple_GET_SIZE(kwnames));

    if (n_pos_args <= 1) {
        return PyIU_PyObject_Vectorcall(self->func, args, n_pos_args, kwnames);
    }

    if (n_args > PyIU_SMALL_ARG_STACK_SIZE) {
        stack = PyIU_AllocatePyObjectArray(n_args);
        if (stack == NULL) {
            return PyErr_NoMemory();
        }
    }

    for (i = 0, j = n_pos_args - 1; i < n_pos_args; i++, j--) {
        stack[i] = args[j];
    }
    memcpy(stack + n_pos_args, args + n_pos_args, (n_args - n_pos_args) * sizeof(PyObject *));
    result = PyIU_PyObject_Vectorcall(self->func, stack, n_pos_args, kwnames);
    if (stack != small_stack) {
        PyMem_Free(stack);
    }
    return result;
}

#else

static PyObject *
flip_call(PyIUObject_Flip *self, PyObject *args, PyObject *kwargs) {
    PyObject *result;
    PyObject *tmpargs;

    if (PyTuple_GET_SIZE(args) >= 2) {
        tmpargs = PyIU_TupleReverse(args);
        result = PyObject_Call(self->func, tmpargs, kwargs);
        Py_DECREF(tmpargs);
    } else {
        result = PyObject_Call(self->func, args, kwargs);
    }

    return result;
}

#endif

static PyObject *
flip_repr(PyIUObject_Flip *self) {
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

static PyObject *
flip_reduce(PyIUObject_Flip *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(O)", Py_TYPE(self),
                         self->func);
}

static PyMethodDef flip_methods[] = {
    {
        "__reduce__",             /* ml_name */
        (PyCFunction)flip_reduce, /* ml_meth */
        METH_NOARGS,              /* ml_flags */
        PYIU_reduce_doc           /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef flip_memberlist[] = {
    {
        "func",                          /* name */
        T_OBJECT,                        /* type */
        offsetof(PyIUObject_Flip, func), /* offset */
        READONLY,                        /* flags */
        flip_prop_func_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Flip = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.flip", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Flip),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                           /* tp_itemsize */
    /* methods */
    (destructor)flip_dealloc, /* tp_dealloc */
#if PyIU_USE_VECTORCALL
    offsetof(PyIUObject_Flip, vectorcall), /* tp_vectorcall_offset */
#else
    (printfunc)0,           /* tp_print */
#endif
    (getattrfunc)0,         /* tp_getattr */
    (setattrfunc)0,         /* tp_setattr */
    0,                      /* tp_reserved */
    (reprfunc)flip_repr,    /* tp_repr */
    (PyNumberMethods *)0,   /* tp_as_number */
    (PySequenceMethods *)0, /* tp_as_sequence */
    (PyMappingMethods *)0,  /* tp_as_mapping */
    (hashfunc)0,            /* tp_hash */
#if PyIU_USE_VECTORCALL
    (ternaryfunc)PyVectorcall_Call, /* tp_call */
#else
    (ternaryfunc)flip_call, /* tp_call */
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
    ,                            /* tp_flags */
    (const char *)flip_doc,      /* tp_doc */
    (traverseproc)flip_traverse, /* tp_traverse */
    (inquiry)flip_clear,         /* tp_clear */
    (richcmpfunc)0,              /* tp_richcompare */
    (Py_ssize_t)0,               /* tp_weaklistoffset */
    (getiterfunc)0,              /* tp_iter */
    (iternextfunc)0,             /* tp_iternext */
    flip_methods,                /* tp_methods */
    flip_memberlist,             /* tp_members */
    0,                           /* tp_getset */
    0,                           /* tp_base */
    0,                           /* tp_dict */
    (descrgetfunc)0,             /* tp_descr_get */
    (descrsetfunc)0,             /* tp_descr_set */
    (Py_ssize_t)0,               /* tp_dictoffset */
    (initproc)0,                 /* tp_init */
    (allocfunc)0,                /* tp_alloc */
    (newfunc)flip_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,   /* tp_free */
};
