/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
    PyObject *except;
    PyObject *first;
} PyIUObject_Iterexcept;

PyTypeObject PyIUType_Iterexcept;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
iterexcept_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"func", "exception", "first", NULL};
    PyIUObject_Iterexcept *self;

    PyObject *func, *except, *first=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O:iter_except", kwlist,
                                     &func, &except, &first)) {
        return NULL;
    }
    PYIU_NULL_IF_NONE(first);

    /* Create and fill struct */
    self = (PyIUObject_Iterexcept *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    Py_INCREF(except);
    Py_XINCREF(first);
    self->func = func;
    self->except = except;
    self->first = first;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
iterexcept_dealloc(PyIUObject_Iterexcept *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_XDECREF(self->except);
    Py_XDECREF(self->first);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
iterexcept_traverse(PyIUObject_Iterexcept *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->func);
    Py_VISIT(self->except);
    Py_VISIT(self->first);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
iterexcept_next(PyIUObject_Iterexcept *self)
{
    PyObject *result;

    /* Call the first if it's set (nulling it thereafter) or the func if not. */
    if (self->first == NULL) {
        result = PyObject_CallObject(self->func, NULL);
    } else {
        result = PyObject_CallObject(self->first, NULL);
        Py_CLEAR(self->first);
    }

    /* Stop if the result is NULL but only clear the exception if the expected
       exception happened otherwise just return the result (thereby bubbling
       up other exceptions).
       */
    if (result == NULL && PyErr_Occurred() && PyErr_ExceptionMatches(self->except)) {
        PyErr_Clear();
        return NULL;
    } else {
        return result;
    }
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
iterexcept_reduce(PyIUObject_Iterexcept *self)
{
    return Py_BuildValue("O(OOO)", Py_TYPE(self),
                         self->func,
                         self->except,
                         self->first ? self->first : Py_None);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef iterexcept_methods[] = {

    {"__reduce__",                                     /* ml_name */
     (PyCFunction)iterexcept_reduce,                   /* ml_meth */
     METH_NOARGS,                                      /* ml_flags */
     PYIU_reduce_doc                                   /* ml_doc */
     },

    {NULL, NULL}                                       /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Iterexcept, x)
static PyMemberDef iterexcept_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     iterexcept_prop_func_doc                           /* doc */
     },

    {"exception",                                       /* name */
     T_OBJECT,                                          /* type */
     OFF(except),                                       /* offset */
     READONLY,                                          /* flags */
    iterexcept_prop_exception_doc                       /* doc */
     },

    {"first",                                           /* name */
     T_OBJECT_EX,                                       /* type */
     OFF(first),                                        /* offset */
     READONLY,                                          /* flags */
     iterexcept_prop_first_doc                          /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Iterexcept = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.iter_except",    /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Iterexcept),          /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)iterexcept_dealloc,                     /* tp_dealloc */
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
    (const char *)iterexcept_doc,                       /* tp_doc */
    (traverseproc)iterexcept_traverse,                  /* tp_traverse */
    (inquiry)0,                                         /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)iterexcept_next,                      /* tp_iternext */
    iterexcept_methods,                                 /* tp_methods */
    iterexcept_memberlist,                              /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,                     /* tp_alloc */
    (newfunc)iterexcept_new,                            /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
