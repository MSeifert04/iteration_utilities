/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *cnt;
    PyObject *funcargs;
} PyIUObject_Tabulate;

static PyTypeObject PyIUType_Tabulate;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
tabulate_new(PyTypeObject *type,
             PyObject *args,
             PyObject *kwargs)
{
    static char *kwlist[] = {"func", "start", NULL};
    PyIUObject_Tabulate *self;

    PyObject *func, *cnt=NULL, *funcargs=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:tabulate", kwlist,
                                     &func, &cnt)) {
        goto Fail;
    }
    if (cnt == NULL) {
        cnt = PyIU_global_zero;
    }
    Py_INCREF(cnt);

    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        goto Fail;
    }

    /* Create and fill struct */
    self = (PyIUObject_Tabulate *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_INCREF(func);
    self->func = func;
    self->cnt = cnt;
    self->funcargs = funcargs;

    return (PyObject *)self;

Fail:
    Py_XDECREF(cnt);
    Py_XDECREF(funcargs);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
tabulate_dealloc(PyIUObject_Tabulate *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->cnt);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
tabulate_traverse(PyIUObject_Tabulate *self,
                  visitproc visit,
                  void *arg)
{
    Py_VISIT(self->func);
    Py_VISIT(self->cnt);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
tabulate_clear(PyIUObject_Tabulate *self)
{
    Py_CLEAR(self->func);
    Py_CLEAR(self->cnt);
    Py_CLEAR(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
tabulate_next(PyIUObject_Tabulate *self)
{
    PyObject *result=NULL, *tmp=NULL;
    if (self->cnt == NULL) {
        goto Fail;
    }
    /* Call the function with the current value as argument. */
    PYIU_RECYCLE_ARG_TUPLE(self->funcargs, self->cnt, return NULL);
    result = PyObject_Call(self->func, self->funcargs, NULL);
    if (result == NULL) {
        goto Fail;
    }
    /* Increment the counter. */
    tmp = self->cnt;
    self->cnt = PyNumber_Add(self->cnt, PyIU_global_one);
    Py_DECREF(tmp);
    if (self->cnt == NULL) {
        goto Fail;
    }
    /* Return the result. */
    return result;

Fail:
    Py_XDECREF(result);
    Py_XDECREF(self->cnt);
    /* Reset counter to NULL, so subsequent iterations yield StopIterations. */
    self->cnt = NULL;
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
tabulate_reduce(PyIUObject_Tabulate *self, PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(OO)",
                         Py_TYPE(self),
                         self->func,
                         self->cnt);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef tabulate_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)tabulate_reduce,                      /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Tabulate, x)
static PyMemberDef tabulate_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     tabulate_prop_func_doc                             /* doc */
     },

    {"current",                                         /* name */
     T_OBJECT,                                          /* type */
     OFF(cnt),                                          /* offset */
     READONLY,                                          /* flags */
     tabulate_prop_current_doc                          /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

static PyTypeObject PyIUType_Tabulate = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.tabulate",       /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Tabulate),            /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)tabulate_dealloc,                       /* tp_dealloc */
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
    (const char *)tabulate_doc,                         /* tp_doc */
    (traverseproc)tabulate_traverse,                    /* tp_traverse */
    (inquiry)tabulate_clear,                            /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)tabulate_next,                        /* tp_iternext */
    tabulate_methods,                                   /* tp_methods */
    tabulate_memberlist,                                /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)tabulate_new,                              /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
