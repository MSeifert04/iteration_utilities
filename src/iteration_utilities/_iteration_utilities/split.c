/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "split.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"

PyDoc_STRVAR(
    split_prop_key_doc,
    "(callable or any type) The function or value by which to split (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    split_prop_maxsplit_doc,
    "(:py:class:`int`) The number of maximum splits (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    split_prop_keep_doc,
    "(:py:class:`bool`) Keep the delimiter (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    split_prop_keepbefore_doc,
    "(:py:class:`bool`) Keep the delimiter as last item of the last group "
    "(readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    split_prop_keepafter_doc,
    "(:py:class:`bool`) Keep the delimiter as first item of the next group "
    "(readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    split_prop_eq_doc,
    "(:py:class:`bool`) Instead of calling :py:attr:`key` compare the items "
    "with it (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    split_doc,
    "split(iterable, key, maxsplit=-1, keep=False, keep_before=False, keep_after=False, eq=False)\n"
    "--\n\n"
    "Splits an `iterable` by a `key` function or delimiter.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    The `iterable` to split.\n"
    "\n"
    "key : callable\n"
    "    The function by which to split the `iterable` (split where\n"
    "    ``key(item) == True``).\n"
    "\n"
    "maxsplit : :py:class:`int`, optional\n"
    "    The number of maximal splits. If ``maxsplit=-1`` then there is no limit.\n"
    "    Default is ``-1``.\n"
    "\n"
    "keep : :py:class:`bool`\n"
    "    If ``True`` also include the items where ``key(item)=True`` as separate list.\n"
    "    Default is ``False``.\n"
    "\n"
    "keep_before : :py:class:`bool`\n"
    "    If ``True`` also include the items where ``key(item)=True`` in the \n"
    "    list before splitting.\n"
    "    Default is ``False``.\n"
    "\n"
    "keep_after : :py:class:`bool`\n"
    "    If ``True`` also include the items where ``key(item)=True`` as first \n"
    "    item in the list after splitting.\n"
    "    Default is ``False``.\n"
    "\n"
    "eq : :py:class:`bool`\n"
    "    If ``True`` split the `iterable` where ``key == item`` instead of\n"
    "    ``key(item) == True``. This can significantly speed up the function if a\n"
    "    single delimiter is used.\n"
    "    Default is ``False``.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "splitted_iterable : generator\n"
    "    Generator containing the splitted `iterable` (lists).\n"
    "\n"
    "Raises\n"
    "-------\n"
    "TypeError\n"
    "    If ``maxsplit`` is smaller than ``-2``. If more than one of the ``keep``\n"
    "    arguments is ``True``.\n"
    "\n"
    "Examples\n"
    "--------\n"
    ">>> from iteration_utilities import split\n"
    ">>> list(split(range(1, 10), lambda x: x%3==0))\n"
    "[[1, 2], [4, 5], [7, 8]]\n"
    "\n"
    ">>> list(split(range(1, 10), lambda x: x%3==0, keep=True))\n"
    "[[1, 2], [3], [4, 5], [6], [7, 8], [9]]\n"
    "\n"
    ">>> list(split(range(1, 10), lambda x: x%3==0, keep_before=True))\n"
    "[[1, 2, 3], [4, 5, 6], [7, 8, 9]]\n"
    "\n"
    ">>> list(split(range(1, 10), lambda x: x%3==0, keep_after=True))\n"
    "[[1, 2], [3, 4, 5], [6, 7, 8], [9]]\n"
    "\n"
    ">>> list(split(range(1, 10), lambda x: x%3==0, maxsplit=1))\n"
    "[[1, 2], [4, 5, 6, 7, 8, 9]]\n"
    "\n"
    ">>> list(split([1,2,3,4,5,3,7,8,3], 3, eq=True))\n"
    "[[1, 2], [4, 5], [7, 8]]\n");

