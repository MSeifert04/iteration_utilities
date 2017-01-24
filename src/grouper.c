/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
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

    PyObject *iterable, *iterator, *fillvalue = NULL, *result = NULL;
    Py_ssize_t times;
    int truncate = 0;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "On|Oi:grouper", kwlist,
                                     &iterable, &times,
                                     &fillvalue, &truncate)) {
        return NULL;
    }
    if (fillvalue != NULL && truncate != 0) {
        PyErr_Format(PyExc_TypeError,
                     "cannot specify both `truncate` and `fillvalue`.");
        return NULL;
    }
    if (times <= 0) {
        PyErr_Format(PyExc_ValueError, "`n` must be greater than 0.");
        return NULL;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    self = (PyIUObject_Grouper *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }
    Py_XINCREF(fillvalue);
    self->iterator = iterator;
    self->times = times;
    self->fillvalue = fillvalue;
    self->truncate = truncate;
    self->result = result;
    return (PyObject *)self;
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
 * Next
 *****************************************************************************/

static PyObject *
grouper_next(PyIUObject_Grouper *self)
{
    PyObject *result = self->result;
    PyObject *(*iternext)(PyObject *);

    PyObject *newresult, *lastresult, *item, *olditem;
    Py_ssize_t idx1, idx2;
    int recycle;

    /* First call needs to create a tuple for the result. */
    if (result == NULL) {
        result = PyTuple_New(self->times);
        self->result = result;
    }

    iternext = *Py_TYPE(self->iterator)->tp_iternext;

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
        item = iternext(self->iterator);

        if (item == NULL) {
            PYIU_CLEAR_STOPITERATION;
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
    if (self->fillvalue == NULL) {
        return Py_BuildValue("O(On)(Oi)", Py_TYPE(self),
                             self->iterator,
                             self->times,
                             self->result ? self->result : Py_None,
                             self->truncate);
    } else {
        return Py_BuildValue("O(OnO)(Oi)", Py_TYPE(self),
                             self->iterator,
                             self->times,
                             self->fillvalue,
                             self->result ? self->result : Py_None,
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
    PyObject *result;
    int truncate;

    if (!PyArg_ParseTuple(state, "Oi", &result, &truncate)) {
        return NULL;
    }
    if (result == Py_None) {
        result = NULL;
    }

    Py_CLEAR(self->result);
    Py_XINCREF(result);
    self->result = result;
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
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    Py_ssize_t groups = len / self->times;
    Py_ssize_t rem = len % self->times;
    if (self->truncate || rem == 0) {
        return PyLong_FromSsize_t(groups);
    } else {
        return PyLong_FromSsize_t(groups + 1);
    }
}
#endif

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef grouper_methods[] = {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__", (PyCFunction)grouper_lengthhint, METH_NOARGS, PYIU_lenhint_doc},
#endif
    {"__reduce__", (PyCFunction)grouper_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)grouper_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(grouper_doc, "grouper(iterable, n, fillvalue=None, truncate=False)\n\
--\n\
\n\
Collect data into fixed-length chunks or blocks.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Any `iterable` to group.\n\
\n\
n : :py:class:`int`\n\
    The number of elements in each chunk.\n\
\n\
fillvalue : any type, optional\n\
    The `fillvalue` if the `iterable` is consumed and the last yielded group\n\
    should be filled. If not given the last yielded group may be shorter\n\
    than the group before.\n\
\n\
truncate : bool, optional\n\
    As alternative to `fillvalue` the last group is discarded if it is\n\
    shorter than `n` and `truncate` is ``True``.\n\
    Default is ``False``.\n\
\n\
Raises\n\
------\n\
TypeError\n\
    If `truncate` is ``True`` and a `fillvalue` is given.\n\
\n\
Returns\n\
-------\n\
groups : generator\n\
    An `iterable` containing the groups/chunks as ``tuple``.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import grouper\n\
\n\
>>> list(grouper('ABCDEFG', 3))\n\
[('A', 'B', 'C'), ('D', 'E', 'F'), ('G',)]\n\
\n\
>>> list(grouper('ABCDEFG', 3, fillvalue='x'))\n\
[('A', 'B', 'C'), ('D', 'E', 'F'), ('G', 'x', 'x')]\n\
\n\
>>> list(grouper('ABCDEFG', 3, truncate=True))\n\
[('A', 'B', 'C'), ('D', 'E', 'F')]");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Grouper = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.grouper",                      /* tp_name */
    sizeof(PyIUObject_Grouper),                         /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)grouper_dealloc,                        /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    0,                                                  /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    0,                                                  /* tp_call */
    0,                                                  /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    grouper_doc,                                        /* tp_doc */
    (traverseproc)grouper_traverse,                     /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)grouper_next,                         /* tp_iternext */
    grouper_methods,                                    /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    grouper_new,                                        /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
