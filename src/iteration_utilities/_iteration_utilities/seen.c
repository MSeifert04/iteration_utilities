/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "seen.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    seen_prop_seenset_doc,
    "(:py:class:`set`) The (hashable) seen values (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    seen_prop_seenlist_doc,
    "(:py:class:`list` or None) The (unhashable) seen values (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    seen_doc,
    "Seen(seenset=None, seenlist=None)\n"
    "--\n\n"
    "Helper class which adds the items after each :py:meth:`.contains_add` check.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "seenset : :py:class:`set` or None, optional\n"
    "    A :py:class:`set` containing initial values.\n"
    "\n"
    "seenlist : :py:class:`list` or None, optional\n"
    "    A :py:class:`list` containing only unhashable initial values.\n"
    "    \n"
    "    .. note::\n"
    "        The `seenlist` should not contain hashable values (these will \n"
    "        be ignored for all practical purposes)!\n"
    "\n"
    "Examples\n"
    "--------\n"
    "This class adds each item after :py:meth:`.contains_add` call but also \n"
    "supports normal :py:meth:`in <.__contains__>` operations::\n"
    "\n"
    "    >>> from iteration_utilities import Seen\n"
    "    >>> x = Seen()\n"
    "    >>> # normal \"in\" operations do not add the element to the instance\n"
    "    >>> 1 in x\n"
    "    False\n"
    "    >>> 1 in x\n"
    "    False\n"
    "    \n"
    "    >>> # \"contains_add\" checks if the item is contained but also adds it\n"
    "    >>> x.contains_add(2)\n"
    "    False\n"
    "    >>> x.contains_add(2)\n"
    "    True\n"
    "    >>> x\n"
    "    iteration_utilities.Seen({2})\n"
    "    \n"
    "    >>> x.contains_add([1, 2])\n"
    "    False\n"
    "    >>> [1, 2] in x\n"
    "    True\n"
    "    >>> x\n"
    "    iteration_utilities.Seen({2}, seenlist=[[1, 2]])\n"
    "\n"
    "This class does only support :py:meth:`in <.__contains__>`, \n"
    ":py:meth:`== <.__eq__>`, :py:meth:`\\!= <.__ne__>` and \n"
    ":py:meth:`len <.__len__>`.\n"
    "It is mostly included because it unified the code in \n"
    ":py:func:`~iteration_utilities.duplicates`,\n"
    ":py:func:`~iteration_utilities.unique_everseen`, and \n"
    ":py:func:`~iteration_utilities.all_distinct` and might be useful in other \n"
    "applications.\n");

PyDoc_STRVAR(
    seen_containsadd_doc,
    "contains_add($self, o, /)\n"
    "--\n\n"
    "Check if `o` is already contained in `self` and return the result.\n"
    "But also adds `o` to `self` if it's not contained.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "o : any type\n"
    "    The object to check if it's contained in `self` and added to\n"
    "    `self` if not.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "contained : :py:class:`bool`\n"
    "    ``True`` if `o` is contained in `self` otherwise ``False``.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "A simple example::\n"
    "\n"
    "    >>> from iteration_utilities import Seen\n"
    "    >>> x = Seen()\n"
    "    >>> 10 in x\n"
    "    False\n"
    "    >>> x.contains_add(10)\n"
    "    False\n"
    "    >>> 10 in x\n"
    "    True\n"
    "    >>> x.contains_add(10)\n"
    "    True\n"
    "    >>> x\n"
    "    iteration_utilities.Seen({10})\n");

/******************************************************************************
 *
 * Helper class that wraps a set and list. This class is simply for convenience
 * so "contains and add if not contained"-operations are seperated from the
 * logic of "uniques_everseen", "all_distinct" doesn't need to contain it.
 *
 * TODO: This refactoring slowed down the code a bit (not-negligible in my
 *       opinion) but it makes it much more concise. Need to check for
 *       possibilities to improve performance.
 *
 * Public macros:
 * - PyIUSeen_Check(PyObject*)
 *
 * Public functions:
 * - PyIUSeen_New(void) -> PyObject*
 * - PyIUSeen_Size(PyIUObject_Seen*) -> Py_ssize_t
 * - PyIUSeen_Contains(PyIUObject_Seen*, PyObject*) -> int
 *          (-1 failure, 0 not contained, 1 contained)
 *****************************************************************************/

