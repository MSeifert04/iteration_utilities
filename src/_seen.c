/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
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

#define PyIUSeen_Check(o) (PyObject_TypeCheck(o, &PyIUType_Seen))
#define PyIUSeen_CheckExact(o) (Py_TYPE(o) == &PyIUType_Seen)

/******************************************************************************
 * Creates a new PyIUSeen objects with empty seenset and seenlist.
 * Returns ``NULL`` on failure with the appropriate exception.
 *****************************************************************************/

PyObject *
PyIUSeen_New(void)
{
    /* Create and fill new object. */
    PyIUObject_Seen *self;

    PyObject *seenset = NULL;

    seenset = PySet_New(NULL);
    if (seenset == NULL) {
        goto Fail;
    }
    self = PyObject_GC_New(PyIUObject_Seen, &PyIUType_Seen);
    if (self == NULL) {
        goto Fail;
    }
    self->seenset = seenset;
    self->seenlist = NULL;
    PyObject_GC_Track(self);
    return (PyObject *)self;

Fail:
    Py_XDECREF(seenset);
    return NULL;
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

    PyObject *seenset = NULL;
    PyObject *seenlist = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:Seen", kwlist,
                                     &seenset, &seenlist)) {
        goto Fail;
    }
    PYIU_NULL_IF_NONE(seenset);
    PYIU_NULL_IF_NONE(seenlist);

    if (seenset == NULL) {
        seenset = PySet_New(NULL);
        if (seenset == NULL) {
            goto Fail;
        }
    } else {
        Py_INCREF(seenset);
        if (!PySet_CheckExact(seenset)) {
            PyErr_Format(PyExc_TypeError,
                         "`seenset` argument for `Seen` must be a set or "
                         "None, not `%.200s`.",
                         Py_TYPE(seenset)->tp_name);
            goto Fail;
        }
    }

    if (seenlist != NULL && !PyList_CheckExact(seenlist)) {
        PyErr_Format(PyExc_TypeError,
                     "`seenlist` argument for `Seen` must be a list or None, "
                     "not `%.200s`.",
                     Py_TYPE(seenlist)->tp_name);
        goto Fail;
    }

    self = (PyIUObject_Seen *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(seenlist);
    self->seenset = seenset;
    self->seenlist = seenlist;

    return (PyObject *)self;

Fail:
    Py_XDECREF(seenset);
    return NULL;
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
 * Clear
 *****************************************************************************/

static int
seen_clear(PyIUObject_Seen *self)
{
    Py_CLEAR(self->seenset);
    Py_CLEAR(self->seenlist);
    return 0;
}

/******************************************************************************
 * Representation
 *****************************************************************************/

static PyObject *
seen_repr(PyIUObject_Seen *self)
{
    PyObject *repr;
    int ok;

    ok = Py_ReprEnter((PyObject*)self);
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
        return PySet_Size(self->seenset) + PyList_GET_SIZE(self->seenlist);
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

static int
PyIUSeen_ContainsAdd(PyObject *self,
                     PyObject *o)
{
    /* In the interest of keeping it fast, no check if it's really a Seen
       instance - SEGFAULT if not!
       However the API is not exported so only crashes in-library stuff.

    if (!PyIUSeen_CheckExact(self)) {
        PyErr_SetString(PyExc_TypeError, "only works for `Seen` instances.");
        return NULL;
    }
    */
    return seen_containsadd_direct((PyIUObject_Seen *)self, o);
}

static PyObject *
seen_containsadd(PyObject *self,
                 PyObject *o)
{
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

/******************************************************************************
 * Type
 *****************************************************************************/

static PySequenceMethods seen_as_sequence = {
    (lenfunc)seen_len,                                  /* sq_length */
    (binaryfunc)0,                                      /* sq_concat */
    (ssizeargfunc)0,                                    /* sq_repeat */
    (ssizeargfunc)0,                                    /* sq_item */
    (void *)0,                                          /* unused */
    (ssizeobjargproc)0,                                 /* sq_ass_item */
    (void *)0,                                          /* unused */
    (objobjproc)seen_containsnoadd_direct,              /* sq_contains */
    (binaryfunc)0,                                      /* sq_inplace_concat */
    (ssizeargfunc)0,                                    /* sq_inplace_repeat */
};

static PyMethodDef seen_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)seen_reduce,                          /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {"contains_add",                                    /* ml_name */
     (PyCFunction)seen_containsadd,                     /* ml_meth */
     METH_O,                                            /* ml_flags */
     seen_containsadd_doc                               /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Seen, x)
static PyMemberDef seen_memberlist[] = {

    {"seenset",                                         /* name */
     T_OBJECT,                                          /* type */
     OFF(seenset),                                      /* offset */
     READONLY,                                          /* flags */
     seen_prop_seenset_doc                              /* doc */
     },

    {"seenlist",                                        /* name */
     T_OBJECT,                                          /* type */
     OFF(seenlist),                                     /* offset */
     READONLY,                                          /* flags */
     seen_prop_seenlist_doc                             /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Seen = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.Seen",           /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Seen),                /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)seen_dealloc,                           /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)seen_repr,                                /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)&seen_as_sequence,             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash  */
    (ternaryfunc)0,                                     /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)0,                                    /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)seen_doc,                             /* tp_doc */
    (traverseproc)seen_traverse,                        /* tp_traverse */
    (inquiry)seen_clear,                                /* tp_clear */
    (richcmpfunc)seen_richcompare,                      /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    seen_methods,                                       /* tp_methods */
    seen_memberlist,                                    /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)seen_new,                                  /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
