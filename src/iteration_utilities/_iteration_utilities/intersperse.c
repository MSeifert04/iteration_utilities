/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "intersperse.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"
#include "docs_setstate.h"

PyDoc_STRVAR(
    intersperse_prop_fillvalue_doc,
    "(any type) The interspersed fillvalue (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    intersperse_doc,
    "intersperse(iterable, e)\n"
    "--\n\n"
    "Alternately yield an item from the `iterable` and `e`. Recipe based on the\n"
    "homonymous function in the `more-itertools` package ([0]_) but significantly\n"
    "modified.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    The iterable to intersperse.\n"
    "\n"
    "e : any type\n"
    "    The value with which to intersperse the `iterable`.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "interspersed : generator\n"
    "    Interspersed `iterable` as generator.\n"
    "\n"
    "Notes\n"
    "-----\n"
    "This is similar to\n"
    "``itertools.chain.from_iterable(zip(iterable, itertools.repeat(e)))`` except\n"
    "that `intersperse` does not yield `e` as last item.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "A few simple examples::\n"
    "\n"
    "    >>> from iteration_utilities import intersperse\n"
    "    >>> list(intersperse([1,2,3], 0))\n"
    "    [1, 0, 2, 0, 3]\n"
    "\n"
    "    >>> list(intersperse('abc', 'x'))\n"
    "    ['a', 'x', 'b', 'x', 'c']\n"
    "\n"
    "References\n"
    "----------\n"
    ".. [0] https://github.com/erikrose/more-itertools\n");

static PyObject *
intersperse_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "e", NULL};
    PyIUObject_Intersperse *self;
    PyObject *iterable;
    PyObject *filler;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:intersperse", kwlist,
                                     &iterable, &filler)) {
        return NULL;
    }
    self = (PyIUObject_Intersperse *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    Py_INCREF(filler);
    self->filler = filler;
    self->nextitem = NULL;
    self->started = 0;
    return (PyObject *)self;
}

static void
intersperse_dealloc(PyIUObject_Intersperse *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->filler);
    Py_XDECREF(self->nextitem);
    Py_TYPE(self)->tp_free(self);
}

static int
intersperse_traverse(PyIUObject_Intersperse *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->filler);
    Py_VISIT(self->nextitem);
    return 0;
}

static int
intersperse_clear(PyIUObject_Intersperse *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->filler);
    Py_CLEAR(self->nextitem);
    return 0;
}

static PyObject *
intersperse_next(PyIUObject_Intersperse *self) {
    if (self->nextitem == NULL) {
        PyObject *item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
        if (item == NULL) {
            return NULL;
        }
        /* If we haven't started we return the first item, otherwise we set
           the nextitem but return the filler.
           */
        if (self->started == 0) {
            self->started = 1;
            return item;
        }
        self->nextitem = item;
        Py_INCREF(self->filler);
        return self->filler;
    } else {
        /* There was a next item, return it and reset nextitem. */
        PyObject *item = self->nextitem;
        self->nextitem = NULL;
        return item;
    }
}

static PyObject *
intersperse_reduce(PyIUObject_Intersperse *self, PyObject *Py_UNUSED(args)) {
    /* Separate cases depending on nextitem == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a next item or
       that the next item was None.
       Better to make an "if" than to introduce another variable depending on
       nextitem == NULL.
       */
    PyObject *value;
    if (self->nextitem == NULL) {
        value = Py_BuildValue("O(OO)(i)", Py_TYPE(self),
                              self->iterator,
                              self->filler,
                              self->started);
    } else {
        value = Py_BuildValue("O(OO)(iO)", Py_TYPE(self),
                              self->iterator,
                              self->filler,
                              self->started,
                              self->nextitem);
    }
    return value;
}

static PyObject *
intersperse_setstate(PyIUObject_Intersperse *self, PyObject *state) {
    int started;
    PyObject *nextitem = NULL;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "i|O:intersperse.__setstate__",
                          &started, &nextitem)) {
        return NULL;
    }

    /* No need to check the type of "next" because any python object is
       valid.

       However we can make sure that "nextitem == NULL" if "started == 0"
       because otherwise this would produce an invalid "intersperse" instance.
       Not a segfault but this comparison isn't really costly.
       */
    if (started == 0 && nextitem != NULL) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the second argument "
                     "in the `state` is not given when the first argument is "
                     "0, got %.200s.",
                     Py_TYPE(self)->tp_name, Py_TYPE(nextitem)->tp_name);
        return NULL;
    }

    Py_XINCREF(nextitem);
    Py_XSETREF(self->nextitem, nextitem);
    self->started = started;
    Py_RETURN_NONE;
}

static PyObject *
intersperse_lengthhint(PyIUObject_Intersperse *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }
    /* We need to multiply the length_hint with 2, to make sure this doesn't
       trigger undefined behaviour we convert it to "size_t" which can always
       hold the result (because it's maximum value is 2 * max(py_ssize_t) + 1)
       Also "LengthHint" always returns >= -1 and we already catched the case
       where it was -1 so it's not-negative (which could be a problem in the
       signed -> unsigned conversion).
       */
    if (self->started == 0) {
        if (len == 0) {
            return PyLong_FromLong(0);
        }
        return PyLong_FromSize_t((size_t)len * 2 - 1);
    } else if (self->nextitem == NULL) {
        return PyLong_FromSize_t((size_t)len * 2);
    } else {
        /* The iterator is always one step advanced! */
        return PyLong_FromSize_t((size_t)len * 2 + 1);
    }
}

static PyMethodDef intersperse_methods[] = {
    {
        "__length_hint__",                   /* ml_name */
        (PyCFunction)intersperse_lengthhint, /* ml_meth */
        METH_NOARGS,                         /* ml_flags */
        PYIU_lenhint_doc                     /* ml_doc */
    },
    {
        "__reduce__",                    /* ml_name */
        (PyCFunction)intersperse_reduce, /* ml_meth */
        METH_NOARGS,                     /* ml_flags */
        PYIU_reduce_doc                  /* ml_doc */
    },
    {
        "__setstate__",                    /* ml_name */
        (PyCFunction)intersperse_setstate, /* ml_meth */
        METH_O,                            /* ml_flags */
        PYIU_setstate_doc                  /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef intersperse_memberlist[] = {
    {
        "fillvalue",                              /* name */
        T_OBJECT,                                 /* type */
        offsetof(PyIUObject_Intersperse, filler), /* offset */
        READONLY,                                 /* flags */
        intersperse_prop_fillvalue_doc            /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Intersperse = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.intersperse", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Intersperse),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                  /* tp_itemsize */
    /* methods */
    (destructor)intersperse_dealloc,       /* tp_dealloc */
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
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    (const char *)intersperse_doc,      /* tp_doc */
    (traverseproc)intersperse_traverse, /* tp_traverse */
    (inquiry)intersperse_clear,         /* tp_clear */
    (richcmpfunc)0,                     /* tp_richcompare */
    (Py_ssize_t)0,                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,     /* tp_iter */
    (iternextfunc)intersperse_next,     /* tp_iternext */
    intersperse_methods,                /* tp_methods */
    intersperse_memberlist,             /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    (descrgetfunc)0,                    /* tp_descr_get */
    (descrsetfunc)0,                    /* tp_descr_set */
    (Py_ssize_t)0,                      /* tp_dictoffset */
    (initproc)0,                        /* tp_init */
    (allocfunc)0,                       /* tp_alloc */
    (newfunc)intersperse_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,          /* tp_free */
};