/******************************************************************************
 * Creates a new PyIUSeen objects with empty seenset and seenlist.
 * Returns ``NULL`` on failure with the appropriate exception.
 *****************************************************************************/

PyObject *
PyIUSeen_New(void) {
    /* Create and fill new object. */
    PyIUObject_Seen *self;
    PyObject *seenset;

    seenset = PySet_New(NULL);
    if (seenset == NULL) {
        return NULL;
    }
    self = PyObject_GC_New(PyIUObject_Seen, &PyIUType_Seen);
    if (self == NULL) {
        Py_DECREF(seenset);
        return NULL;
    }
    self->seenset = seenset;
    self->seenlist = NULL;
    PyObject_GC_Track(self);
    return (PyObject *)self;
}

static PyObject *
seen_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"seenset", "seenlist", NULL};
    PyIUObject_Seen *self;

    PyObject *seenset = NULL;
    PyObject *seenlist = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:Seen", kwlist,
                                     &seenset, &seenlist)) {
        return NULL;
    }
    if (seenset == Py_None) {
        seenset = NULL;
    }
    if (seenlist == Py_None) {
        seenlist = NULL;
    }

    if (seenset == NULL) {
        seenset = PySet_New(NULL);
        if (seenset == NULL) {
            return NULL;
        }
    } else {
        if (!PyIU_IsTypeExact(seenset, &PySet_Type)) {
            PyErr_Format(PyExc_TypeError,
                         "`seenset` argument for `Seen` must be a set or "
                         "None, not `%.200s`.",
                         Py_TYPE(seenset)->tp_name);
            return NULL;
        }
        Py_INCREF(seenset);
    }

    if (seenlist != NULL && !PyList_CheckExact(seenlist)) {
        PyErr_Format(PyExc_TypeError,
                     "`seenlist` argument for `Seen` must be a list or None, "
                     "not `%.200s`.",
                     Py_TYPE(seenlist)->tp_name);
        Py_DECREF(seenset);
        return NULL;
    }

    self = (PyIUObject_Seen *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(seenset);
        return NULL;
    }
    Py_XINCREF(seenlist);
    self->seenset = seenset;
    self->seenlist = seenlist;
    return (PyObject *)self;
}

static void
seen_dealloc(PyIUObject_Seen *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->seenset);
    Py_XDECREF(self->seenlist);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static int
seen_traverse(PyIUObject_Seen *self, visitproc visit, void *arg) {
    Py_VISIT(self->seenset);
    Py_VISIT(self->seenlist);
    return 0;
}

static int
seen_clear(PyIUObject_Seen *self) {
    Py_CLEAR(self->seenset);
    Py_CLEAR(self->seenlist);
    return 0;
}

static PyObject *
seen_repr(PyIUObject_Seen *self) {
    PyObject *repr;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    if (self->seenlist != NULL && PyList_GET_SIZE(self->seenlist) > 0) {
        repr = PyUnicode_FromFormat("%s(%R, seenlist=%R)",
                                    Py_TYPE(self)->tp_name,
                                    self->seenset, self->seenlist);
    } else {
        repr = PyUnicode_FromFormat("%s(%R)",
                                    Py_TYPE(self)->tp_name, self->seenset);
    }

    Py_ReprLeave((PyObject *)self);
    return repr;
}

