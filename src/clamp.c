/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *low;
    PyObject *high;
    int inclusive;
    int remove;
} PyIUObject_Clamp;

PyTypeObject PyIUType_Clamp;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
clamp_new(PyTypeObject *type,
          PyObject *args,
          PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "low", "high", "inclusive", "remove", NULL};
    PyIUObject_Clamp *self;

    PyObject *iterable;
    PyObject *iterator=NULL;
    PyObject *low=NULL;
    PyObject *high=NULL;
    int inclusive=0, remove=1;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OOii:clamp", kwlist,
                                     &iterable, &low, &high, &inclusive, &remove)) {
        goto Fail;
    }
    /* None cannot be compared so it's unlikely we exclude use-cases by
       allowing low=None as equivalent to not giving any "low" argument.
       */
    PYIU_NULL_IF_NONE(low);
    PYIU_NULL_IF_NONE(high);

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    self = (PyIUObject_Clamp *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(low);
    Py_XINCREF(high);
    self->iterator = iterator;
    self->low = low;
    self->high = high;
    self->inclusive = inclusive;
    self->remove = remove;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
clamp_dealloc(PyIUObject_Clamp *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->low);
    Py_XDECREF(self->high);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
clamp_traverse(PyIUObject_Clamp *self,
               visitproc visit,
               void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->low);
    Py_VISIT(self->high);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
clamp_clear(PyIUObject_Clamp *self)
{
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->low);
    Py_CLEAR(self->high);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
clamp_next(PyIUObject_Clamp *self)
{
    PyObject *item;
    int res;

    while ( (item = (*Py_TYPE(self->iterator)->tp_iternext)(self->iterator)) ) {
        /* Check if it's smaller than the lower bound. */
        if (self->low != NULL) {
            res = PyObject_RichCompareBool(item, self->low,
                                           self->inclusive ? Py_LE : Py_LT);
            if (res == 1) {
                Py_DECREF(item);
                if (!(self->remove)) {
                    Py_INCREF(self->low);
                    return self->low;
                }
                continue;
            } else if (res == -1) {
                Py_DECREF(item);
                return NULL;
            }
        }
        /* Check if it's bigger than the upper bound. */
        if (self->high != NULL) {
            res = PyObject_RichCompareBool(item, self->high,
                                           self->inclusive ? Py_GE : Py_GT);
            if (res == 1) {
                Py_DECREF(item);
                if (!(self->remove)) {
                    Py_INCREF(self->high);
                    return self->high;
                }
                continue;
            } else if (res == -1) {
                Py_DECREF(item);
                return NULL;
            }
        }
        /* Still here? Return the item! */
        return item;
    }
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
clamp_reduce(PyIUObject_Clamp *self)
{
    return Py_BuildValue("O(OOOii)", Py_TYPE(self),
                         self->iterator,
                         self->low ? self->low : Py_None,
                         self->high ? self->high : Py_None,
                         self->inclusive,
                         self->remove);
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
clamp_lengthhint(PyIUObject_Clamp *self)
{
    Py_ssize_t len = 0;
    /* If we don't remove outliers or there are no bounds at all we can
       determine the length. */
    if (!(self->remove) || (self->low == NULL && self->high == NULL)) {
        len = PyObject_LengthHint(self->iterator, 0);
        if (len == -1) {
            return NULL;
        }
    }
    return PyLong_FromSsize_t(len);
}
#endif

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef clamp_methods[] = {

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__",                                 /* ml_name */
     (PyCFunction)clamp_lengthhint,                     /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_lenhint_doc                                   /* ml_doc */
     },
#endif

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)clamp_reduce,                         /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Clamp, x)
static PyMemberDef clamp_memberlist[] = {

    {"low",                                             /* name */
     T_OBJECT,                                          /* type */
     OFF(low),                                          /* offset */
     READONLY,                                          /* flags */
     clamp_prop_low_doc                                 /* doc */
     },

    {"high",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(high),                                         /* offset */
     READONLY,                                          /* flags */
     clamp_prop_high_doc                                /* doc */
     },

    {"inclusive",                                       /* name */
     T_BOOL,                                            /* type */
     OFF(inclusive),                                    /* offset */
     READONLY,                                          /* flags */
     clamp_prop_inclusive_doc                           /* doc */
     },

    {"remove",                                          /* name */
     T_BOOL,                                            /* type */                                       /* type */
     OFF(remove),                                       /* offset */
     READONLY,                                          /* flags */
     clamp_prop_remove_doc                              /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Clamp = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.clamp",          /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Clamp),               /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)clamp_dealloc,                          /* tp_dealloc */
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
    (const char *)clamp_doc,                            /* tp_doc */
    (traverseproc)clamp_traverse,                       /* tp_traverse */
    (inquiry)clamp_clear,                               /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)clamp_next,                           /* tp_iternext */
    clamp_methods,                                      /* tp_methods */
    clamp_memberlist,                                   /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,                     /* tp_alloc */
    (newfunc)clamp_new,                                 /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
