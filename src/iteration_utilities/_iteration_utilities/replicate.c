/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "replicate.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"
#include "docs_setstate.h"

PyDoc_STRVAR(
    replicate_prop_times_doc,
    "(:py:class:`int`) The number of times each item is replicated (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    replicate_prop_timescurrent_doc,
    "(:py:class:`int`) A counter indicating how often the current item was "
    "already replicated (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    replicate_prop_current_doc,
    "(any type) The item that is currently replicated (readonly).\n"
    "\n"
    "Only available if an item has been replicated.\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    replicate_doc,
    "replicate(iterable, times)\n"
    "--\n\n"
    "Replicates each item in the `iterable` for `times` times.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    The iterable which contains the elements to be replicated.\n"
    "\n"
    "times : positive :py:class:`int`\n"
    "    The number of `times` each element is replicated.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "repeated_iterable : generator\n"
    "    A generator containing the replicated items from `iterable`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    ">>> from iteration_utilities import replicate\n"
    ">>> ''.join(replicate('abc', 3))\n"
    "'aaabbbccc'\n"
    "\n"
    ">>> list(replicate(range(3), 5))\n"
    "[0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2]\n");

static PyObject *
replicate_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "times", NULL};
    PyIUObject_Replicate *self;
    PyObject *iterable;
    Py_ssize_t times;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "On:replicate", kwlist,
                                     &iterable, &times)) {
        return NULL;
    }
    if (times <= 1) {
        PyErr_Format(PyExc_ValueError,
                     "`times` argument for `replicate` must be greater "
                     "than 1, not `%zd`",
                     times);
        return NULL;
    }
    self = (PyIUObject_Replicate *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->current = NULL;
    self->repeattotal = times;
    self->repeatcurrent = 0;
    return (PyObject *)self;
}

static void
replicate_dealloc(PyIUObject_Replicate *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->current);
    Py_TYPE(self)->tp_free(self);
}

static int
replicate_traverse(PyIUObject_Replicate *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->current);
    return 0;
}

static int
replicate_clear(PyIUObject_Replicate *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->current);
    return 0;
}

static PyObject *
replicate_next(PyIUObject_Replicate *self) {
    if (self->current == NULL) {
        /* First time around we need to get the first element of the iterator
           to fill the current. */
        self->current = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
    } else if (self->repeatcurrent == self->repeattotal) {
        /* If we had x repeats then we also need to get the next element, and
           dereference the old one. */
        PyObject *item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
        Py_SETREF(self->current, item);
        self->repeatcurrent = 0;
    }
    if (self->current == NULL) {
        /* In case something unexpected happened or the iterator finished we can
           stop now. */
        if (PyErr_Occurred() && PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        }
        return NULL;
    }

    /* Otherwise just return the current item. */
    self->repeatcurrent++;
    Py_INCREF(self->current);
    return self->current;
}

static PyObject *
replicate_reduce(PyIUObject_Replicate *self, PyObject *Py_UNUSED(args)) {
    /* Separate cases depending on current == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a current item or
       that the current item was None.
       Better to make an "if" than to introduce another variable depending on
       current == NULL.
       */
    if (self->current == NULL) {
        return Py_BuildValue("O(On)", Py_TYPE(self),
                             self->iterator,
                             self->repeattotal);
    } else {
        return Py_BuildValue("O(On)(On)", Py_TYPE(self),
                             self->iterator,
                             self->repeattotal,
                             self->current,
                             self->repeatcurrent);
    }
}

static PyObject *
replicate_setstate(PyIUObject_Replicate *self, PyObject *state) {
    PyObject *current;
    Py_ssize_t repeatcurrent;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "On:replicate.__setstate__",
                          &current, &repeatcurrent)) {
        return NULL;
    }

    if (repeatcurrent < 0 || repeatcurrent > self->repeattotal) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected a that the second item "
                     "in the `state` is greater or equal to zero and below "
                     "the `times` (%zd), not `%zd`.",
                     Py_TYPE(self)->tp_name, self->repeattotal, repeatcurrent);
        return NULL;
    }

    Py_INCREF(current);
    Py_XSETREF(self->current, current);
    self->repeatcurrent = repeatcurrent;
    Py_RETURN_NONE;
}

