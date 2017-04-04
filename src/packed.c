/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *func;
} PyIUObject_Packed;

PyTypeObject PyIUType_Packed;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
packed_new(PyTypeObject *type,
           PyObject *args,
           PyObject *kwargs)
{
    PyIUObject_Packed *self;

    PyObject *func;

    /* Parse arguments */
    if (!PyArg_UnpackTuple(args, "packed", 1, 1, &func)) {
        return NULL;
    }

    /* Create struct */
    self = (PyIUObject_Packed *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
packed_dealloc(PyIUObject_Packed *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->func);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
packed_traverse(PyIUObject_Packed *self,
                visitproc visit,
                void *arg)
{
    Py_VISIT(self->func);
    return 0;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
packed_call(PyIUObject_Packed *self,
            PyObject *args,
            PyObject *kwargs)
{
    PyObject *packed, *newpacked, *res;
    if (!PyArg_UnpackTuple(args, "packed.__call__", 1, 1, &packed)) {
        return NULL;
    }

    Py_INCREF(packed);

    if (!PyTuple_CheckExact(packed)) {
        newpacked = PySequence_Tuple(packed);
        Py_DECREF(packed);
        if (newpacked == NULL) {
            return NULL;
        }
        packed = newpacked;
    }

    res = PyObject_Call(self->func, packed, kwargs);
    Py_DECREF(packed);
    return res;
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
packed_repr(PyIUObject_Packed *self)
{
    PyObject *result = NULL;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    result = PyUnicode_FromFormat("%s(%R)",
                                  Py_TYPE(self)->tp_name,
                                  self->func);

    Py_ReprLeave((PyObject *)self);
    return result;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
packed_reduce(PyIUObject_Packed *self,
              PyObject *unused)
{
    return Py_BuildValue("O(O)", Py_TYPE(self), self->func);
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef packed_methods[] = {

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)packed_reduce,                        /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Packed, x)
static PyMemberDef packed_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(func),                                         /* offset */
     READONLY,                                          /* flags */
     packed_prop_func_doc                               /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Packed = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.packed",         /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Packed),              /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)packed_dealloc,                         /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)packed_repr,                              /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)packed_call,                           /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)packed_doc,                           /* tp_doc */
    (traverseproc)packed_traverse,                      /* tp_traverse */
    (inquiry)0,                                         /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)0,                                     /* tp_iter */
    (iternextfunc)0,                                    /* tp_iternext */
    packed_methods,                                     /* tp_methods */
    packed_memberlist,                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)packed_new,                                /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
