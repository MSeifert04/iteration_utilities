/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iteratortuple;
    Py_ssize_t numactive;
    Py_ssize_t active;
} PyIUObject_Roundrobin;

PyTypeObject PyIUType_Roundrobin;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
roundrobin_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    PyIUObject_Roundrobin *self;

    PyObject *iteratortuple = NULL;

    /* Create and fill struct */
    iteratortuple = PyIU_CreateIteratorTuple(args);
    if (iteratortuple == NULL) {
        goto Fail;
    }

    self = (PyIUObject_Roundrobin *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }

    self->iteratortuple = iteratortuple;
    self->numactive = PyTuple_GET_SIZE(args);
    self->active = 0;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iteratortuple);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
roundrobin_dealloc(PyIUObject_Roundrobin *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iteratortuple);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
roundrobin_traverse(PyIUObject_Roundrobin *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iteratortuple);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
roundrobin_next(PyIUObject_Roundrobin *self)
{
    PyObject *iterator, *item;

    /* Stop if there is no active iterator left. */
    if (self->numactive == 0) {
        return NULL;
    }

    iterator = PyTuple_GET_ITEM(self->iteratortuple, self->active);
    while ((item = (*Py_TYPE(iterator)->tp_iternext)(iterator)) == NULL) {
        if (PyErr_Occurred()) {
            if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                PyErr_Clear();
            } else {
                return NULL;
            }
        }
        /* If the last iterable in the iteratortuple is empty simply set it to
           NULL and reset the active pointer to 0.
            */
        if (self->active == self->numactive-1) {
            PyTuple_SET_ITEM(self->iteratortuple, self->active, NULL);
            self->active = 0;
        /* Otherwise move each item in the tuple (after the empty iterator) one
           to the left.
           */
        } else {
            PyIU_TupleRemove(self->iteratortuple, self->active, self->numactive);
        }
        self->numactive--;
        Py_DECREF(iterator);

        /* End the loop as soon as no active iterators are avaiable or use the
           next iterator.
           */
        if (self->numactive == 0) {
            break;
        } else {
            iterator = PyTuple_GET_ITEM(self->iteratortuple, self->active);
        }
    }

    if (self->numactive == 0) {
        return NULL;
    }

    /* Increment the active pointer (potentially wrapping it around to 0). */
    self->active = (self->active + 1) % (self->numactive);
    return item;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
roundrobin_reduce(PyIUObject_Roundrobin *self)
{
    PyObject *ittuple, *res;

    /* The "next" method modifies the "iteratortuple" so when someone uses
       "reduce" they would see how it changes. So we need to return a copy
       of that tuple here.
       An additional side-effect of "next" is that exhausted iterators are
       removed from the tuple, decremented and finally replaced by NULL at the
       end of the tuple. Python interpreters shouldn't see these NULLs
       (otherwise that might segfault) so we can simply "copy by slicing"
       when there are already exhausted iterators inside.
       */
    if (PyTuple_GET_SIZE(self->iteratortuple) != self->numactive) {
        ittuple = PyTuple_GetSlice(self->iteratortuple, 0, self->numactive);
    } else {
        ittuple = PyIU_TupleCopy(self->iteratortuple);
    }
    /* The error handling for both branches. */
    if (ittuple == NULL) {
        return NULL;
    }

    res = Py_BuildValue("OO(nn)", Py_TYPE(self),
                        ittuple,
                        self->numactive,
                        self->active);
    Py_DECREF(ittuple);
    return res;
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
roundrobin_setstate(PyIUObject_Roundrobin *self,
                    PyObject *state)
{
    Py_ssize_t numactive, active;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "nn:roundrobin.__setstate__",
                          &numactive, &active)) {
        return NULL;
    }

    /* active and numactive must be greater than zero, otherwise the "next"
       method could access out-of-bounds indices for the iteratortuple.
       */
    if (active < 0 || numactive < 0) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the first (%zd) and "
                     "second (%zd) argument in the `state` are not negative.",
                     Py_TYPE(self)->tp_name, numactive, active);
        return NULL;
    }
    /* If numactive is not zero than the active must be strictly smaller than
       numactive, otherwise the next "next" call would access an out of bounds
       index of the iteratortuple (or NULL). If "numactive" is zero then
       "active" must be zero as well (in this case it must not be greater but
       equal).
       */
    if (numactive != 0 && active >= numactive) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the first (%zd) "
                     "argument in the `state` is strictly greater than the "
                     "second (%zd) argument, if the first argument isn't zero.",
                     Py_TYPE(self)->tp_name, numactive, active);
        return NULL;
    } else if (numactive == 0 && active != 0) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the second (%zd) "
                     "argument in the `state` is zero if the first "
                     "argument (%zd) argument is zero.",
                     Py_TYPE(self)->tp_name, active, numactive);
        return NULL;
    }

    /* The "numactive" argument must match the number of not-NULL arguments
       in the iteratortuple. Luckily the NULLs are at the end of the
       "iteratortuple".
       */
    if (1) {
        Py_ssize_t tupsize = PyTuple_GET_SIZE(self->iteratortuple);
        /* decrement the tuple size as long as the last item is NULL. */
        while (tupsize > 0 &&
               PyTuple_GET_ITEM(self->iteratortuple, tupsize - 1) == NULL) {
            tupsize--;
        }
        if (numactive != tupsize) {
            PyErr_Format(PyExc_ValueError,
                         "`%.200s.__setstate__` expected that the first "
                         "argument in the `state` (%zd) is equal to the number "
                         "of not exhausted iterators (%zd) in the instance.",
                         Py_TYPE(self)->tp_name, numactive, tupsize);
            return NULL;
        }
    }

    self->numactive = numactive;
    self->active = active;
    Py_RETURN_NONE;
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
roundrobin_lengthhint(PyIUObject_Roundrobin *self)
{
    Py_ssize_t i, len = 0;

    for (i=0 ; i<self->numactive ; i++) {
        PyObject *it = PyTuple_GET_ITEM(self->iteratortuple, i);
        len += PyObject_LengthHint(it, 0);
    }

    return PyLong_FromSsize_t(len);
}
#endif

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef roundrobin_methods[] = {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__", (PyCFunction)roundrobin_lengthhint, METH_NOARGS, PYIU_lenhint_doc},
#endif
    {"__reduce__",   (PyCFunction)roundrobin_reduce,   METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)roundrobin_setstate, METH_O,      PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(roundrobin_doc, "roundrobin(*iterables)\n\
--\n\
\n\
Round-Robin implementation ([0]_).\n\
\n\
Parameters\n\
----------\n\
iterables : iterable\n\
    `Iterables` to combine using the round-robin. Any amount of iterables\n\
    are supported.\n\
\n\
Returns\n\
-------\n\
roundrobin : generator\n\
    Iterable filled with the values of the `iterables`.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import roundrobin\n\
>>> list(roundrobin('ABC', 'D', 'EF'))\n\
['A', 'D', 'E', 'B', 'F', 'C']\n\
\n\
References\n\
----------\n\
.. [0] https://en.wikipedia.org/wiki/Round-robin_scheduling");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Roundrobin = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.roundrobin",                   /* tp_name */
    sizeof(PyIUObject_Roundrobin),                      /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)roundrobin_dealloc,                     /* tp_dealloc */
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
    roundrobin_doc,                                     /* tp_doc */
    (traverseproc)roundrobin_traverse,                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)roundrobin_next,                      /* tp_iternext */
    roundrobin_methods,                                 /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    roundrobin_new,                                     /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};