static PyObject *
seen_richcompare(PyObject *v, PyObject *w, int op) {
    PyIUObject_Seen *l;
    PyIUObject_Seen *r;
    int ok;

    /* Only allow == and != for now.  */
    switch (op) {
        case Py_EQ:
        case Py_NE:
            break;
        default:
            Py_RETURN_NOTIMPLEMENTED;
    }
    if (!PyIUSeen_Check(v) || !(PyIUSeen_Check(w))) {
        PyErr_SetString(PyExc_TypeError,
                        "`Seen` instances can only compared to other `Seen` "
                        "instances.");
        return NULL;
    }
    l = (PyIUObject_Seen *)v;
    r = (PyIUObject_Seen *)w;

    /* Check if either both have seenlists or none. */
    if ((l->seenlist == NULL && r->seenlist != NULL && PyList_GET_SIZE(r->seenlist)) ||
        (r->seenlist == NULL && l->seenlist != NULL && PyList_GET_SIZE(l->seenlist))) {
        if (op == Py_NE) {
            Py_RETURN_TRUE;
        } else {
            Py_RETURN_FALSE;
        }
        /* If both have seenlists then compare them. */
    } else if (l->seenlist != NULL && r->seenlist != NULL) {
        ok = PyObject_RichCompareBool(l->seenlist, r->seenlist, op);
        if (op == Py_EQ && ok == 0) {
            Py_RETURN_FALSE;
        } else if (op == Py_NE && ok == 1) {
            Py_RETURN_TRUE;
        } else if (ok == -1) {
            return NULL;
        }
    }
    ok = PyObject_RichCompareBool(l->seenset, r->seenset, op);
    if (ok == 1) {
        Py_RETURN_TRUE;
    } else if (ok == 0) {
        Py_RETURN_FALSE;
    } else {
        return NULL;
    }
}

static PyObject *
seen_reduce(PyIUObject_Seen *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OO)", Py_TYPE(self),
                         self->seenset,
                         self->seenlist ? self->seenlist : Py_None);
}

/******************************************************************************
 * Len
 *
 * May be not overflow safe ...
 *****************************************************************************/

Py_ssize_t
PyIUSeen_Size(PyIUObject_Seen *self) {
    assert(self != NULL);

    if (self->seenlist != NULL) {
        return PySet_Size(self->seenset) + PyList_GET_SIZE(self->seenlist);
    } else {
        return PySet_Size(self->seenset);
    }
}

static Py_ssize_t
seen_len(PyObject *self) {
    return PyIUSeen_Size((PyIUObject_Seen *)self);
}

/******************************************************************************
 * ContainsAdd
 *
 * Checks if the object is contained in seenset or seenlist and returns
 * 1  - if the item was found
 * 0  - if the item was not found
 * -1 - if some exception happened.
 *****************************************************************************/

static int
seen_containsadd_direct(PyIUObject_Seen *self, PyObject *o) {
    int ok;
    Py_ssize_t oldsize = PySet_GET_SIZE(self->seenset);
    ok = PySet_Add(self->seenset, o);
    if (ok == 0) {
        /* No error: If the size of the set hasn't changed then the item was
          contained in the set already. */
        return PySet_GET_SIZE(self->seenset) == oldsize ? 1 : 0;
    } else {
        /* Clear TypeErrors because they are thrown if the object is
           unhashable.
           */
        if (PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                PyErr_Clear();
            } else {
                return -1;
            }
        }
        if (self->seenlist == NULL && !(self->seenlist = PyList_New(0))) {
            return -1;
        }
        ok = PySequence_Contains(self->seenlist, o);

        if (ok == 1) {
            /* Unhashable, found */
            return 1;
        } else if (ok == 0) {
            /* Unhashable, not found */
            if (PyList_Append(self->seenlist, o) == -1) {
                return -1;
            }
            return 0;
        } else {
            /* Unhashable and exception when looking it up in the list. */
            return -1;
        }
    }
}

static int
seen_containsnoadd_direct(PyIUObject_Seen *self, PyObject *o) {
    int ok = PySet_Contains(self->seenset, o);
    if (ok != -1) {
        return ok;
    } else {
        /* Clear TypeErrors because they are thrown if the object is
           unhashable.
           */
        if (PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_TypeError)) {
                PyErr_Clear();
            } else {
                return -1;
            }
        }
        if (self->seenlist == NULL) {
            return 0;
        }
        return PySequence_Contains(self->seenlist, o);
    }
}

int PyIUSeen_ContainsAdd(PyObject *self, PyObject *o) {
    assert(self != NULL && PyIU_IsTypeExact(self, &PyIUType_Seen));

    return seen_containsadd_direct((PyIUObject_Seen *)self, o);
}

