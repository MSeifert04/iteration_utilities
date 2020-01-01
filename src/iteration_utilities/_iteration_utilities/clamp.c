/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "clamp.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"

PyDoc_STRVAR(
    clamp_prop_low_doc,
    "(any type) The lower bound for `clamp` (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(
    clamp_prop_high_doc,
    "(any type) The upper bound for `clamp` (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(
    clamp_prop_inclusive_doc,
    "(:py:class:`bool`) Are the bounds inclusive (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(
    clamp_prop_remove_doc,
    "(:py:class:`bool`) Remove the outliers or clamp them to nearest bound "
    "(readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    clamp_doc,
    "clamp(iterable, low=None, high=None, inclusive=False, remove=True)\n"
    "--\n\n"
    "Remove values which are not between `low` and `high`.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    Clamp the values from this `iterable`.\n"
    "\n"
    "low : any type, optional\n"
    "    The lower bound for clamp. If not given or ``None`` there is no lower \n"
    "    bound.\n"
    "\n"
    "high : any type, optional\n"
    "    The upper bound for clamp. If not given or ``None`` there is no upper \n"
    "    bound.\n"
    "\n"
    "inclusive : :py:class:`bool`, optional\n"
    "    If ``True`` also remove values that are equal to `low` and `high`.\n"
    "    Default is ``False``.\n"
    "\n"
    "remove : :py:class:`bool`, optional\n"
    "    If ``True`` remove the items outside the range given by ``low`` and\n"
    "    ``high``, otherwise replace them with ``low`` if they are lower or\n"
    "    ``high`` if they are higher.\n"
    "    Default is ``True``.\n"
    "\n"
    "    .. versionadded:: 0.2\n"
    "\n"
    "Returns\n"
    "-------\n"
    "clamped : generator\n"
    "    A generator containing the values of `iterable` which are between `low`\n"
    "    and `high`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "This function is equivalent to a generator expression like:\n"
    "``(item for item in iterable if low <= item <= high)`` or\n"
    "``(item for item in iterable if low < item < high)`` if `inclusive=True`.\n"
    "Or a similar `filter`: ``filter(lambda item: low <= item <= high, iterable)``\n"
    "But it also allows for either ``low`` or ``high`` to be ignored and is faster.\n"
    "Some simple examples::\n"
    "\n"
    "    >>> from iteration_utilities import clamp\n"
    "    >>> list(clamp(range(5), low=2))\n"
    "    [2, 3, 4]\n"
    "    >>> list(clamp(range(5), high=2))\n"
    "    [0, 1, 2]\n"
    "    >>> list(clamp(range(1000), low=2, high=8, inclusive=True))\n"
    "    [3, 4, 5, 6, 7]\n"
    "\n"
    "If ``remove=False`` the function will replace values instead::\n"
    "\n"
    "    >>> list(clamp(range(10), low=4, high=8, remove=False))\n"
    "    [4, 4, 4, 4, 4, 5, 6, 7, 8, 8]\n");

static PyObject *
clamp_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "low", "high", "inclusive", "remove", NULL};
    PyIUObject_Clamp *self;
    PyObject *iterable;
    PyObject *low = NULL;
    PyObject *high = NULL;
    int inclusive = 0;
    int remove = 1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOpp:clamp", kwlist,
                                     &iterable, &low, &high, &inclusive, &remove)) {
        return NULL;
    }
    self = (PyIUObject_Clamp *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    /* None cannot be compared so it's unlikely we exclude use-cases by
       allowing low=None as equivalent to not giving any "low" argument.
       */
    self->low = low == Py_None ? NULL : low;
    Py_XINCREF(self->low);
    self->high = high == Py_None ? NULL : high;
    Py_XINCREF(self->high);
    self->inclusive = inclusive;
    self->remove = remove;
    return (PyObject *)self;
}

static void
clamp_dealloc(PyIUObject_Clamp *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->low);
    Py_XDECREF(self->high);
    Py_TYPE(self)->tp_free(self);
}

static int
clamp_traverse(PyIUObject_Clamp *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->low);
    Py_VISIT(self->high);
    return 0;
}

static int
clamp_clear(PyIUObject_Clamp *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->low);
    Py_CLEAR(self->high);
    return 0;
}

