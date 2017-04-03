/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *keyfunc;
    PyObject *lastitem;
    PyObject *funcargs;
} PyIUObject_UniqueJust;

PyTypeObject PyIUType_UniqueJust;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
uniquejust_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_UniqueJust *self;

    PyObject *iterable;
    PyObject *iterator=NULL;
    PyObject *keyfunc=NULL;
    PyObject *funcargs=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_justseen", kwlist,
                                     &iterable, &keyfunc)) {
        goto Fail;
    }

    /* Create and fill struct */
    PYIU_NULL_IF_NONE(keyfunc);

    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    if (keyfunc != NULL) {
        funcargs = PyTuple_New(1);
        if (funcargs == NULL) {
            goto Fail;
        }
    }
    self = (PyIUObject_UniqueJust *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(keyfunc);
    self->iterator = iterator;
    self->keyfunc = keyfunc;
    self->lastitem = NULL;
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
uniquejust_dealloc(PyIUObject_UniqueJust *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->keyfunc);
    Py_XDECREF(self->lastitem);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
uniquejust_traverse(PyIUObject_UniqueJust *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->keyfunc);
    Py_VISIT(self->lastitem);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
uniquejust_next(PyIUObject_UniqueJust *self)
{
    PyObject *item, *old, *val=NULL;
    int ok;

    while ( (item = (*Py_TYPE(self->iterator)->tp_iternext)(self->iterator)) ) {

        /* Apply keyfunc or use the original */
        if (self->keyfunc == NULL) {
            Py_INCREF(item);
            val = item;
        } else {
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, goto Fail);
            val = PyObject_Call(self->keyfunc, self->funcargs, NULL);
            if (val == NULL) {
                goto Fail;
            }
        }

        /* If no lastitem set it to the current and simply return the item. */
        if (self->lastitem == NULL) {
            self->lastitem = val;
            return item;
        }

        /* Otherwise compare it with the last item and only return it if it
           differs. */
        ok = PyObject_RichCompareBool(val, self->lastitem, Py_EQ);

        /* Not equal */
        if (ok == 0) {
            old = self->lastitem;
            self->lastitem = val;
            Py_DECREF(old);
            return item;

        /* Failure */
        } else if (ok < 0) {
            goto Fail;
        }

        Py_DECREF(val);
        Py_DECREF(item);
    }
    return NULL;

Fail:
    Py_DECREF(item);
    Py_XDECREF(val);
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
uniquejust_reduce(PyIUObject_UniqueJust *self)
{
    /* Seperate cases depending on lastitem == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a last item or
       that the last item was None.
       Better to make an "if" than to introduce another variable depending on
       lastitem == NULL.
       */
    if (self->lastitem != NULL) {
        return Py_BuildValue("O(OO)(O)", Py_TYPE(self),
                             self->iterator,
                             self->keyfunc ? self->keyfunc : Py_None,
                             self->lastitem);
    } else {
        return Py_BuildValue("O(OO)", Py_TYPE(self),
                             self->iterator,
                             self->keyfunc ? self->keyfunc : Py_None);
    }
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
uniquejust_setstate(PyIUObject_UniqueJust *self,
                    PyObject *state)
{
    PyObject *lastitem;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "O:unique_justseen.__setstate__", &lastitem)) {
        return NULL;
    }

    /* No need to check the type of "lastitem" because any python object is
       valid.
       */

    Py_CLEAR(self->lastitem);
    self->lastitem = lastitem;
    Py_INCREF(self->lastitem);

    Py_RETURN_NONE;
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef uniquejust_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction) uniquejust_reduce,                   /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {"__setstate__",                                    /* ml_name */
     (PyCFunction)uniquejust_setstate,                  /* ml_meth */
     METH_O,                                            /* ml_flags */
     PYIU_setstate_doc                                  /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_UniqueJust, x)
static PyMemberDef uniquejust_memberlist[] = {

    {"key",                                             /* name */
     T_OBJECT,                                          /* type */
     OFF(keyfunc),                                      /* offset */
     READONLY,                                          /* flags */
     uniquejust_prop_key_doc                            /* doc */
     },

    {"lastseen",                                        /* name */
     T_OBJECT,                                          /* type */
     OFF(lastitem),                                     /* offset */
     READONLY,                                          /* flags */
     uniquejust_prop_lastseen_doc                       /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_UniqueJust = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.unique_justseen",/* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_UniqueJust),          /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)uniquejust_dealloc,                     /* tp_dealloc */
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
    (const char *)uniquejust_doc,                       /* tp_doc */
    (traverseproc)uniquejust_traverse,                  /* tp_traverse */
    (inquiry)0,                                         /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)uniquejust_next,                      /* tp_iternext */
    uniquejust_methods,                                 /* tp_methods */
    uniquejust_memberlist,                              /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)uniquejust_new,                            /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
