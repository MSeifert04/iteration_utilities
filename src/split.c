/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *delimiter;
    Py_ssize_t maxsplit;
    int keep_delimiter;
    int cmp;
    PyObject *next;
    PyObject *funcargs;
} PyIUObject_Split;

static PyTypeObject PyIUType_Split;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
split_new(PyTypeObject *type,
          PyObject *args,
          PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", "maxsplit", "keep", "eq", NULL};
    PyIUObject_Split *self;

    PyObject *iterable, *iterator, *delimiter, *funcargs=NULL;
    Py_ssize_t maxsplit = -1;  /* -1 means no maxsplit! */
    int keep_delimiter = 0, cmp = 0;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|nii:split", kwlist,
                                     &iterable, &delimiter,
                                     &maxsplit, &keep_delimiter, &cmp)) {
        return NULL;
    }
    if (maxsplit <= -2) {
        PyErr_Format(PyExc_ValueError,
                     "`maxsplit` must be -1 or greater.");
        return NULL;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }
    self = (PyIUObject_Split *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(iterator);
        Py_DECREF(funcargs);
        return NULL;
    }
    Py_INCREF(delimiter);
    self->iterator = iterator;
    self->delimiter = delimiter;
    self->maxsplit = maxsplit;
    self->keep_delimiter = keep_delimiter;
    self->cmp = cmp;
    self->next = NULL;
    self->funcargs = funcargs;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
split_dealloc(PyIUObject_Split *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->delimiter);
    Py_XDECREF(self->next);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
split_traverse(PyIUObject_Split *self,
               visitproc visit,
               void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->delimiter);
    Py_VISIT(self->next);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
split_next(PyIUObject_Split *self)
{
    PyObject *result, *item=NULL, *val=NULL, *tmp=NULL;
    int ok;

    /* Create a list to hold the result. */
    result = PyList_New(0);
    if (result == NULL) {
        goto Fail;
    }

    /* If there was already a value saved as next just append it and return it.
       This case happenes if someone wants to keep the delimiter.
       */
    if (self->next != NULL) {
        ok = PyList_Append(result, self->next);
        Py_DECREF(self->next);
        self->next = NULL;
        if (ok == 0) {
            return result;
        } else {
            goto Fail;
        }
    }

    while ( (item = (*Py_TYPE(self->iterator)->tp_iternext)(self->iterator)) ) {
        /* Compare the value to the delimiter or call the delimiter function on
           it to determine if we should split here.
           */
        if (self->cmp) {
            ok = PyObject_RichCompareBool(self->delimiter, item, Py_EQ);

        } else {
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, tmp, goto Fail)
            val = PyObject_Call(self->delimiter, self->funcargs, NULL);
            if (val == NULL) {
                goto Fail;
            }
            ok = PyObject_IsTrue(val);
        }

        /* Value is not delimiter or we already used up the maxsplit
           splittings.
           */
        if (ok == 0 || self->maxsplit == 0) {
            ok = PyList_Append(result, item);
            if (ok != 0) {
                goto Fail;
            }
            Py_DECREF(item);
            Py_XDECREF(val);

        /* Split here. */
        } else if (ok == 1) {
            Py_XDECREF(val);
            /* Decrement maxsplit */
            if (self->maxsplit != -1) {
                self->maxsplit--;
            }
            /* Keep the delimiter (if requested) as next item. */
            if (self->keep_delimiter) {
                self->next = item;
            } else {
                Py_DECREF(item);
            }
            return result;

        } else {
            goto Fail;
        }
    }

    PYIU_CLEAR_STOPITERATION;

    /* Only return the last result if there is something in it. */
    if (PyList_GET_SIZE(result) == 0) {
        Py_DECREF(result);
        return NULL;
    } else {
        return result;
    }


Fail:
    Py_XDECREF(result);
    Py_XDECREF(item);
    Py_XDECREF(val);
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
split_reduce(PyIUObject_Split *self)
{
    if (self->next == NULL) {
        return Py_BuildValue("O(OOnii)", Py_TYPE(self),
                             self->iterator,
                             self->delimiter,
                             self->maxsplit,
                             self->keep_delimiter,
                             self->cmp);
    } else {
        return Py_BuildValue("O(OOnii)(O)", Py_TYPE(self),
                             self->iterator,
                             self->delimiter,
                             self->maxsplit,
                             self->keep_delimiter,
                             self->cmp,
                             self->next);
    }
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
split_setstate(PyIUObject_Split *self,
               PyObject *state)
{
    PyObject *next;

    if (!PyArg_ParseTuple(state, "O", &next)) {
        return NULL;
    }

    Py_CLEAR(self->next);
    Py_INCREF(next);
    self->next = next;
    Py_RETURN_NONE;
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef split_methods[] = {
    {"__reduce__", (PyCFunction)split_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)split_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(split_doc, "split(iterable, key, maxsplit=-1, keep=False, eq=False)\n\
--\n\
\n\
Splits an `iterable` by a `key`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    The `iterable` to split.\n\
\n\
key : callable\n\
    The function by which to split the `iterable` (split where\n\
    ``key(item) == True``).\n\
\n\
maxsplit : int, optional\n\
    The number of maximal splits. If ``maxsplit=-1`` then there is no limit.\n\
    Default is ``-1``.\n\
\n\
keep : bool\n\
    If ``True`` also include the items where ``key(item)=True``.\n\
    Default is ``False``.\n\
\n\
eq : bool\n\
    If ``True`` split the `iterable` where ``key == item`` instead of\n\
    ``key(item) == True``. This can significantly speedup the function is a\n\
    simple delimiter is used.\n\
    Default is ``False``.\n\
\n\
Returns\n\
-------\n\
splitted_iterable : generator\n\
    Generator containing the split `iterable` as lists.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import split\n\
>>> list(split(range(1, 10), lambda x: x%3==0))\n\
[[1, 2], [4, 5], [7, 8]]\n\
\n\
>>> list(split(range(1, 10), lambda x: x%3==0, keep=True))\n\
[[1, 2], [3], [4, 5], [6], [7, 8], [9]]\n\
\n\
>>> list(split(range(1, 10), lambda x: x%3==0, maxsplit=1))\n\
[[1, 2], [4, 5, 6, 7, 8, 9]]\n\
\n\
>>> list(split([1,2,3,4,5,3,7,8,3], 3, eq=True))\n\
[[1, 2], [4, 5], [7, 8]]");

/******************************************************************************
 * Type
 *****************************************************************************/

static PyTypeObject PyIUType_Split = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.split",                        /* tp_name */
    sizeof(PyIUObject_Split),                           /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)split_dealloc,                          /* tp_dealloc */
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
    split_doc,                                          /* tp_doc */
    (traverseproc)split_traverse,                       /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)split_next,                           /* tp_iternext */
    split_methods,                                      /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    split_new,                                          /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