static PyObject *
seen_containsadd(PyObject *self, PyObject *o) {
    int ok;
    ok = seen_containsadd_direct((PyIUObject_Seen *)self, o);
    if (ok == 0) {
        Py_RETURN_FALSE;
    } else if (ok == 1) {
        Py_RETURN_TRUE;
    } else {
        return NULL;
    }
}

static PySequenceMethods seen_as_sequence = {
    (lenfunc)seen_len,                     /* sq_length */
    (binaryfunc)0,                         /* sq_concat */
    (ssizeargfunc)0,                       /* sq_repeat */
    (ssizeargfunc)0,                       /* sq_item */
    (void *)0,                             /* unused */
    (ssizeobjargproc)0,                    /* sq_ass_item */
    (void *)0,                             /* unused */
    (objobjproc)seen_containsnoadd_direct, /* sq_contains */
    (binaryfunc)0,                         /* sq_inplace_concat */
    (ssizeargfunc)0,                       /* sq_inplace_repeat */
};

static PyMethodDef seen_methods[] = {
    {
        "__reduce__",             /* ml_name */
        (PyCFunction)seen_reduce, /* ml_meth */
        METH_NOARGS,              /* ml_flags */
        PYIU_reduce_doc           /* ml_doc */
    },
    {
        "contains_add",                /* ml_name */
        (PyCFunction)seen_containsadd, /* ml_meth */
        METH_O,                        /* ml_flags */
        seen_containsadd_doc           /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef seen_memberlist[] = {
    {
        "seenset",                          /* name */
        T_OBJECT,                           /* type */
        offsetof(PyIUObject_Seen, seenset), /* offset */
        READONLY,                           /* flags */
        seen_prop_seenset_doc               /* doc */
    },
    {
        "seenlist",                          /* name */
        T_OBJECT,                            /* type */
        offsetof(PyIUObject_Seen, seenlist), /* offset */
        READONLY,                            /* flags */
        seen_prop_seenlist_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Seen = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.Seen", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Seen),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                           /* tp_itemsize */
    /* methods */
    (destructor)seen_dealloc,               /* tp_dealloc */
    (printfunc)0,                           /* tp_print */
    (getattrfunc)0,                         /* tp_getattr */
    (setattrfunc)0,                         /* tp_setattr */
    0,                                      /* tp_reserved */
    (reprfunc)seen_repr,                    /* tp_repr */
    (PyNumberMethods *)0,                   /* tp_as_number */
    (PySequenceMethods *)&seen_as_sequence, /* tp_as_sequence */
    (PyMappingMethods *)0,                  /* tp_as_mapping */
    (hashfunc)0,                            /* tp_hash  */
    (ternaryfunc)0,                         /* tp_call */
    (reprfunc)0,                            /* tp_str */
    (getattrofunc)0,                        /* tp_getattro */
    (setattrofunc)0,                        /* tp_setattro */
    (PyBufferProcs *)0,                     /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,       /* tp_flags */
    (const char *)seen_doc,        /* tp_doc */
    (traverseproc)seen_traverse,   /* tp_traverse */
    (inquiry)seen_clear,           /* tp_clear */
    (richcmpfunc)seen_richcompare, /* tp_richcompare */
    (Py_ssize_t)0,                 /* tp_weaklistoffset */
    (getiterfunc)0,                /* tp_iter */
    (iternextfunc)0,               /* tp_iternext */
    seen_methods,                  /* tp_methods */
    seen_memberlist,               /* tp_members */
    0,                             /* tp_getset */
    0,                             /* tp_base */
    0,                             /* tp_dict */
    (descrgetfunc)0,               /* tp_descr_get */
    (descrsetfunc)0,               /* tp_descr_set */
    (Py_ssize_t)0,                 /* tp_dictoffset */
    (initproc)0,                   /* tp_init */
    (allocfunc)0,                  /* tp_alloc */
    (newfunc)seen_new,             /* tp_new */
    (freefunc)PyObject_GC_Del,     /* tp_free */
};
