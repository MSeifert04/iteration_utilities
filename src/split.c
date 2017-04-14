/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *delimiter;
    Py_ssize_t maxsplit;
    int keep_delimiter;
    int keep_before;
    int keep_after;
    int cmp;
    PyObject *next;
    PyObject *funcargs;
} PyIUObject_Split;

PyTypeObject PyIUType_Split;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
split_new(PyTypeObject *type,
          PyObject *args,
          PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", "maxsplit",
                             "keep", "keep_before", "keep_after", "eq", NULL};
    PyIUObject_Split *self;

    PyObject *iterable;
    PyObject *delimiter;
    PyObject *iterator=NULL;
    PyObject *funcargs=NULL;
    Py_ssize_t maxsplit = -1;  /* -1 means no maxsplit! */
    int keep_delimiter = 0, keep_before = 0, keep_after = 0, cmp = 0;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|niiii:split", kwlist,
                                     &iterable, &delimiter,
                                     &maxsplit, &keep_delimiter,
                                     &keep_before, &keep_after, &cmp)) {
        goto Fail;
    }
    if (maxsplit <= -2) {
        PyErr_SetString(PyExc_ValueError,
                        "`maxsplit` argument for `split` must be -1 or greater.");
        goto Fail;
    }
    if ((keep_delimiter ? 1 : 0) + (keep_before ? 1 : 0) +
            (keep_after ? 1 : 0) > 1 ) {
        PyErr_SetString(PyExc_ValueError,
                        "only one or none of `keep`, `keep_before`, "
                        "`keep_after` arguments for `split` may be set.");
        goto Fail;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    if (!cmp) {
        funcargs = PyTuple_New(1);
        if (funcargs == NULL) {
            goto Fail;
        }
    }
    self = (PyIUObject_Split *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_INCREF(delimiter);
    self->iterator = iterator;
    self->delimiter = delimiter;
    self->maxsplit = maxsplit;
    self->keep_delimiter = keep_delimiter;
    self->keep_before = keep_before;
    self->keep_after = keep_after;
    self->cmp = cmp;
    self->next = NULL;
    self->funcargs = funcargs;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(funcargs);
    return NULL;

}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
split_dealloc(PyIUObject_Split *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->delimiter);
    Py_XDECREF(self->next);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
split_traverse(PyIUObject_Split *self,
               visitproc visit,
               void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->delimiter);
    Py_VISIT(self->next);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
split_clear(PyIUObject_Split *self)
{
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->delimiter);
    Py_CLEAR(self->next);
    Py_CLEAR(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
split_next(PyIUObject_Split *self)
{
    PyObject *result, *item=NULL, *val=NULL;
    int ok;

    /* Create a list to hold the result. */
    result = PyList_New(0);
    if (result == NULL) {
        goto Fail;
    }

    /* If there was already a value saved as next just append it and return it.
       This case happenes if someone wants to keep the delimiter.
       */
    if (self->next != NULL) {
        ok = PyList_Append(result, self->next);
        Py_DECREF(self->next);
        self->next = NULL;
        if (ok == 0) {
            if ( !self->keep_after ) {
                return result;
            }
        } else {
            goto Fail;
        }
    }

    while ( (item = Py_TYPE(self->iterator)->tp_iternext(self->iterator)) ) {
        /* Compare the value to the delimiter or call the delimiter function on
           it to determine if we should split here.
           */
        if (self->cmp) {
            ok = PyObject_RichCompareBool(self->delimiter, item, Py_EQ);

        } else {
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, goto Fail);
            val = PyObject_Call(self->delimiter, self->funcargs, NULL);
            if (val == NULL) {
                goto Fail;
            }
            ok = PyObject_IsTrue(val);
        }

        /* Value is not delimiter or we already used up the maxsplit
           splittings.
           */
        if (ok == 0 || self->maxsplit == 0) {
            ok = PyList_Append(result, item);
            if (ok != 0) {
                goto Fail;
            }
            Py_DECREF(item);
            Py_XDECREF(val);

        /* Split here. */
        } else if (ok == 1) {
            /* Decrement maxsplit */
            if (self->maxsplit != -1) {
                self->maxsplit--;
            }
            /* Keep the delimiter (if requested) as next item. */
            if (self->keep_delimiter || self->keep_after) {
                self->next = item;
            } else if (self->keep_before) {
                ok = PyList_Append(result, item);
                if (ok != 0) {
                    goto Fail;
                }
                Py_DECREF(item);
            } else {
                Py_DECREF(item);
            }
            Py_XDECREF(val);
            return result;

        } else {
            goto Fail;
        }
    }

    if (PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
            PyErr_Clear();
        } else {
            Py_DECREF(result);
            return NULL;
        }
    }

    /* Only return the last result if there is something in it. */
    if (PyList_GET_SIZE(result) == 0) {
        Py_DECREF(result);
        return NULL;
    } else {
        return result;
    }


Fail:
    Py_XDECREF(result);
    Py_XDECREF(item);
    Py_XDECREF(val);
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
split_reduce(PyIUObject_Split *self)
{
    /* Seperate cases depending on next == NULL because otherwise "None"
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
                             self->keep_delimiter,
                             self->keep_before,
                             self->keep_after,
                             self->cmp);
    } else {
        return Py_BuildValue("O(OOniiii)(O)", Py_TYPE(self),
                             self->iterator,
                             self->delimiter,
                             self->maxsplit,
                             self->keep_delimiter,
                             self->keep_before,
                             self->keep_after,
                             self->cmp,
                             self->next);
    }
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
split_setstate(PyIUObject_Split *self,
               PyObject *state)
{
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

    Py_CLEAR(self->next);
    Py_INCREF(next);
    self->next = next;

    Py_RETURN_NONE;
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef split_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)split_reduce,                         /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {"__setstate__",                                    /* ml_name */
     (PyCFunction)split_setstate,                       /* ml_meth */
     METH_O,                                            /* ml_flags */
     PYIU_setstate_doc                                  /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Split, x)
static PyMemberDef split_memberlist[] = {

    {"key",                                             /* name */
     T_OBJECT,                                          /* type */
     OFF(delimiter),                                    /* offset */
     READONLY,                                          /* flags */
     split_prop_key_doc                                 /* doc */
     },

    {"maxsplit",                                        /* name */
     T_PYSSIZET,                                        /* type */
     OFF(maxsplit),                                     /* offset */
     READONLY,                                          /* flags */
     split_prop_maxsplit_doc /* doc */
     },

    {"keep",                                            /* name */
     T_BOOL,                                            /* type */
     OFF(keep_delimiter),                               /* offset */
     READONLY,                                          /* flags */
     split_prop_keep_doc /* doc */
     },

    {"keep_before",                                     /* name */
     T_BOOL,                                            /* type */
     OFF(keep_before),                                  /* offset */
     READONLY,                                          /* flags */
     split_prop_keepbefore_doc                          /* doc */
     },

    {"keep_after",                                      /* name */
     T_BOOL,                                            /* type */
     OFF(keep_after),                                   /* offset */
     READONLY,                                          /* flags */
     split_prop_keepafter_doc                           /* doc */
     },

    {"eq",                                              /* name */
     T_BOOL,                                            /* type */
     OFF(cmp),                                          /* offset */
     READONLY,                                          /* flags */
     split_prop_eq_doc                                  /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Split = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.split",          /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Split),               /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)split_dealloc,                          /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)0,                                        /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)0,                                     /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)split_doc,                            /* tp_doc */
    (traverseproc)split_traverse,                       /* tp_traverse */
    (inquiry)split_clear,                               /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)split_next,                           /* tp_iternext */
    split_methods,                                      /* tp_methods */
    split_memberlist,                                   /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,                     /* tp_alloc */
    (newfunc)split_new,                                 /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
