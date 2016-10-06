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
} recipes_accumulateobject;


static PyTypeObject recipes_accumulate_type;


static PyObject *
recipes_accumulate_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwargs[] = {"func", "iterable", "start", NULL};
    PyObject *iterable=NULL, *binop=NULL, *start=NULL;
    PyObject *iterator;
    recipes_accumulateobject *lz;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OO:accumulate",
                                     kwargs, &binop, &iterable, &start)) {
        return NULL;
    }

    /* If only one positional argument was given interpret this as iterable. */
    if (kwds == NULL && iterable == NULL && start == NULL) {
        iterable = binop;
        binop = NULL;
    }

    /* Get iterator. */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        return NULL;
    }

    /* create accumulateobject structure */
    lz = (recipes_accumulateobject *)type->tp_alloc(type, 0);
    if (lz == NULL) {
        Py_DECREF(iterator);
        return NULL;
    }

    if (binop != Py_None) {
        Py_XINCREF(binop);
        lz->binop = binop;
    }

    if (start != NULL && start != Py_None) {
        Py_INCREF(start);
        lz->total = start;
    } else {
        lz->total = NULL;
    }

    lz->iterator = iterator;
    return (PyObject *)lz;
}


static void
recipes_accumulate_dealloc(recipes_accumulateobject *lz)
{
    PyObject_GC_UnTrack(lz);
    Py_XDECREF(lz->binop);
    Py_XDECREF(lz->total);
    Py_XDECREF(lz->iterator);
    Py_TYPE(lz)->tp_free(lz);
}


static int
recipes_accumulate_traverse(recipes_accumulateobject *lz, visitproc visit, void *arg)
{
    Py_VISIT(lz->binop);
    Py_VISIT(lz->iterator);
    Py_VISIT(lz->total);
    return 0;
}


static PyObject *
recipes_accumulate_next(recipes_accumulateobject *lz)
{
    PyObject *val, *oldtotal, *newtotal;

    val = (*Py_TYPE(lz->iterator)->tp_iternext)(lz->iterator);
    if (val == NULL) {
        return NULL;
    }

    if (lz->total == NULL) {
        Py_INCREF(val);
        lz->total = val;
        return lz->total;
    }

    if (lz->binop == NULL || lz->binop == Py_None) {
        newtotal = PyNumber_Add(lz->total, val);
    } else {
        newtotal = PyObject_CallFunctionObjArgs(lz->binop, lz->total, val, NULL);
    }
    Py_DECREF(val);
    if (newtotal == NULL) {
        return NULL;
    }

    oldtotal = lz->total;
    lz->total = newtotal;
    Py_DECREF(oldtotal);
    Py_INCREF(newtotal);

    return newtotal;
}

static PyObject *
recipes_accumulate_reduce(recipes_accumulateobject *lz)
{
    if (lz->total != NULL) {
        return Py_BuildValue("O(OOO)",
                             Py_TYPE(lz),
                             lz->binop ? lz->binop : Py_None,
                             lz->iterator,
                             lz->total);
    } else {
        return Py_BuildValue("O(OO)",
                             Py_TYPE(lz),
                             lz->binop ? lz->binop : Py_None,
                             lz->iterator);
    }

}

static PyMethodDef recipes_accumulate_methods[] = {
    {"__reduce__",
     (PyCFunction)recipes_accumulate_reduce,
     METH_NOARGS,
     ""},

    {NULL,              NULL}   /* sentinel */
};


PyDoc_STRVAR(recipes_accumulate_doc,
"accumulate(iterable, *)\n\
accumulate(func, iterable[, start])\n\
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
    >>> list(accumulate(operator.add, data))     # running sum (explicit)\n\
    [3, 7, 13, 15, 16, 25, 25, 32, 37, 45]\n\
    >>> list(accumulate(operator.mul, data))     # running product\n\
    [3, 12, 72, 144, 144, 1296, 0, 0, 0, 0]\n\
    >>> list(accumulate(max, data))              # running maximum\n\
    [3, 4, 6, 6, 6, 9, 9, 9, 9, 9]\n\
\n\
Amortize a 5% loan of 1000 (start value) with 4 annual payments of 90::\n\
\n\
    >>> cashflows = [-90, -90, -90, -90]\n\
    >>> list(accumulate(lambda bal, pmt: bal*1.05 + pmt, cashflows, 1000))\n\
    [960.0, 918.0, 873.9000000000001, 827.5950000000001]\n\
\n\
Chaotic recurrence relation [1]_::\n\
\n\
    >>> logistic_map = lambda x, _:  r * x * (1 - x)\n\
    >>> r = 3.8\n\
    >>> x0 = 0.4\n\
    >>> inputs = repeat(x0, 36)     # only the initial value is used\n\
    >>> [format(x, '.2f') for x in accumulate(logistic_map, inputs)]\n\
    ['0.40', '0.91', '0.30', '0.81', '0.60', '0.92', '0.29', '0.79', \
'0.63', '0.88', '0.39', '0.90', '0.33', '0.84', '0.52', '0.95', '0.18', \
'0.57', '0.93', '0.25', '0.71', '0.79', '0.63', '0.88', '0.39', '0.91', \
'0.32', '0.83', '0.54', '0.95', '0.20', '0.60', '0.91', '0.30', '0.80', '0.60']\n\
\n\
References\n\
----------\n\
.. [0] https://docs.python.org/3/library/itertools.html#itertools.accumulate\n\
.. [1] https://en.wikipedia.org/wiki/Logistic_map");

static PyTypeObject recipes_accumulate_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.accumulate",   /* tp_name */
    sizeof(recipes_accumulateobject),   /* tp_basicsize */
    0,                                  /* tp_itemsize */
    /* methods */
    (destructor)recipes_accumulate_dealloc, /* tp_dealloc */
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
    recipes_accumulate_doc,             /* tp_doc */
    (traverseproc)recipes_accumulate_traverse, /* tp_traverse */
    0,                                  /* tp_clear */
    0,                                  /* tp_richcompare */
    0,                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                  /* tp_iter */
    (iternextfunc)recipes_accumulate_next, /* tp_iternext */
    recipes_accumulate_methods,         /* tp_methods */
    0,                                  /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    0,                                  /* tp_descr_get */
    0,                                  /* tp_descr_set */
    0,                                  /* tp_dictoffset */
    0,                                  /* tp_init */
    0,                                  /* tp_alloc */
    recipes_accumulate_new,             /* tp_new */
    PyObject_GC_Del,                    /* tp_free */
};
