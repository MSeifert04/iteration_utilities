/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

/******************************************************************************
 * Parts are taken from the CPython package (PSF licensed).
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *binop;
    PyObject *total;
    PyObject *funcargs;
} PyIUObject_Accumulate;

PyTypeObject PyIUType_Accumulate;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
accumulate_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "func", "start", NULL};
    PyIUObject_Accumulate *self;

    PyObject *iterable;
    PyObject *iterator = NULL;
    PyObject *binop = NULL;
    PyObject *start = NULL;
    PyObject *funcargs = NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:accumulate", kwlist,
                                     &iterable, &binop, &start)) {
        goto Fail;
    }
    PYIU_NULL_IF_NONE(binop);

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }

    if (binop != NULL) {
        funcargs = PyTuple_New(2);
        if (funcargs == NULL) {
            goto Fail;
        }
    }

    self = (PyIUObject_Accumulate *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(binop);
    Py_XINCREF(start);
    self->binop = binop;
    self->iterator = iterator;
    self->total = start;
    self->funcargs = funcargs;
    return (PyObject *)self;

Fail:
    Py_XDECREF(funcargs);
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
accumulate_dealloc(PyIUObject_Accumulate *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->binop);
    Py_XDECREF(self->total);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
accumulate_traverse(PyIUObject_Accumulate *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->binop);
    Py_VISIT(self->total);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
accumulate_clear(PyIUObject_Accumulate *self)
{
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->binop);
    Py_CLEAR(self->total);
    Py_CLEAR(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
accumulate_next(PyIUObject_Accumulate *self)
{
    PyObject *item, *oldtotal, *newtotal;

    /* Get next item from iterator.  */
    item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
    if (item == NULL) {
        return NULL;
    }

    /* If it's the first element the total is yet unset and we simply return
       the item.
       */
    if (self->total == NULL) {
        Py_INCREF(item);
        self->total = item;
        return self->total;
    }

    /* Apply the binop to the old total and the item defaulting to add if the
       binop is not set or set to None.
       */
    if (self->binop == NULL) {
        newtotal = PyNumber_Add(self->total, item);
    } else {
        PYIU_RECYCLE_ARG_TUPLE_BINOP(self->funcargs, self->total, item, Py_DECREF(item);
                                                                        return NULL);
        newtotal = PyObject_Call(self->binop, self->funcargs, NULL);
    }
    Py_DECREF(item);
    if (newtotal == NULL) {
        return NULL;
    }

    /* Update the total and return it. */
    oldtotal = self->total;
    self->total = newtotal;
    Py_DECREF(oldtotal);
    Py_INCREF(newtotal);
    return newtotal;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
accumulate_reduce(PyIUObject_Accumulate *self)
{
    /* Seperate cases depending on total == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a start or
       that the start was None.
       Better to make an "if" than to introduce another variable depending on
       total == NULL.
       */
    if (self->total != NULL) {
        return Py_BuildValue("O(OOO)",
                             Py_TYPE(self),
                             self->iterator,
                             self->binop ? self->binop : Py_None,
                             self->total);
    } else {
        return Py_BuildValue("O(OO)",
                             Py_TYPE(self),
                             self->iterator,
                             self->binop ? self->binop : Py_None);
    }
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
accumulate_lengthhint(PyIUObject_Accumulate *self)
{
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }
    return PyLong_FromSsize_t(len);
}
#endif

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef accumulate_methods[] = {

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__",                                 /* ml_name */
     (PyCFunction)accumulate_lengthhint,                /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_lenhint_doc                                   /* ml_doc */
     },
#endif

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)accumulate_reduce,                    /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Accumulate, x)
static PyMemberDef accumulate_memberlist[] = {

    {"func",                                            /* name */
     T_OBJECT,                                          /* type */
     OFF(binop),                                        /* offset */
     READONLY,                                          /* flags */
     accumulate_prop_func_doc                           /* doc */
     },

    {"current",                                         /* name */
     T_OBJECT_EX,                                       /* type */
     OFF(total),                                        /* offset */
     READONLY,                                          /* flags */
     accumulate_prop_current_doc                        /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Accumulate = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.accumulate",     /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Accumulate),          /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)accumulate_dealloc,                     /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)0,                                        /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash  */
    (ternaryfunc)0,                                     /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)accumulate_doc,                       /* tp_doc */
    (traverseproc)accumulate_traverse,                  /* tp_traverse */
    (inquiry)accumulate_clear,                          /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)accumulate_next,                      /* tp_iternext */
    accumulate_methods,                                 /* tp_methods */
    accumulate_memberlist,                              /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)0,                                       /* tp_alloc */
    (newfunc)accumulate_new,                            /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
