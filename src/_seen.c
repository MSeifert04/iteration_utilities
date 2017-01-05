/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *
 * Helper class that wraps a set and list. This class is simply for convenience
 * so "contains and add if not contained"-operations are seperated from the
 * logic of "uniques_everseen", "all_distinct" doesn't need to contain it.
 *
 * TODO: This refactoring slowed down the code a bit (not-negligable in my
 *       opinion) but it makes it much more concise. Need to check for
 *       possibilities to improve performance.
 *
 * Public macros:
 * - PyIUSeen_Check(PyObject*)
 * - PyIUSeen_CheckExact(PyObject*)
 *
 * Public functions:
 * - PyIUSeen_New(void) -> PyObject*
 * - PyIUSeen_Size(PyIUObject_Seen*) -> Py_ssize_t
 * - PyIUSeen_Contains(PyIUObject_Seen*, PyObject*) -> int
 *          (-1 failure, 0 not contained, 1 contained)
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *seenset;
    PyObject *seenlist;
} PyIUObject_Seen;

PyTypeObject PyIUType_Seen;

#define PyIUSeen_Check(obj) (PyObject_IsInstance(obj, (PyObject*) &PyIUType_Seen))
#define PyIUSeen_CheckExact(op) (Py_TYPE(op) == &PyIUType_Seen)

/******************************************************************************
 * Creates a new PyIUSeen objects with empty seenset and seenlist.
 * Returns ``NULL`` on failure with the appropriate exception.
 *****************************************************************************/

PyObject *
PyIUSeen_New(void)
{
    /* Create and fill new object. */
    PyObject *seenset;
    PyIUObject_Seen *self;
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

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
seen_new(PyTypeObject *type,
         PyObject *args,
         PyObject *kwargs)
{
    static char *kwlist[] = {"seenset", "seenlist", NULL};
    PyIUObject_Seen *self;

    PyObject *seenset=NULL, *seenlist=NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:Seen", kwlist,
                                     &seenset, &seenlist)) {
        return NULL;
    }
    if (seenset == NULL) {
        seenset = PySet_New(NULL);
        if (seenset == NULL) {
            return NULL;
        }
    } else {
        if (PySet_Check(seenset) && PyAnySet_CheckExact(seenset)) {
            Py_INCREF(seenset);
        } else {
            PyErr_Format(PyExc_TypeError, "`seenset` must be a set.");
            return NULL;
        }
    }
    if (seenlist != NULL && !PyList_CheckExact(seenlist)) {
        if (seenlist == Py_None) {
            seenlist = NULL;
        } else {
            PyErr_Format(PyExc_TypeError, "`seenlist` must be a list.");
            return NULL;
        }
    }

    self = (PyIUObject_Seen *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(seenset);
        Py_XDECREF(seenlist);
        return NULL;
    }
    Py_XINCREF(seenlist);
    self->seenset = seenset;
    self->seenlist = seenlist;

    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
