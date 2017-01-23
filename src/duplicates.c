/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *
 * IMPORTANT NOTE (Implementation):
 *
 * This function is almost identical to "unique_everseen", so any changes
 * or bugfixes should also be implemented there!!!
 *
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *key;
    PyObject *seen;
    PyObject *funcargs;
} PyIUObject_Duplicates;

PyTypeObject PyIUType_Duplicates;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
duplicates_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_Duplicates *self;

    PyObject *iterable, *iterator, *seen, *key=NULL, *funcargs=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:duplicates", kwlist,
                                     &iterable, &key)) {
        return NULL;
    }
    if (key == Py_None) {
        key = NULL;
    }
    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        return NULL;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        Py_DECREF(funcargs);
        return NULL;
    }
    seen = PyIUSeen_New();
    if (seen == NULL) {
        Py_DECREF(funcargs);
        Py_DECREF(iterator);
        return NULL;
    }
    self = (PyIUObject_Duplicates *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(funcargs);
        Py_DECREF(iterator);
        Py_DECREF(seen);
        return NULL;
    }
    Py_XINCREF(key);
    self->iterator = iterator;
    self->key = key;
    self->seen = seen;
    self->funcargs = funcargs;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
duplicates_dealloc(PyIUObject_Duplicates *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->key);
    Py_XDECREF(self->seen);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
duplicates_traverse(PyIUObject_Duplicates *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->key);
    Py_VISIT(self->seen);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
duplicates_next(PyIUObject_Duplicates *self)
{
    PyObject *item=NULL, *temp=NULL, *tmp=NULL;
    int ok;

    while ( (item = (*Py_TYPE(self->iterator)->tp_iternext)(self->iterator)) ) {

        /* Use the item if key is not given, otherwise apply the key. */
        if (self->key == NULL) {
            temp = item;
        } else {
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, tmp, goto Fail)
            temp = PyObject_Call(self->key, self->funcargs, NULL);
            if (temp == NULL) {
                goto Fail;
            }
        }

        /* Check if the item is in seen.  */
        ok = PyIUSeen_ContainsAdd(self->seen, temp);
        if (ok == 1) {
            goto Found;
        /* Failure when looking. */
        } else if (ok == -1) {
            goto Fail;
        }

        /* We have found the item either in the set or list so continue. */
        if (self->key != NULL) {
            Py_DECREF(temp);
        }
        Py_DECREF(item);
    }
    PYIU_CLEAR_STOPITERATION;
    return NULL;

Found:
    if (self->key != NULL) {
        Py_XDECREF(temp);
    }
    return item;
Fail:
    if (self->key != NULL) {
        Py_XDECREF(temp);
    }
    Py_XDECREF(item);
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
duplicates_reduce(PyIUObject_Duplicates *self)
{
    PyObject *value;
    value = Py_BuildValue("O(OO)(O)", Py_TYPE(self),
                          self->iterator,
                          self->key ? self->key : Py_None,
                          self->seen);
    return value;
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
duplicates_setstate(PyIUObject_Duplicates *self,
                    PyObject *state)
{
    PyObject *seen;

    if (!PyArg_ParseTuple(state, "O", &seen)) {
        return NULL;
    }

    Py_CLEAR(self->seen);
    self->seen = seen;
    Py_INCREF(self->seen);
    Py_RETURN_NONE;
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef duplicates_methods[] = {
    {"__reduce__",   (PyCFunction)duplicates_reduce,   METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)duplicates_setstate, METH_O,      PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Seen property
 *****************************************************************************/

static PyObject *
duplicates_getseen(PyIUObject_Duplicates *self,
                   void *closure)
{
    Py_INCREF(self->seen);
    return self->seen;
}

/******************************************************************************
 * key property
 *****************************************************************************/

static PyObject *
duplicates_getkey(PyIUObject_Duplicates *self,
                  void *closure)
{
    if (self->key == NULL) {
        Py_RETURN_NONE;
    }
    Py_INCREF(self->key);
    return self->key;
}

/******************************************************************************
 * Properties
 *****************************************************************************/

static PyGetSetDef duplicates_getsetlist[] = {
    {"seen",     (getter)duplicates_getseen,     NULL},
    {"key",      (getter)duplicates_getkey,      NULL},
    {NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(duplicates_doc, "duplicates(iterable, key=None)\n\
--\n\
\n\
Return only duplicate entries, remembers all items ever seen.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` containing the elements.\n\
\n\
key : callable, optional\n\
    If given it must be a callable taking one argument and this\n\
    callable is applied to the value before checking if it was seen yet.\n\
\n\
Returns\n\
-------\n\
iterable : generator\n\
    An iterable containing all duplicates values of the `iterable`.\n\
\n\
Attributes\n\
----------\n\
seen : Seen\n\
    Already seen values.\n\
key : callable, None\n\
    The key function.\n\
\n\
Notes\n\
-----\n\
The items in the `iterable` should implement equality.\n\
\n\
If the items are hashable the function is much faster.\n\
\n\
Examples\n\
--------\n\
Multiple duplicates will be kept::\n\
\n\
    >>> from iteration_utilities import duplicates\n\
    >>> list(duplicates('AABBCCDA'))\n\
    ['A', 'B', 'C', 'A']\n\
\n\
    >>> list(duplicates('ABBCcAD', str.lower))\n\
    ['B', 'c', 'A']\n\
\n\
To get each duplicate only once this can be combined with \n\
:py:func:`~iteration_utilities.unique_everseen`::\n\
\n\
    >>> from iteration_utilities import unique_everseen\n\
    >>> list(unique_everseen(duplicates('AABBCCDA')))\n\
    ['A', 'B', 'C']");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Duplicates = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.duplicates",                   /* tp_name */
    sizeof(PyIUObject_Duplicates),                      /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)duplicates_dealloc,                     /* tp_dealloc */
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
    duplicates_doc,                                     /* tp_doc */
    (traverseproc)duplicates_traverse,                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)duplicates_next,                      /* tp_iternext */
    duplicates_methods,                                 /* tp_methods */
    0,                                                  /* tp_members */
    duplicates_getsetlist,                              /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    duplicates_new,                                     /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