static PyObject *
split_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "key", "maxsplit", "keep", "keep_before", "keep_after", "eq", NULL};
    PyIUObject_Split *self;
    PyObject *iterable;
    PyObject *delimiter;
    Py_ssize_t maxsplit = -1; /* -1 means no maxsplit! */
    int keep_delimiter = 0;
    int keep_before = 0;
    int keep_after = 0;
    int cmp = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|npppp:split", kwlist,
                                     &iterable, &delimiter,
                                     &maxsplit, &keep_delimiter,
                                     &keep_before, &keep_after, &cmp)) {
        return NULL;
    }
    if (maxsplit <= -2) {
        PyErr_SetString(PyExc_ValueError,
                        "`maxsplit` argument for `split` must be -1 or greater.");
        return NULL;
    }
    if ((keep_delimiter ? 1 : 0) + (keep_before ? 1 : 0) +
            (keep_after ? 1 : 0) >
        1) {
        PyErr_SetString(PyExc_ValueError,
                        "only one or none of `keep`, `keep_before`, "
                        "`keep_after` arguments for `split` may be set.");
        return NULL;
    }

    self = (PyIUObject_Split *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    Py_INCREF(delimiter);
    self->delimiter = delimiter;
    self->maxsplit = maxsplit;
    if (keep_delimiter) {
        self->keep = PyIU_Split_Keep;
    } else if (keep_before) {
        self->keep = PyIU_Split_KeepBefore;
    } else if (keep_after) {
        self->keep = PyIU_Split_KeepAfter;
    } else {
        self->keep = PyIU_Split_KeepNone;
    }
    self->cmp = cmp;
    self->next = NULL;
    return (PyObject *)self;
}

static void
split_dealloc(PyIUObject_Split *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->delimiter);
    Py_XDECREF(self->next);
    Py_TYPE(self)->tp_free(self);
}

static int
split_traverse(PyIUObject_Split *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->delimiter);
    Py_VISIT(self->next);
    return 0;
}

static int
split_clear(PyIUObject_Split *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->delimiter);
    Py_CLEAR(self->next);
    return 0;
}

static PyObject *
split_next(PyIUObject_Split *self) {
    PyObject *result;
    PyObject *item;
    int ok;

    /* Create a list to hold the result. */
    result = PyList_New(0);
    if (result == NULL) {
        return NULL;
    }

    if (self->next != NULL) {
        /* If there was already a value saved as next just append it and return it.
           This case happens if someone wants to keep the delimiter. */
        ok = PyList_Append(result, self->next);
        Py_CLEAR(self->next);
        if (ok == 0) {
            if (self->keep != PyIU_Split_KeepAfter) {
                return result;
            }
        } else {
            Py_DECREF(result);
            return NULL;
        }
    }

    while ((item = Py_TYPE(self->iterator)->tp_iternext(self->iterator))) {
        int should_split;
        /* Compare the value to the delimiter or call the delimiter function on
           it to determine if we should split here.
           */
        if (self->maxsplit == 0) {
            should_split = 0;
        } else {
            if (self->cmp) {
                should_split = PyObject_RichCompareBool(self->delimiter, item, Py_EQ);
            } else {
                PyObject *val = PyIU_CallWithOneArgument(self->delimiter, item);
                if (val == NULL) {
                    Py_DECREF(item);
                    Py_DECREF(result);
                    return NULL;
                }
                should_split = PyObject_IsTrue(val);
                Py_DECREF(val);
            }
        }

        if (should_split == 0) {
            /* Value is not delimiter or we already used up the maxsplit splittings. */
            ok = PyList_Append(result, item);
            Py_DECREF(item);
            if (ok != 0) {
                Py_DECREF(result);
                return NULL;
            }
        } else if (should_split == 1) {
            /* Split here. */
            if (self->maxsplit != -1) {
                self->maxsplit--;
            }
            /* Keep the delimiter (if requested) as next item. */
            if (self->keep == PyIU_Split_Keep || self->keep == PyIU_Split_KeepAfter) {
                self->next = item;
            } else if (self->keep == PyIU_Split_KeepBefore) {
                ok = PyList_Append(result, item);
                Py_DECREF(item);
                if (ok != 0) {
                    Py_DECREF(result);
                    return NULL;
                }
            } else {
                Py_DECREF(item);
            }
            return result;

        } else {
            Py_DECREF(item);
            Py_DECREF(result);
            return NULL;
        }
    }

    if (PyIU_ErrorOccurredClearStopIteration()) {
        Py_DECREF(result);
        return NULL;
    }

    /* Only return the last result if there is something in it. */
    if (PyList_GET_SIZE(result) == 0) {
        Py_DECREF(result);
        return NULL;
    } else {
        return result;
    }
}

static PyObject *
split_reduce(PyIUObject_Split *self, PyObject *Py_UNUSED(args)) {
    /* Separate cases depending on next == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a next item or
       that the next item was None.
       Better to make an "if" than to introduce another variable depending on
       next == NULL.
       */
    if (self->next == NULL) {
        return Py_BuildValue("O(OOniiii)", Py_TYPE(self),
                             self->iterator,
                             self->delimiter,
                             self->maxsplit,
                             self->keep == PyIU_Split_Keep,
                             self->keep == PyIU_Split_KeepBefore,
                             self->keep == PyIU_Split_KeepAfter,
                             self->cmp);
    } else {
        return Py_BuildValue("O(OOniiii)(O)", Py_TYPE(self),
                             self->iterator,
                             self->delimiter,
                             self->maxsplit,
                             self->keep == PyIU_Split_Keep,
                             self->keep == PyIU_Split_KeepBefore,
                             self->keep == PyIU_Split_KeepAfter,
                             self->cmp,
                             self->next);
    }
}

