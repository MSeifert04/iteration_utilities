/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *iterator;
} PyIUObject_Starfilter;

static PyTypeObject PyIUType_Starfilter;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
starfilter_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"pred", "iterable", NULL};
    PyIUObject_Starfilter *self;

    PyObject *iterable;
    PyObject *func;
    PyObject *iterator=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:starfilter", kwlist,
                                     &func, &iterable)) {
        goto Fail;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    self = (PyIUObject_Starfilter *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_INCREF(func);
    self->iterator = iterator;
    self->func = func;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
starfilter_dealloc(PyIUObject_Starfilter *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
starfilter_traverse(PyIUObject_Starfilter *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->func);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
starfilter_clear(PyIUObject_Starfilter *self)
{
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->func);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
starfilter_next(PyIUObject_Starfilter *self)
{
    PyObject *item, *val, *newargs;
    int ok;

    while ( (item = Py_TYPE(self->iterator)->tp_iternext(self->iterator)) ) {
        if (!PyTuple_CheckExact(item)) {
            newargs = PySequence_Tuple(item);
            if (newargs == NULL) {
                Py_DECREF(item);
                return NULL;
            }
        } else {
            Py_INCREF(item);
            newargs = item;
        }
        val = PyObject_Call(self->func, newargs, NULL);
        Py_DECREF(newargs);
        if (val == NULL) {
            Py_DECREF(item);
            return NULL;
        }
        ok = PyObject_IsTrue(val);
        Py_DECREF(val);

        if (ok > 0) {
            return item;
        }
        Py_DECREF(item);
        if (ok < 0) {
            return NULL;
        }
    }
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
starfilter_reduce(PyIUObject_Starfilter *self, PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(OO)", Py_TYPE(self), self->func, self->iterator);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef starfilter_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)starfilter_reduce,                    /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Starfilter, x)
static PyMemberDef starfilter_memberlist[] = {

    {"pred",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     starfilter_prop_pred_doc                           /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

static PyTypeObject PyIUType_Starfilter = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.starfilter",     /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Starfilter),          /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)starfilter_dealloc,                     /* tp_dealloc */
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
    (const char *)starfilter_doc,                       /* tp_doc */
    (traverseproc)starfilter_traverse,                  /* tp_traverse */
    (inquiry)starfilter_clear,                          /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)starfilter_next,                      /* tp_iternext */
    starfilter_methods,                                 /* tp_methods */
    starfilter_memberlist,                              /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,                     /* tp_alloc */
    (newfunc)starfilter_new,                            /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
