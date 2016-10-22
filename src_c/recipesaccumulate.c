/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

/******************************************************************************
 * Part of recipes.c
 *
 * - "accumulate" is only included for python 2 compatibility. But has a
 *   different signature as well.
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *total;
    PyObject *iterator;
    PyObject *binop;
} PyIUObject_Accumulate;

static PyTypeObject PyIUType_Accumulate;

/******************************************************************************
 *
 * New
 *
 *****************************************************************************/

static PyObject * accumulate_new(PyTypeObject *type, PyObject *args,
                                 PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "func", "start", NULL};
    PyIUObject_Accumulate *lz;

    PyObject *iterator, *iterable, *binop=NULL, *start=NULL;

    /* Parse arguments */
    // accumulate(iterable)
    if (PyTuple_Check(args) && PyTuple_Size(args) == 1 && kwargs == NULL) {
        if (!PyArg_UnpackTuple(args, "accumulate", 1, 1, &iterable)) {
            return NULL;
        }
    // accumulate(binop, iterable[, start])
    } else if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:accumulate", kwlist,
                                            &iterable, &binop, &start)) {
        return NULL;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }
    lz = (PyIUObject_Accumulate *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }
    Py_XINCREF(binop);
    Py_XINCREF(start);
    lz->binop = binop;
    lz->iterator = iterator;
    lz->total = start;
    return (PyObject *)lz;
}

/******************************************************************************
 *
 * Destructor
 *
 *****************************************************************************/

static void accumulate_dealloc(PyIUObject_Accumulate *lz) {
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->binop);
    Py_XDECREF(lz->total);
    Py_XDECREF(lz->iterator);
    Py_TYPE(lz)->tp_free(lz);
}

/******************************************************************************
 *
 * Traverse
 *
 *****************************************************************************/

static int accumulate_traverse(PyIUObject_Accumulate *lz, visitproc visit,
                               void *arg) {
    Py_VISIT(lz->binop);
    Py_VISIT(lz->iterator);
    Py_VISIT(lz->total);
    return 0;
}

/******************************************************************************
 *
 * Next
 *
 *****************************************************************************/

static PyObject * accumulate_next(PyIUObject_Accumulate *lz) {
    PyObject *item, *oldtotal, *newtotal;

    // Get next item from iterator
    item = (*Py_TYPE(lz->iterator)->tp_iternext)(lz->iterator);
    if (item == NULL) {
        PYIU_CLEAR_STOPITERATION;
        return NULL;
    }

    // If it's the first element the total is yet unset and we simply return
    // the item.
    if (lz->total == NULL) {
        Py_INCREF(item);
        lz->total = item;
        return lz->total;
    }

    // Apply the binop to the old total and the item defaulting to add if the
    // binop is not set or set to None.
    if (lz->binop == NULL || lz->binop == Py_None) {
        newtotal = PyNumber_Add(lz->total, item);
    } else {
        newtotal = PyObject_CallFunctionObjArgs(lz->binop, lz->total, item, NULL);
    }
    Py_DECREF(item);
    if (newtotal == NULL) {
        return NULL;
    }

    // Update to the new state
    oldtotal = lz->total;
    lz->total = newtotal;
    Py_DECREF(oldtotal);
    Py_INCREF(newtotal);
    return newtotal;
}

/******************************************************************************
 *
 * Reduce
 *
 *****************************************************************************/

static PyObject * accumulate_reduce(PyIUObject_Accumulate *lz) {
    if (lz->total != NULL) {
        return Py_BuildValue("O(OOO)",
                             Py_TYPE(lz),
                             lz->iterator,
                             lz->binop ? lz->binop : Py_None,
                             lz->total);
    } else {
        return Py_BuildValue("O(OO)",
                             Py_TYPE(lz),
                             lz->iterator,
                             lz->binop ? lz->binop : Py_None);
    }

}

/******************************************************************************
 *
 * Methods
 *
 *****************************************************************************/

static PyMethodDef accumulate_methods[] = {
    {"__reduce__", (PyCFunction)accumulate_reduce, METH_NOARGS, ""},
    {NULL, NULL}
};

/******************************************************************************
 *
 * Docstring
 *
 *****************************************************************************/

