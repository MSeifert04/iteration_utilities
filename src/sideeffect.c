/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iterator;  /* iterator over data */
    PyObject *func;      /* Function to call */
    Py_ssize_t times;    /* Call side effects each x items */
    Py_ssize_t count;    /* Current counter when to call func */
    PyObject *collected; /* Collect items to pass to side-effects */
    PyObject *funcargs;  /* Wrapper for the arguments for the function */
} PyIUObject_Sideeffects;

PyTypeObject PyIUType_Sideeffects;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
sideeffects_new(PyTypeObject *type,
                PyObject *args,
                PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "func", "times", NULL};
    PyIUObject_Sideeffects *self;

    PyObject *iterable;
    PyObject *iterator;
    PyObject *func;
    Py_ssize_t times = 0;
    Py_ssize_t count = 0;
    PyObject *collected;
    PyObject *funcargs;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|n:sideeffects", kwlist,
                                     &iterable, &func, &times)) {
        return NULL;
    }
    if (times <= 0) {  /* negative values will be interpreted as zero... */
        times = 0;
        collected = NULL;
    } else {
        collected = PyTuple_New(times);
        if (collected == NULL) {
            return NULL;
        }
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        Py_XDECREF(collected);
        return NULL;
    }
    funcargs = PyTuple_New(1);
    if (funcargs == NULL) {
        Py_XDECREF(collected);
        Py_DECREF(iterator);
        return NULL;
    }
    self = (PyIUObject_Sideeffects *)type->tp_alloc(type, 0);
    if (self == NULL) {
        Py_XDECREF(collected);
        Py_DECREF(iterator);
        Py_DECREF(funcargs);
        return NULL;
    }
    Py_INCREF(func);
    self->iterator = iterator;
    self->func = func;
    self->times = times;
    self->count = count;
    self->collected = collected;
    self->funcargs = funcargs;
    return (PyObject *)self;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
sideeffects_dealloc(PyIUObject_Sideeffects *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->func);
    Py_XDECREF(self->collected);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
sideeffects_traverse(PyIUObject_Sideeffects *self,
                     visitproc visit,
                     void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->func);
    Py_VISIT(self->collected);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
sideeffects_next(PyIUObject_Sideeffects *self)
{
    PyObject *item, *tmp=NULL, *temp=NULL, *tmptuple=NULL;
    Py_ssize_t i;

    item = (*Py_TYPE(self->iterator)->tp_iternext)(self->iterator);
    if (item == NULL) {
        PYIU_CLEAR_STOPITERATION;
        if (self->count != 0) {
            /* Call function with the remaining items. */
            tmptuple = PyTuple_GetSlice(self->collected, 0, self->count);
            if (tmptuple == NULL) {
                goto Fail;
            }
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, tmptuple, tmp, return NULL)
            temp = PyObject_Call(self->func, self->funcargs, NULL);
            Py_DECREF(tmptuple);
            if (temp == NULL) {
                return NULL;
            } else {
                Py_DECREF(temp);
            }
        }
        return NULL;
    }

    if (self->times == 0) {
        /* Always call the function if times == 0 */
        PYIU_RECYCLE_ARG_TUPLE(self->funcargs, item, tmp, goto Fail)
        temp = PyObject_Call(self->func, self->funcargs, NULL);
        if (temp == NULL) {
            goto Fail;
        } else {
            Py_DECREF(temp);
        }
    } else {
        Py_INCREF(item);
        /* Add the item to the collected tuple and call the function if
           count == times after incrementing the count. */
        PyTuple_SET_ITEM(self->collected, self->count, item);
        self->count++;
        if (self->count == self->times) {
            self->count = 0;
            PYIU_RECYCLE_ARG_TUPLE(self->funcargs, self->collected, tmp, goto Fail)
            temp = PyObject_Call(self->func, self->funcargs, NULL);
            if (temp == NULL) {
                goto Fail;
            } else {
                Py_DECREF(temp);
            }
            /* If the "collected" has a refcount of 1 after calling the
               function reuse it, otherwise create a new one */
            if (Py_REFCNT(self->collected) == 1) {
                for (i=0 ; i < self->times ; i++) {
                    temp = PyTuple_GET_ITEM(self->collected, i);
                    PyTuple_SET_ITEM(self->collected, i, NULL);
                    Py_DECREF(temp);
                }
            } else {
                Py_DECREF(self->collected);
                self->collected = PyTuple_New(self->times);
                if (self->collected == NULL) {
                    goto Fail;
                }
            }
        }
    }

    return item;