seen_dealloc(PyIUObject_Seen *self)
{
    Py_XDECREF(self->seenset);
    Py_XDECREF(self->seenlist);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
seen_traverse(PyIUObject_Seen *self,
              visitproc visit,
              void *arg)
{
    Py_VISIT(self->seenset);
    Py_VISIT(self->seenlist);
    return 0;
}

/******************************************************************************
 * Representation
 *****************************************************************************/

static PyObject *
seen_repr(PyIUObject_Seen *self)
{
    if (self->seenlist != NULL && PyList_Size(self->seenlist) > 0) {
        return PyUnicode_FromFormat("Seen(%R, unhashable=%R)",
                                    self->seenset, self->seenlist);
    } else {
        return PyUnicode_FromFormat("Seen(%R)",
                                    self->seenset);
    }
}

/******************************************************************************
 * Rich comparison
 *****************************************************************************/

static PyObject *
seen_richcompare(PyObject *v,
                 PyObject *w,
                 int op)
{
    PyIUObject_Seen *l, *r;
    int ok;

    /* Only allow == and != for now.  */
    switch (op) {
        case Py_EQ: break;
        case Py_NE: break;
        default: Py_RETURN_NOTIMPLEMENTED;
    }
    if (!PyIUSeen_CheckExact(v) || !(PyIUSeen_CheckExact(w))) {
        PyErr_Format(PyExc_TypeError,
                     "`Seen` instances can only compared to other `Seen` instances.");
        return NULL;
    }
    l = (PyIUObject_Seen *)v;
    r = (PyIUObject_Seen *)w;

    /* Check if either both have seenlists or none. */
    if ((l->seenlist == NULL && r->seenlist != NULL && PyList_Size(r->seenlist)) ||
            (r->seenlist == NULL && l->seenlist != NULL && PyList_Size(l->seenlist))) {
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

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
seen_reduce(PyIUObject_Seen *self)
{
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
PyIUSeen_Size(PyIUObject_Seen *self)
{
    if (self->seenlist != NULL) {
        return PySet_Size(self->seenset) + PyList_Size(self->seenlist);
    } else {
        return PySet_Size(self->seenset);
    }
}

static Py_ssize_t
seen_len(PyObject *self)
{
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
seen_containsadd_direct(PyIUObject_Seen *self,
                        PyObject *o)
{
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
        /* Unhashable, found */
        if (ok == 1) {
            return 1;
        /* Unhashable, not found */
        } else if (ok == 0) {
            if (PyList_Append(self->seenlist, o) == -1) {
                return -1;
            }
            return 0;
        /* Unhashable and exception when looking it up in the list. */
        } else {
            return -1;
        }
    }
}

static int
seen_containsnoadd_direct(PyIUObject_Seen *self,
                          PyObject *o)
{
    int ok;
    ok = PySet_Contains(self->seenset, o);
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

int
PyIUSeen_ContainsAdd(PyObject *self,
                     PyObject *o)
{
    return seen_containsadd_direct((PyIUObject_Seen *)self, o);
}

static PyObject *
seen_containsadd(PyObject *self,
                 PyObject *o)
{
    int ok;
    if (!PyIUSeen_CheckExact(self)) {
        PyErr_Format(PyExc_TypeError, "only works for `Seen` instances.");
        return NULL;
    }
    ok = seen_containsadd_direct((PyIUObject_Seen *)self, o);
    if (ok == 0) {
        Py_RETURN_FALSE;
    } else if (ok == 1) {
        Py_RETURN_TRUE;
    } else {
        return NULL;
    }
}

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(seen_containsadd_doc, "contains_add(o)\n\
--\n\
\n\
Check if `o` is already contained in `self` and return the result.\n\
But also adds `o` to `self` if it's not contained.\n\
\n\
Parameters\n\
----------\n\
o : any type\n\
    The object to check if it's contained in `self` and added to\n\
    `self` if not.\n\
\n\
Returns\n\
-------\n\
contained : bool\n\
    ``True`` if `o` is contained in `self` otherwise ``False``.\n\
\n\
Examples\n\
--------\n\
A simple example::\n\
\n\
    >>> from iteration_utilities import Seen\n\
    >>> x = Seen()\n\
    >>> 10 in x\n\
    False\n\
    >>> x.contains_add(10)\n\
    False\n\
    >>> 10 in x\n\
    True\n\
    >>> x.contains_add(10)\n\
    True\n\
    >>> x  #doctest: +SKIP\n\
    Seen({10})");

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef seen_methods[] = {
    {"__reduce__",   (PyCFunction)seen_reduce,      METH_NOARGS, PYIU_reduce_doc},
    {"contains_add", (PyCFunction)seen_containsadd, METH_O,      seen_containsadd_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Sequence
 *****************************************************************************/

static PySequenceMethods seen_as_sequence = {
    seen_len,                                                /* sq_length */
    0,                                                       /* sq_concat */
    0,                                                       /* sq_repeat */
    0,                                                       /* sq_item */
    0,                                                       /* sq_slice */
    0,                                                       /* sq_ass_item */
    0,                                                       /* sq_ass_slice */
    (objobjproc)seen_containsnoadd_direct,                   /* sq_contains */
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(seen_doc, "Seen(seenset, seenlist)\n\
--\n\
\n\
Helper class which adds the items after each `contains_add` check.\n\
\n\
Parameters\n\
----------\n\
seenset : set, optional\n\
    A `set` containing initial values.\n\
\n\
seenlist : list, optional\n\
    A `list` containing only unhashable initial values.\n\
    \n\
    .. note::\n\
        If the `seenlist` contains hashable values these will be ignored!\n\
\n\
Examples\n\
--------\n\
This class adds each item after `contains_add` call but also supports normal\n\
``in`` operations::\n\
\n\
    >>> from iteration_utilities import Seen\n\
    >>> x = Seen()\n\
    >>> # normal \"in\" operations do not add the element to the instance\n\
    >>> 1 in x\n\
    False\n\
    >>> 1 in x\n\
    False\n\
    \n\
    >>> # \"contains_add\" checks if the item is contained but also adds it\n\
    >>> x.contains_add(2)\n\
    False\n\
    >>> x.contains_add(2)\n\
    True\n\
    >>> x  # doctest: +SKIP\n\
    Seen({2})\n\
    \n\
    >>> x.contains_add([1, 2])\n\
    False\n\
    >>> [1, 2] in x\n\
    True\n\
    >>> x  # doctest: +SKIP\n\
    Seen({2}, unhashable=[[1, 2]])\n\
\n\
This class does only support ``in``, ``==``, ``!=`` and ``len``.\n\
It is mostly included because it unified the code in `duplicates`,\n\
`unique_everseen` and `all_distinct` and might be useful in other \n\
applications.");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Seen = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.Seen",                         /* tp_name */
    sizeof(PyIUObject_Seen),                            /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)seen_dealloc,                           /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)seen_repr,                                /* tp_repr */
    0,                                                  /* tp_as_number */
    &seen_as_sequence,                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash  */
    0,                                                  /* tp_call */
    0,                                                  /* tp_str */
    0,                                                  /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    seen_doc,                                           /* tp_doc */
    (traverseproc)seen_traverse,                        /* tp_traverse */
    0,                                                  /* tp_clear */
    seen_richcompare,                                   /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    0,                                                  /* tp_iter */
    0,                                                  /* tp_iternext */
    seen_methods,                                       /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    seen_new,                                           /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
