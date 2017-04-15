/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *fillvalue;
    PyObject *result;
    Py_ssize_t times;
    int truncate;
} PyIUObject_Grouper;

PyTypeObject PyIUType_Grouper;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
grouper_new(PyTypeObject *type,
            PyObject *args,
            PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "n", "fillvalue", "truncate", NULL};
    PyIUObject_Grouper *self;

    PyObject *iterable;
    PyObject *iterator = NULL;
    PyObject *fillvalue = NULL;
    PyObject *result = NULL;
    Py_ssize_t times;
    int truncate = 0;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "On|Oi:grouper", kwlist,
                                     &iterable, &times,
                                     &fillvalue, &truncate)) {
        goto Fail;
    }
    if (fillvalue != NULL && truncate != 0) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot specify both the `truncate` and the "
                        "`fillvalue` argument for `grouper`.");
        goto Fail;
    }
    if (times <= 0) {
        PyErr_SetString(PyExc_ValueError,
                        "`n` argument for `grouper` must be greater than 0.");
        goto Fail;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    self = (PyIUObject_Grouper *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(fillvalue);
    self->iterator = iterator;
    self->times = times;
    self->fillvalue = fillvalue;
    self->truncate = truncate;
    self->result = result;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
grouper_dealloc(PyIUObject_Grouper *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->fillvalue);
    Py_XDECREF(self->result);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
grouper_traverse(PyIUObject_Grouper *self,
                 visitproc visit,
                 void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->fillvalue);
    Py_VISIT(self->result);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
grouper_clear(PyIUObject_Grouper *self)
{
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->fillvalue);
    Py_CLEAR(self->result);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
grouper_next(PyIUObject_Grouper *self)
{
    PyObject *result = self->result;

    PyObject *newresult, *lastresult, *item, *olditem;
    Py_ssize_t idx1, idx2;
    int recycle;

    /* First call needs to create a tuple for the result. */
    if (result == NULL) {
        result = PyTuple_New(self->times);
        self->result = result;
    }

    /* Recycle old result if the instance is the only one holding a reference,
       otherwise create a new tuple.
       */
    recycle = (Py_REFCNT(result) == 1);
    if (recycle) {
        newresult = result;
    } else {
        newresult = PyTuple_New(self->times);
        if (newresult == NULL) {
            return NULL;
        }
    }

    /* Take the next self->times elements from the iterator.  */
    for (idx1=0 ; idx1<self->times ; idx1++) {
        item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);

        if (item == NULL) {
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                    PyErr_Clear();
                } else {
                    return NULL;
                }
            }
            /* In case it would be the first element of a new tuple or we
               truncate the iterator we stop here.
               */
            if (idx1 == 0 || self->truncate != 0) {
                Py_DECREF(newresult);
                return NULL;

            /* If we want to fill the last group just proceed but use the
               fillvalue as item.
               */
            } else if (self->fillvalue != NULL) {
                Py_INCREF(self->fillvalue);
                item = self->fillvalue;

            /* Otherwise we need a return just the last idx1 items. Because
               idx1 is by definition smaller than self->times we need a new
               tuple to hold the result.
               */
            } else {
                lastresult = PyTuple_New(idx1);
                if (lastresult == NULL) {
                    Py_DECREF(newresult);
                    return NULL;
                }
                /* Fill in already found values. The Incref them is save
                   because the old references will be destroyed when the old
                   tuple is destroyed.
                   -> Maybe use _PyTuple_Resize but the warning in the docs
                   that one shouldn't assume that the tuple is the same made
                   me hesitate.
                   */
                for (idx2=0 ; idx2<idx1 ; idx2++) {
                    olditem = PyTuple_GET_ITEM(newresult, idx2);
                    Py_INCREF(olditem);
                    PyTuple_SET_ITEM(lastresult, idx2, olditem);
                }
                Py_DECREF(newresult);
                return lastresult;
            }
        }

        /* If we recycle we need to decref the old results before replacing
           them.
           */
        if (recycle) {
            olditem = PyTuple_GET_ITEM(newresult, idx1);
            PyTuple_SET_ITEM(newresult, idx1, item);
            /* May be insecure because deleting elements might have
               consequences for the sequence. A better way would be to keep
               all of them until the tuple elements are replaced and then to
               delete them.
               */
            Py_XDECREF(olditem);
        } else {
            PyTuple_SET_ITEM(newresult, idx1, item);
        }
    }
    if (recycle) {
        Py_INCREF(newresult);
    }
    return newresult;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
grouper_reduce(PyIUObject_Grouper *self)
{
    /* Seperate cases depending on fillvalue == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a fillvalue or
       that the next item was None.
       Better to make an "if" than to introduce another variable depending on
       fillvalue == NULL.
       */
    if (self->fillvalue == NULL) {
        return Py_BuildValue("O(On)(i)", Py_TYPE(self),
                             self->iterator,
                             self->times,
                             self->truncate);
    } else {
        return Py_BuildValue("O(OnO)(i)", Py_TYPE(self),
                             self->iterator,
                             self->times,
                             self->fillvalue,
                             self->truncate);
    }
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
grouper_setstate(PyIUObject_Grouper *self,
                 PyObject *state)
{
    int truncate;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "i:grouper.__setstate__", &truncate)) {
        return NULL;
    }

    /* truncate is just a boolean-like flag so there isn't anything that could
       checked here.
       */

    self->truncate = truncate;
    Py_RETURN_NONE;
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
grouper_lengthhint(PyIUObject_Grouper *self)
{
    Py_ssize_t groups, rem;
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }
    groups = len / self->times;
    rem = len % self->times;
    if (self->truncate || rem == 0) {
        return PyLong_FromSsize_t(groups);
    } else {
        /* groups + 1 cannot overflow because that could only happen if
           "times" is 1 and in that case "rem==0". So it would always enter the
           first branch which does not contain addition.
           */
        return PyLong_FromSsize_t(groups + 1);
    }
}
#endif

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef grouper_methods[] = {

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__",                                 /* ml_name */
     (PyCFunction)grouper_lengthhint,                   /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_lenhint_doc                                   /* ml_doc */
     },
#endif

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)grouper_reduce,                       /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {"__setstate__",                                    /* ml_name */
     (PyCFunction)grouper_setstate,                     /* ml_meth */
     METH_O,                                            /* ml_flags */
     PYIU_setstate_doc                                  /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Grouper, x)
static PyMemberDef grouper_memberlist[] = {

    {"fillvalue",                                       /* name */
     T_OBJECT_EX,                                       /* type */
     OFF(fillvalue),                                    /* offset */
     READONLY,                                          /* flags */
     grouper_prop_fillvalue_doc                         /* doc */
     },

    {"times",                                           /* name */
     T_PYSSIZET,                                        /* type */
     OFF(times),                                        /* offset */
     READONLY,                                          /* flags */
     grouper_prop_times_doc                             /* doc */
     },

    {"truncate",                                        /* name */
     T_BOOL,                                            /* type */
     OFF(truncate),                                     /* offset */
     READONLY,                                          /* flags */
     grouper_prop_truncate_doc                          /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Grouper = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.grouper",        /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Grouper),             /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)grouper_dealloc,                        /* tp_dealloc */
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
    (const char *)grouper_doc,                          /* tp_doc */
    (traverseproc)grouper_traverse,                     /* tp_traverse */
    (inquiry)grouper_clear,                             /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)grouper_next,                         /* tp_iternext */
    grouper_methods,                                    /* tp_methods */
    grouper_memberlist,                                 /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,                     /* tp_alloc */
    (newfunc)grouper_new,                               /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