Fail:
    Py_DECREF(item);
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
sideeffects_reduce(PyIUObject_Sideeffects *self)
{
    Py_ssize_t i;
    PyObject *temp;
    /* Need to set all unset elements to None otherwise not possible to
       pickle the collected tuple. */
    for (i=0 ; i < self->times ; i++) {
        temp = PyTuple_GET_ITEM(self->collected, i);
        if (temp == NULL) {
            Py_INCREF(Py_None);
            PyTuple_SET_ITEM(self->collected, i, Py_None);
        }
    }
    return Py_BuildValue("O(OOn)(nO)", Py_TYPE(self),
                         self->iterator, self->func, self->times,
                         self->count,
                         self->collected ? self->collected : Py_None);
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
sideeffects_setstate(PyIUObject_Sideeffects *self,
                     PyObject *state)
{
    Py_ssize_t count;
    PyObject *collected;
    if (!PyArg_ParseTuple(state, "nO", &count, &collected)) {
        return NULL;
    }

    self->count = count;

    if (collected != Py_None) {
        Py_CLEAR(self->collected);
        Py_INCREF(collected);
        self->collected = collected;
    }

    Py_RETURN_NONE;
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
static PyObject *
sideeffects_lengthhint(PyIUObject_Sideeffects *self)
{
    return PyLong_FromSsize_t(PyObject_LengthHint(self->iterator, 0));
}
#endif

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef sideeffects_methods[] = {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 4)
    {"__length_hint__", (PyCFunction)sideeffects_lengthhint, METH_NOARGS, PYIU_lenhint_doc},
#endif
    {"__reduce__", (PyCFunction)sideeffects_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)sideeffects_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(sideeffects_doc, "sideeffects(iterable, func, times=0)\n\
--\n\
\n\
Does a normal iteration over `iterable` and only uses `func` each `times` \n\
items for it's side effects.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    `Iterable` containing the elements.\n\
\n\
func : callable\n\
    Function that is called for the side effects.\n\
\n\
times : int, optional\n\
    Call the function each `times` items with the last `times` items. \n\
    If ``0`` the argument for `func` will be the item itself. For any \n\
    number greater than zero the argument will be a tuple.\n\
    Default is ``0``.\n\
\n\
Returns\n\
-------\n\
iterator : generator\n\
    A normal iterator over `iterable`.\n\
\n\
Examples\n\
--------\n\
A simple example::\n\
\n\
    >>> from iteration_utilities import sideeffects\n\
    >>> def printit(val):\n\
    ...     print(val)\n\
    >>> list(sideeffects([1,2,3,4], printit))  # in python3 one could use print directly\n\
    1\n\
    2\n\
    3\n\
    4\n\
    [1, 2, 3, 4]\n\
    >>> list(sideeffects([1,2,3,4,5], printit, 2))\n\
    (1, 2)\n\
    (3, 4)\n\
    (5,)\n\
    [1, 2, 3, 4, 5]\n\
\n\
");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Sideeffects = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.sideeffects",                  /* tp_name */
    sizeof(PyIUObject_Sideeffects),                     /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)sideeffects_dealloc,                    /* tp_dealloc */
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
    sideeffects_doc,                                    /* tp_doc */
    (traverseproc)sideeffects_traverse,                 /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)sideeffects_next,                     /* tp_iternext */
    sideeffects_methods,                                /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    sideeffects_new,                                    /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};