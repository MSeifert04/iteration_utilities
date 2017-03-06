/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;
    PyObject *keyfunc;
    PyObject *lastitem;
    PyObject *funcargs;
} PyIUObject_UniqueJust;

PyTypeObject PyIUType_UniqueJust;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
uniquejust_new(PyTypeObject *type,
               PyObject *args,
               PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_UniqueJust *self;

    PyObject *iterable, *iterator, *keyfunc=NULL, *funcargs=NULL;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_justseen", kwlist,
                                     &iterable, &keyfunc)) {
        return NULL;
    }

    /* Create and fill struct */
    if (keyfunc == Py_None) {
        keyfunc = NULL;
    }
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        return NULL;
    }
    self = (PyIUObject_UniqueJust *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_DECREF(iterator);
        Py_XDECREF(keyfunc);
        Py_XDECREF(funcargs);
        return NULL;
    }
    Py_XINCREF(keyfunc);
    self->iterator = iterator;
    self->keyfunc = keyfunc;
    self->lastitem = NULL;
    self->funcargs = funcargs;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
uniquejust_dealloc(PyIUObject_UniqueJust *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->keyfunc);
    Py_XDECREF(self->lastitem);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
uniquejust_traverse(PyIUObject_UniqueJust *self,
                    visitproc visit,
                    void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->keyfunc);
    Py_VISIT(self->lastitem);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
uniquejust_next(PyIUObject_UniqueJust *self)
{
    PyObject *item, *old, *val=NULL, *tmp=NULL;
    int ok;

    while ( (item = (*Py_TYPE(self->iterator)->tp_iternext)(self->iterator)) ) {

        /* Apply keyfunc or use the original */
        if (self->keyfunc == NULL) {
            Py_INCREF(item);
            val = item;
        } else {
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, tmp, goto Fail)
            val = PyObject_Call(self->keyfunc, self->funcargs, NULL);
            if (val == NULL) {
                goto Fail;
            }
        }

        /* If no lastitem set it to the current and simply return the item. */
        if (self->lastitem == NULL) {
            self->lastitem = val;
            return item;
        }

        /* Otherwise compare it with the last item and only return it if it
           differs. */
        ok = PyObject_RichCompareBool(val, self->lastitem, Py_EQ);

        /* Not equal */
        if (ok == 0) {
            old = self->lastitem;
            self->lastitem = val;
            Py_DECREF(old);
            return item;

        /* Failure */
        } else if (ok < 0) {
            goto Fail;
        }

        Py_DECREF(val);
        Py_DECREF(item);
    }
    return NULL;

Fail:
    Py_DECREF(item);
    Py_XDECREF(val);
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
uniquejust_reduce(PyIUObject_UniqueJust *self)
{
    PyObject *value;

    if (self->lastitem != NULL) {
        value = Py_BuildValue("O(OO)(O)", Py_TYPE(self),
                             self->iterator,
                             self->keyfunc ? self->keyfunc : Py_None,
                             self->lastitem ? self->lastitem : Py_None);
    } else {
        value = Py_BuildValue("O(OO)", Py_TYPE(self),
                             self->iterator,
                             self->keyfunc ? self->keyfunc : Py_None);
    }

    return value;
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
uniquejust_setstate(PyIUObject_UniqueJust *self,
                    PyObject *state)
{
    PyObject *lastitem;

    if (!PyArg_ParseTuple(state, "O", &lastitem)) {
        return NULL;
    }

    Py_CLEAR(self->lastitem);
    self->lastitem = lastitem;
    Py_INCREF(self->lastitem);

    Py_RETURN_NONE;
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef uniquejust_methods[] = {
    {"__reduce__", (PyCFunction)uniquejust_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)uniquejust_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(uniquejust_doc, "unique_justseen(iterable, key=None)\n\
--\n\
\n\
List unique elements, preserving order. Remember only the element just seen.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` to check.\n\
\n\
key : callable or None, optional\n\
    If ``None`` the values are taken as they are. If it's a callable the\n\
    callable is applied to the value before comparing it.\n\
    Default is ``None``.\n\
\n\
Returns\n\
-------\n\
iterable : generator\n\
    An iterable containing all unique values just seen in the `iterable`.\n\
\n\
Examples\n\
--------\n\
>>> from iteration_utilities import unique_justseen\n\
>>> list(unique_justseen('AAAABBBCCDAABBB'))\n\
['A', 'B', 'C', 'D', 'A', 'B']\n\
\n\
>>> list(unique_justseen('ABBCcAD', str.lower))\n\
['A', 'B', 'C', 'A', 'D']");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_UniqueJust = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.unique_justseen",              /* tp_name */
    sizeof(PyIUObject_UniqueJust),                      /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)uniquejust_dealloc,                     /* tp_dealloc */
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
    uniquejust_doc,                                     /* tp_doc */
    (traverseproc)uniquejust_traverse,                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)uniquejust_next,                      /* tp_iternext */
    uniquejust_methods,                                 /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    uniquejust_new,                                     /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
