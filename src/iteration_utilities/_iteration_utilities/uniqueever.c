/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 *
 * IMPORTANT NOTE (Implementation):
 *
 * This function is almost identical to "duplicates", so any changes
 * or bugfixes should also be implemented there!!!
 *
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *key;
    PyObject *seen;
} PyIUObject_UniqueEver;

static PyTypeObject PyIUType_UniqueEver;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
uniqueever_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_UniqueEver *self;

    PyObject *iterable;
    PyObject *iterator=NULL;
    PyObject *seen=NULL;
    PyObject *key=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_everseen", kwlist,
                                     &iterable, &key)) {
        goto Fail;
    }
    if (key == Py_None) {
        key = NULL;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    seen = PyIUSeen_New();
    if (seen == NULL) {
        goto Fail;
    }
    self = (PyIUObject_UniqueEver *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(key);
    self->iterator = iterator;
    self->key = key;
    self->seen = seen;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(seen);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
uniqueever_dealloc(PyIUObject_UniqueEver *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->key);
    Py_XDECREF(self->seen);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
uniqueever_traverse(PyIUObject_UniqueEver *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->key);
    Py_VISIT(self->seen);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
uniqueever_clear(PyIUObject_UniqueEver *self)
{
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->key);
    Py_CLEAR(self->seen);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
uniqueever_next(PyIUObject_UniqueEver *self)
{
    PyObject *item=NULL, *temp=NULL;
    int ok;

    while ( (item = Py_TYPE(self->iterator)->tp_iternext(self->iterator)) ) {

        /* Use the item if key is not given, otherwise apply the key. */
        if (self->key == NULL) {
            temp = item;
        } else {
            temp = PyIU_CallWithOneArgument(self->key, item);
            if (temp == NULL) {
                goto Fail;
            }
        }

        /* Check if the item is in seen. */
        ok = PyIUSeen_ContainsAdd(self->seen, temp);
        if (ok == 0) {
            goto Notfound;
        /* Failure when looking if item. */
        } else if (ok == -1) {
            goto Fail;
        }

        /* We have found the item either in the set or list so continue. */
        if (self->key != NULL) {
            Py_DECREF(temp);
        }
        Py_DECREF(item);
    }

    return NULL;

Notfound:
    if (self->key != NULL) {
        Py_XDECREF(temp);
    }
    return item;
Fail:
    if (self->key != NULL) {
        Py_XDECREF(temp);
    }
    Py_XDECREF(item);
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
uniqueever_reduce(PyIUObject_UniqueEver *self, PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(OO)(O)", Py_TYPE(self),
                         self->iterator,
                         self->key ? self->key : Py_None,
                         self->seen);
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
uniqueever_setstate(PyIUObject_UniqueEver *self,
                    PyObject *state)
{
    PyObject *seen;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "O:unique_everseen.__setstate__", &seen)) {
        return NULL;
    }

    /* object passed in must be an instance of Seen. Otherwise the function
       calls could result in an segmentation fault.
       */
    if (!PyIUSeen_CheckExact(seen)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `Seen` instance as "
                     "first argument in the `state`, got %.200s.",
                     Py_TYPE(self)->tp_name, Py_TYPE(seen)->tp_name);
        return NULL;
    }

    Py_CLEAR(self->seen);
    self->seen = seen;
    Py_INCREF(self->seen);
    Py_RETURN_NONE;
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef uniqueever_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)uniqueever_reduce,                    /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {"__setstate__",                                    /* ml_name */
     (PyCFunction)uniqueever_setstate,                  /* ml_meth */
     METH_O,                                            /* ml_flags */
     PYIU_setstate_doc                                  /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_UniqueEver, x)
static PyMemberDef uniqueever_memberlist[] = {

    {"seen",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(seen),                                         /* offset */
     READONLY,                                          /* flags */
     uniqueever_prop_seen_doc                           /* doc */
     },

    {"key",                                             /* name */
     T_OBJECT,                                          /* type */
     OFF(key),                                          /* offset */
     READONLY,                                          /* flags */
     uniqueever_prop_key_doc                            /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

static PyTypeObject PyIUType_UniqueEver = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.unique_everseen",/* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_UniqueEver),          /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)uniqueever_dealloc,                     /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)0,                                                  /* tp_repr */
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
    (const char *)uniqueever_doc,                       /* tp_doc */
    (traverseproc)uniqueever_traverse,                  /* tp_traverse */
    (inquiry)uniqueever_clear,                          /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)uniqueever_next,                      /* tp_iternext */
    uniqueever_methods,                                 /* tp_methods */
    uniqueever_memberlist,                              /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)uniqueever_new,                            /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