static PyObject *
replicate_lengthhint(PyIUObject_Replicate *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }

    /* Check if it is safe (no overflow) to multiply it. */
    if (len > PY_SSIZE_T_MAX / self->repeattotal) {
        PyErr_SetString(PyExc_OverflowError,
                        "cannot fit 'int' into an index-sized "
                        "integer");
        return NULL;
    }
    len *= self->repeattotal;

    if (self->current != NULL) {
        /* We need to avoid signed integer overflow so do the operation on
           size_t instead. "repeattotal" >= "repeatcurrent" so we only
           deal with positive values here and we're overflow safe when doing
           the operation on (size_t) because 2*PY_SSIZE_T_MAX is still
           below SIZE_T_MAX. We also don't need to check for overflow because
           we can simply return "PyLong_FromSize_t", which will fail when
           someone else wants it as Py_ssize_t (later).
           */
        size_t ulen = (size_t)len;
        ulen += (size_t)(self->repeattotal - self->repeatcurrent);
        return PyLong_FromSize_t(ulen);
    }
    return PyLong_FromSsize_t(len);
}

static PyMethodDef replicate_methods[] = {
    {
        "__length_hint__",                 /* ml_name */
        (PyCFunction)replicate_lengthhint, /* ml_meth */
        METH_NOARGS,                       /* ml_flags */
        PYIU_lenhint_doc                   /* ml_doc */
    },
    {
        "__reduce__",                  /* ml_name */
        (PyCFunction)replicate_reduce, /* ml_meth */
        METH_NOARGS,                   /* ml_flags */
        PYIU_reduce_doc                /* ml_doc */
    },
    {
        "__setstate__",                  /* ml_name */
        (PyCFunction)replicate_setstate, /* ml_meth */
        METH_O,                          /* ml_flags */
        PYIU_setstate_doc                /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef replicate_memberlist[] = {
    {
        "times",                                     /* name */
        T_PYSSIZET,                                  /* type */
        offsetof(PyIUObject_Replicate, repeattotal), /* offset */
        READONLY,                                    /* flags */
        replicate_prop_times_doc                     /* doc */
    },
    {
        "timescurrent",                                /* name */
        T_PYSSIZET,                                    /* type */
        offsetof(PyIUObject_Replicate, repeatcurrent), /* offset */
        READONLY,                                      /* flags */
        replicate_prop_timescurrent_doc                /* doc */
    },
    {
        "current",                               /* name */
        T_OBJECT_EX,                             /* type */
        offsetof(PyIUObject_Replicate, current), /* offset */
        READONLY,                                /* flags */
        replicate_prop_current_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Replicate = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.replicate", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Replicate),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                /* tp_itemsize */
    /* methods */
    (destructor)replicate_dealloc,         /* tp_dealloc */
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
    (const char *)replicate_doc,      /* tp_doc */
    (traverseproc)replicate_traverse, /* tp_traverse */
    (inquiry)replicate_clear,         /* tp_clear */
    (richcmpfunc)0,                   /* tp_richcompare */
    (Py_ssize_t)0,                    /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,   /* tp_iter */
    (iternextfunc)replicate_next,     /* tp_iternext */
    replicate_methods,                /* tp_methods */
    replicate_memberlist,             /* tp_members */
    0,                                /* tp_getset */
    0,                                /* tp_base */
    0,                                /* tp_dict */
    (descrgetfunc)0,                  /* tp_descr_get */
    (descrsetfunc)0,                  /* tp_descr_set */
    (Py_ssize_t)0,                    /* tp_dictoffset */
    (initproc)0,                      /* tp_init */
    (allocfunc)PyType_GenericAlloc,   /* tp_alloc */
    (newfunc)replicate_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,        /* tp_free */
};
