/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *value;
} PyIUObject_Applyfunc;

static PyTypeObject PyIUType_Applyfunc;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
applyfunc_new(PyTypeObject *type,
              PyObject *args,
              PyObject *kwargs)
{
    static char *kwlist[] = {"func", "initial", NULL};
    PyIUObject_Applyfunc *self;

    PyObject *func, *initial;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO:applyfunc", kwlist,
                                     &func, &initial)) {
        goto Fail;
    }

    /* Create and fill struct */
    self = (PyIUObject_Applyfunc *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_INCREF(func);
    Py_INCREF(initial);
    self->func = func;
    self->value = initial;

    return (PyObject *)self;

Fail:
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
applyfunc_dealloc(PyIUObject_Applyfunc *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->value);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
applyfunc_traverse(PyIUObject_Applyfunc *self,
                   visitproc visit,
                   void *arg)
{
    Py_VISIT(self->func);
    Py_VISIT(self->value);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
applyfunc_clear(PyIUObject_Applyfunc *self)
{
    Py_CLEAR(self->func);
    Py_CLEAR(self->value);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
applyfunc_next(PyIUObject_Applyfunc *self)
{
    PyObject *newval, *temp;

    /* Call the function with the current value as argument.  */
    newval = PyIU_CallWithOneArgument(self->func, self->value);
    if (newval == NULL) {
        return NULL;
    }

    /* Save the new value and also return it. */
    temp = self->value;
    self->value = newval;
    Py_DECREF(temp);

    Py_INCREF(newval);
    return newval;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
applyfunc_reduce(PyIUObject_Applyfunc *self, PyObject *Py_UNUSED(args))
{
    return Py_BuildValue("O(OO)",
                         Py_TYPE(self),
                         self->func,
                         self->value);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef applyfunc_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)applyfunc_reduce,                     /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Applyfunc, x)
static PyMemberDef applyfunc_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     applyfunc_prop_func_doc                            /* doc */
     },

    {"current",                                         /* name */
     T_OBJECT,                                          /* type */
     OFF(value),                                        /* offset */
     READONLY,                                          /* flags */
     applyfunc_prop_current_doc                         /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

static PyTypeObject PyIUType_Applyfunc = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.applyfunc",      /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Applyfunc),           /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)applyfunc_dealloc,                      /* tp_dealloc */
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
    (const char *)applyfunc_doc,                        /* tp_doc */
    (traverseproc)applyfunc_traverse,                   /* tp_traverse */
    (inquiry)applyfunc_clear,                           /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)applyfunc_next,                       /* tp_iternext */
    applyfunc_methods,                                  /* tp_methods */
    applyfunc_memberlist,                               /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)applyfunc_new,                             /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
