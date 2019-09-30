/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

static PyObject PlaceholderStruct;

#define PYIU_Placeholder (&PlaceholderStruct)

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
placeholder_new(PyTypeObject *type,
                PyObject *args,
                PyObject *kwargs)
{
    if (PyTuple_GET_SIZE(args) || (kwargs != NULL && PyDict_Size(kwargs))) {
        PyErr_SetString(PyExc_TypeError, "`PlaceholderType` takes no arguments.");
        return NULL;
    }
    Py_INCREF(PYIU_Placeholder);
    return PYIU_Placeholder;
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
placeholder_repr(PyObject *self)
{
    return PyUnicode_FromString("_");
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

#if PY_MAJOR_VERSION == 3
static PyObject *
placeholder_reduce(PyObject *self, PyObject *Py_UNUSED(args))
{
    return PyUnicode_FromString("iteration_utilities.Placeholder");
}
#endif

/******************************************************************************
 * Type
 *****************************************************************************/

#if PY_MAJOR_VERSION == 3
static PyMethodDef placeholder_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)placeholder_reduce,                   /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};
#else
static PyMethodDef placeholder_methods[] = {

    {NULL, NULL}                                        /* sentinel */
};
#endif

static PyTypeObject Placeholder_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.PlaceholderType",/* tp_name */
    (Py_ssize_t)0,                                      /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)0,                                      /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)placeholder_repr,                         /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)0,                                     /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)0,                                    /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                                 /* tp_flags */
    (const char *)placeholder_doc,                      /* tp_doc */
    (traverseproc)0,                                    /* tp_traverse */
    (inquiry)0,                                         /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    placeholder_methods,                                /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)placeholder_new,                           /* tp_new */
};

static PyObject PlaceholderStruct = {
    _PyObject_EXTRA_INIT
    1, &Placeholder_Type
};