static PyObject *
split_setstate(PyIUObject_Split *self, PyObject *state) {
    PyObject *next;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "O:split.__setstate__", &next)) {
        return NULL;
    }

    /* No need to check the type of "next" because any python object is
       valid.
       */

    Py_INCREF(next);
    Py_XSETREF(self->next, next);

    Py_RETURN_NONE;
}

static PyObject *
split_getkeep(PyIUObject_Split *self, void *Py_UNUSED(closure)) {
    return PyBool_FromLong(self->keep == PyIU_Split_Keep);
}

static PyObject *
split_getkeepbefore(PyIUObject_Split *self, void *Py_UNUSED(closure)) {
    return PyBool_FromLong(self->keep == PyIU_Split_KeepBefore);
}

static PyObject *
split_getkeepafter(PyIUObject_Split *self, void *Py_UNUSED(closure)) {
    return PyBool_FromLong(self->keep == PyIU_Split_KeepAfter);
}

static PyObject *
split_get_eq(PyIUObject_Split *self, void *Py_UNUSED(closure)) {
    return PyBool_FromLong(self->cmp);
}

static PyMethodDef split_methods[] = {
    {
        "__reduce__",              /* ml_name */
        (PyCFunction)split_reduce, /* ml_meth */
        METH_NOARGS,               /* ml_flags */
        PYIU_reduce_doc            /* ml_doc */
    },
    {
        "__setstate__",              /* ml_name */
        (PyCFunction)split_setstate, /* ml_meth */
        METH_O,                      /* ml_flags */
        PYIU_setstate_doc            /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyGetSetDef split_getsetlist[] = {
    {
        "keep",                /* name */
        (getter)split_getkeep, /* get */
        (setter)0,             /* set */
        split_prop_keep_doc,   /* doc */
        (void *)NULL           /* closure */
    },
    {
        "keep_before",               /* name */
        (getter)split_getkeepbefore, /* get */
        (setter)0,                   /* set */
        split_prop_keepbefore_doc,   /* doc */
        (void *)NULL                 /* closure */
    },
    {
        "keep_after",               /* name */
        (getter)split_getkeepafter, /* get */
        (setter)0,                  /* set */
        split_prop_keepafter_doc,   /* doc */
        (void *)NULL                /* closure */
    },
    {
        "eq",                 /* name */
        (getter)split_get_eq, /* get */
        (setter)0,            /* set */
        split_prop_eq_doc,    /* doc */
        (void *)NULL          /* closure */
    },
    {NULL} /* sentinel */
};

static PyMemberDef split_memberlist[] = {
    {
        "key",                                 /* name */
        T_OBJECT,                              /* type */
        offsetof(PyIUObject_Split, delimiter), /* offset */
        READONLY,                              /* flags */
        split_prop_key_doc                     /* doc */
    },
    {
        "maxsplit",                           /* name */
        T_PYSSIZET,                           /* type */
        offsetof(PyIUObject_Split, maxsplit), /* offset */
        READONLY,                             /* flags */
        split_prop_maxsplit_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Split = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.split", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Split),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                            /* tp_itemsize */
    /* methods */
    (destructor)split_dealloc,             /* tp_dealloc */
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
    (const char *)split_doc,        /* tp_doc */
    (traverseproc)split_traverse,   /* tp_traverse */
    (inquiry)split_clear,           /* tp_clear */
    (richcmpfunc)0,                 /* tp_richcompare */
    (Py_ssize_t)0,                  /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter, /* tp_iter */
    (iternextfunc)split_next,       /* tp_iternext */
    split_methods,                  /* tp_methods */
    split_memberlist,               /* tp_members */
    split_getsetlist,               /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    (descrgetfunc)0,                /* tp_descr_get */
    (descrsetfunc)0,                /* tp_descr_set */
    (Py_ssize_t)0,                  /* tp_dictoffset */
    (initproc)0,                    /* tp_init */
    (allocfunc)PyType_GenericAlloc, /* tp_alloc */
    (newfunc)split_new,             /* tp_new */
    (freefunc)PyObject_GC_Del,      /* tp_free */
};
