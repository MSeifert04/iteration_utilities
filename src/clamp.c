/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
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
    if (low == Py_None) {
        low = NULL;
    }
    if (high == Py_None) {
        high = NULL;
    }

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
    }
    return PyLong_FromSsize_t(len);
}
#endif

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef clamp_methods[] = {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__", (PyCFunction)clamp_lengthhint, METH_NOARGS, PYIU_lenhint_doc},
#endif
    {"__reduce__", (PyCFunction)clamp_reduce, METH_NOARGS, PYIU_reduce_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(clamp_doc, "clamp(iterable, low=None, high=None, inclusive=False, remove=True)\n\
--\n\
\n\
Remove values which are not between `low` and `high`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Clamp the values from this `iterable`.\n\
\n\
low : any type, optional\n\
    The lower bound for clamp. If not given or ``None`` there is no lower \n\
    bound.\n\
\n\
high : any type, optional\n\
    The upper bound for clamp. If not given or ``None`` there is no upper \n\
    bound.\n\
\n\
inclusive : bool, optional\n\
    If ``True`` also remove values that are equal to `low` and `high`.\n\
    Default is ``False``.\n\
\n\
remove : bool, optional\n\
    If ``True`` remove the items outside the range given by ``low`` and\n\
    ``high``, otherwise replace them with ``low`` if they are lower or\n\
    ``high`` if they are higher.\n\
    Default is ``True``.\n\
\n\
    .. versionadded:: 0.2\n\
\n\
Returns\n\
-------\n\
clamped : generator\n\
    A generator containing the values of `iterable` which are between `low`\n\
    and `high`.\n\
\n\
Examples\n\
--------\n\
This function is equivalent to a generator expression like:\n\
``(item for item in iterable if low <= item <= high)`` or\n\
``(item for item in iterable if low < item < high)`` if `inclusive=True`.\n\
Or a similar `filter`: ``filter(lambda item: low <= item <= high, iterable)``\n\
But it also allows for either ``low`` or ``high`` to be ignored and is faster.\n\
Some simple examples::\n\
\n\
    >>> from iteration_utilities import clamp\n\
    >>> list(clamp(range(5), low=2))\n\
    [2, 3, 4]\n\
    >>> list(clamp(range(5), high=2))\n\
    [0, 1, 2]\n\
    >>> list(clamp(range(1000), low=2, high=8, inclusive=True))\n\
    [3, 4, 5, 6, 7]\n\
\n\
If ``remove=False`` the function will replace values instead::\n\
\n\
    >>> list(clamp(range(10), low=4, high=8, remove=False))\n\
    [4, 4, 4, 4, 4, 5, 6, 7, 8, 8]\n\
");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Clamp = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.clamp",                        /* tp_name */
    sizeof(PyIUObject_Clamp),                           /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)clamp_dealloc,                          /* tp_dealloc */
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
    clamp_doc,                                          /* tp_doc */
    (traverseproc)clamp_traverse,                       /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)clamp_next,                           /* tp_iternext */
    clamp_methods,                                      /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    clamp_new,                                          /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