PyDoc_STRVAR(accumulate_doc, "accumulate(iterable[, func, start])\n\
\n\
Make an iterator that returns accumulated sums, or accumulated\n\
results of other binary functions (specified via the optional `func`\n\
argument). Copied and modified from [0]_.\n\
\n\
Parameters\n\
----------\n\
func : callable or None, optional\n\
    The function with which to accumulate. Should be a function of two\n\
    arguments.\n\
    If ``None`` defaults to :py:func:`operator.add`.\n\
\n\
iterable : iterable\n\
    The `iterable` to accumulate.\n\
\n\
start : any type, optional\n\
    If given this value is inserted before the `iterable`.\n\
\n\
Returns\n\
-------\n\
accumulated : generator\n\
    The accumulated results as generator.\n\
\n\
Notes\n\
-----\n\
Elements of the input `iterable` may be any type that can be\n\
accepted as arguments to `func`. (For example, with the default\n\
operation of addition, elements may be any addable type including\n\
Decimal or Fraction.) If the input `iterable` is empty, the output\n\
iterable will also be empty.\n\
\n\
Examples\n\
--------\n\
There are a number of uses for the `func` argument. It can be set to\n\
:py:func:`min` for a running minimum, :py:func:`max` for a running\n\
maximum, or :py:func:`operator.mul` for a running product. Amortization\n\
tables can be built by accumulating interest and applying payments.\n\
First-order recurrence relations can be modeled by supplying the\n\
initial value in the `iterable` and using only the accumulated total in\n\
`func` argument::\n\
\n\
    >>> from iteration_utilities import accumulate\n\
    >>> from itertools import repeat\n\
    >>> import operator\n\
\n\
    >>> data = [3, 4, 6, 2, 1, 9, 0, 7, 5, 8]\n\
    >>> list(accumulate(data))                   # running sum\n\
    [3, 7, 13, 15, 16, 25, 25, 32, 37, 45]\n\
    >>> list(accumulate(data, operator.add))     # running sum (explicit)\n\
    [3, 7, 13, 15, 16, 25, 25, 32, 37, 45]\n\
    >>> list(accumulate(data, operator.mul))     # running product\n\
    [3, 12, 72, 144, 144, 1296, 0, 0, 0, 0]\n\
    >>> list(accumulate(data, max))              # running maximum\n\
    [3, 4, 6, 6, 6, 9, 9, 9, 9, 9]\n\
\n\
Amortize a 5% loan of 1000 (start value) with 4 annual payments of 90::\n\
\n\
    >>> cashflows = [-90, -90, -90, -90]\n\
    >>> list(accumulate(cashflows, lambda bal, pmt: bal*1.05 + pmt, 1000))\n\
    [960.0, 918.0, 873.9000000000001, 827.5950000000001]\n\
\n\
Chaotic recurrence relation [1]_::\n\
\n\
    >>> logistic_map = lambda x, _:  r * x * (1 - x)\n\
    >>> r = 3.8\n\
    >>> x0 = 0.4\n\
    >>> inputs = repeat(x0, 36)     # only the initial value is used\n\
    >>> [format(x, '.2f') for x in accumulate(inputs, logistic_map)]\n\
    ['0.40', '0.91', '0.30', '0.81', '0.60', '0.92', '0.29', '0.79', \
'0.63', '0.88', '0.39', '0.90', '0.33', '0.84', '0.52', '0.95', '0.18', \
'0.57', '0.93', '0.25', '0.71', '0.79', '0.63', '0.88', '0.39', '0.91', \
'0.32', '0.83', '0.54', '0.95', '0.20', '0.60', '0.91', '0.30', '0.80', '0.60']\n\
\n\
References\n\
----------\n\
.. [0] https://docs.python.org/3/library/itertools.html#itertools.accumulate\n\
.. [1] https://en.wikipedia.org/wiki/Logistic_map");

/******************************************************************************
 *
 * Type
 *
 *****************************************************************************/

static PyTypeObject PyIUType_Accumulate = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.accumulate",   /* tp_name */
    sizeof(PyIUObject_Accumulate),      /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)accumulate_dealloc,     /* tp_dealloc */
    0,                                  /* tp_print */
    0,                                  /* tp_getattr */
    0,                                  /* tp_setattr */
    0,                                  /* tp_reserved */
    0,                                  /* tp_repr */
    0,                                  /* tp_as_number */
    0,                                  /* tp_as_sequence */
    0,                                  /* tp_as_mapping */
    0,                                  /* tp_hash */
    0,                                  /* tp_call */
    0,                                  /* tp_str */
    PyObject_GenericGetAttr,            /* tp_getattro */
    0,                                  /* tp_setattro */
    0,                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    accumulate_doc,                     /* tp_doc */
    (traverseproc)accumulate_traverse,  /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)accumulate_next,      /* tp_iternext */
    accumulate_methods,                 /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    accumulate_new,                     /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