static PyObject *
clamp_next(PyIUObject_Clamp *self) {
    PyObject *item;
    int res;

    while ((item = Py_TYPE(self->iterator)->tp_iternext(self->iterator))) {
        /* Check if it's smaller than the lower bound. */
        if (self->low != NULL) {
            res = PyObject_RichCompareBool(item, self->low,
                                           self->inclusive ? Py_LE : Py_LT);
            if (res == 1) {
                Py_DECREF(item);
                if (!(self->remove)) {
                    Py_INCREF(self->low);
                    return self->low;
                }
                continue;
            } else if (res == -1) {
                Py_DECREF(item);
                return NULL;
            }
        }
        /* Check if it's bigger than the upper bound. */
        if (self->high != NULL) {
            res = PyObject_RichCompareBool(item, self->high,
                                           self->inclusive ? Py_GE : Py_GT);
            if (res == 1) {
                Py_DECREF(item);
                if (!(self->remove)) {
                    Py_INCREF(self->high);
                    return self->high;
                }
                continue;
            } else if (res == -1) {
                Py_DECREF(item);
                return NULL;
            }
        }
        /* Still here? Return the item! */
        return item;
    }
    return NULL;
}

static PyObject *
clamp_reduce(PyIUObject_Clamp *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OOOii)", Py_TYPE(self),
                         self->iterator,
                         self->low ? self->low : Py_None,
                         self->high ? self->high : Py_None,
                         self->inclusive,
                         self->remove);
}

static PyObject *
clamp_lengthhint(PyIUObject_Clamp *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t len = 0;
    /* If we don't remove outliers or there are no bounds at all we can
       determine the length. */
    if (!(self->remove) || (self->low == NULL && self->high == NULL)) {
        len = PyObject_LengthHint(self->iterator, 0);
        if (len == -1) {
            return NULL;
        }
    }
    return PyLong_FromSsize_t(len);
}

static PyObject *
clamp_get_inclusive(PyIUObject_Clamp *self, void *Py_UNUSED(closure)) {
    return PyBool_FromLong(self->inclusive);
}

static PyObject *
clamp_get_remove(PyIUObject_Clamp *self, void *Py_UNUSED(closure)) {
    return PyBool_FromLong(self->remove);
}

static PyMethodDef clamp_methods[] = {
    {
        "__length_hint__",             /* ml_name */
        (PyCFunction)clamp_lengthhint, /* ml_meth */
        METH_NOARGS,                   /* ml_flags */
        PYIU_lenhint_doc               /* ml_doc */
    },
    {
        "__reduce__",              /* ml_name */
        (PyCFunction)clamp_reduce, /* ml_meth */
        METH_NOARGS,               /* ml_flags */
        PYIU_reduce_doc            /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyGetSetDef clamp_getsetlist[] = {
    {
        "inclusive",                 /* name */
        (getter)clamp_get_inclusive, /* get */
        (setter)0,                   /* set */
        clamp_prop_inclusive_doc,    /* doc */
        (void *)NULL                 /* closure */
    },
    {
        "remove",                 /* name */
        (getter)clamp_get_remove, /* get */
        (setter)0,                /* set */
        clamp_prop_remove_doc,    /* doc */
        (void *)NULL              /* closure */
    },
    {NULL} /* sentinel */
};

static PyMemberDef clamp_memberlist[] = {
    {
        "low",                           /* name */
        T_OBJECT,                        /* type */
        offsetof(PyIUObject_Clamp, low), /* offset */
        READONLY,                        /* flags */
        clamp_prop_low_doc               /* doc */
    },
    {
        "high",                           /* name */
        T_OBJECT,                         /* type */
        offsetof(PyIUObject_Clamp, high), /* offset */
        READONLY,                         /* flags */
        clamp_prop_high_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Clamp = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.clamp", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Clamp),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                            /* tp_itemsize */
    /* methods */
    (destructor)clamp_dealloc,             /* tp_dealloc */
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
        Py_TPFLAGS_BASETYPE,        /* tp_flags */
    (const char *)clamp_doc,        /* tp_doc */
    (traverseproc)clamp_traverse,   /* tp_traverse */
    (inquiry)clamp_clear,           /* tp_clear */
    (richcmpfunc)0,                 /* tp_richcompare */
    (Py_ssize_t)0,                  /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter, /* tp_iter */
    (iternextfunc)clamp_next,       /* tp_iternext */
    clamp_methods,                  /* tp_methods */
    clamp_memberlist,               /* tp_members */
    clamp_getsetlist,               /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    (descrgetfunc)0,                /* tp_descr_get */
    (descrsetfunc)0,                /* tp_descr_set */
    (Py_ssize_t)0,                  /* tp_dictoffset */
    (initproc)0,                    /* tp_init */
    (allocfunc)PyType_GenericAlloc, /* tp_alloc */
    (newfunc)clamp_new,             /* tp_new */
    (freefunc)PyObject_GC_Del,      /* tp_free */
};
