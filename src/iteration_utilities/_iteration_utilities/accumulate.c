/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "accumulate.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"
#include "helper.h"

PyDoc_STRVAR(
    accumulate_prop_func_doc,
    "(callable or None) The function used for accumulation (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(
    accumulate_prop_current_doc,
    "(any type) The current accumulated total (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    accumulate_doc,
    "accumulate(iterable, func=None, start=None)\n"
    "--\n\n"
    "Make an iterator that returns accumulated sums, or accumulated\n"
    "results of other binary functions (specified via the optional `func`\n"
    "argument). Copied and modified from [0]_.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    The `iterable` to accumulate.\n"
    "\n"
    "func : callable or None, optional\n"
    "    The function with which to accumulate. Should be a function of two\n"
    "    arguments.\n"
    "    If ``None`` defaults to :py:func:`operator.add`.\n"
    "\n"
    "start : any type, optional\n"
    "    If given (even as ``None``) this value is inserted before the `iterable`.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "accumulated : generator\n"
    "    The accumulated results as generator.\n"
    "\n"
    "Notes\n"
    "-----\n"
    "Elements of the input `iterable` may be any type that can be\n"
    "accepted as arguments to `func`. (For example, with the default\n"
    "operation of addition, elements may be any addable type including\n"
    "Decimal or Fraction.) If the input `iterable` is empty, the output\n"
    "iterable will also be empty.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "There are a number of uses for the `func` argument. It can be set to\n"
    ":py:func:`min` for a running minimum, :py:func:`max` for a running\n"
    "maximum, or :py:func:`operator.mul` for a running product. Amortization\n"
    "tables can be built by accumulating interest and applying payments.\n"
    "First-order recurrence relations can be modeled by supplying the\n"
    "initial value in the `iterable` and using only the accumulated total in\n"
    "`func` argument::\n"
    "\n"
    "    >>> from iteration_utilities import accumulate\n"
    "    >>> from itertools import repeat\n"
    "    >>> import operator\n"
    "\n"
    "    >>> data = [3, 4, 6, 2, 1, 9, 0, 7, 5, 8]\n"
    "    >>> list(accumulate(data))                   # running sum\n"
    "    [3, 7, 13, 15, 16, 25, 25, 32, 37, 45]\n"
    "    >>> list(accumulate(data, operator.add))     # running sum (explicit)\n"
    "    [3, 7, 13, 15, 16, 25, 25, 32, 37, 45]\n"
    "    >>> list(accumulate(data, operator.mul))     # running product\n"
    "    [3, 12, 72, 144, 144, 1296, 0, 0, 0, 0]\n"
    "    >>> list(accumulate(data, max))              # running maximum\n"
    "    [3, 4, 6, 6, 6, 9, 9, 9, 9, 9]\n"
    "\n"
    "Amortize a 5% loan of 1000 (start value) with 4 annual payments of 90::\n"
    "\n"
    "    >>> cashflows = [-90, -90, -90, -90]\n"
    "    >>> list(accumulate(cashflows, lambda bal, pmt: bal*1.05 + pmt, 1000))\n"
    "    [960.0, 918.0, 873.9000000000001, 827.5950000000001]\n"
    "\n"
    "Chaotic recurrence relation [1]_::\n"
    "\n"
    "    >>> logistic_map = lambda x, _:  r * x * (1 - x)\n"
    "    >>> r = 3.8\n"
    "    >>> x0 = 0.4\n"
    "    >>> inputs = repeat(x0, 36)     # only the initial value is used\n"
    "    >>> [format(x, '.2f') for x in accumulate(inputs, logistic_map)]\n"
    "    ['0.40', '0.91', '0.30', '0.81', '0.60', '0.92', '0.29', '0.79', "
    "'0.63', '0.88', '0.39', '0.90', '0.33', '0.84', '0.52', '0.95', '0.18', "
    "'0.57', '0.93', '0.25', '0.71', '0.79', '0.63', '0.88', '0.39', '0.91', "
    "'0.32', '0.83', '0.54', '0.95', '0.20', '0.60', '0.91', '0.30', '0.80', '0.60']\n"
    "\n"
    "References\n"
    "----------\n"
    ".. [0] https://docs.python.org/3/library/itertools.html#itertools.accumulate\n"
    ".. [1] https://en.wikipedia.org/wiki/Logistic_map\n");

/******************************************************************************
 * Parts are taken from the CPython package (PSF licensed).
 *****************************************************************************/

static PyObject *
accumulate_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "func", "start", NULL};
    PyIUObject_Accumulate *self;
    PyObject *iterable;
    PyObject *binop = NULL;
    PyObject *start = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|OO:accumulate", kwlist,
                                     &iterable, &binop, &start)) {
        return NULL;
    }
    self = (PyIUObject_Accumulate *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->binop = binop == Py_None ? NULL : binop;
    Py_XINCREF(self->binop);
    Py_XINCREF(start);
    self->total = start;
    return (PyObject *)self;
}

static void
accumulate_dealloc(PyIUObject_Accumulate *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->binop);
    Py_XDECREF(self->total);
    Py_TYPE(self)->tp_free(self);
}

static int
accumulate_traverse(PyIUObject_Accumulate *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->binop);
    Py_VISIT(self->total);
    return 0;
}

static int
accumulate_clear(PyIUObject_Accumulate *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->binop);
    Py_CLEAR(self->total);
    return 0;
}

static PyObject *
accumulate_next(PyIUObject_Accumulate *self) {
    PyObject *item;
    PyObject *newtotal;

    /* Get next item from iterator.  */
    item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
    if (item == NULL) {
        return NULL;
    }
    /* If it's the first element the total is yet unset and we simply return
       the item.
       */
    if (self->total == NULL) {
        Py_INCREF(item);
        self->total = item;
        return item;
    }
    /* Apply the binop to the old total and the item defaulting to add if the
       binop is not set or set to None.
       */
    if (self->binop == NULL) {
        newtotal = PyNumber_Add(self->total, item);
    } else {
        newtotal = PyIU_CallWithTwoArguments(self->binop, self->total, item);
    }
    Py_DECREF(item);
    if (newtotal == NULL) {
        return NULL;
    }
    /* Update the total and return it. */
    Py_INCREF(newtotal);
    Py_SETREF(self->total, newtotal);
    return newtotal;
}

static PyObject *
accumulate_reduce(PyIUObject_Accumulate *self, PyObject *Py_UNUSED(args)) {
    /* Separate cases depending on total == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a start or
       that the start was None.
       Better to make an "if" than to introduce another variable depending on
       total == NULL.
       */
    if (self->total != NULL) {
        return Py_BuildValue("O(OOO)",
                             Py_TYPE(self),
                             self->iterator,
                             self->binop ? self->binop : Py_None,
                             self->total);
    } else {
        return Py_BuildValue("O(OO)",
                             Py_TYPE(self),
                             self->iterator,
                             self->binop ? self->binop : Py_None);
    }
}

static PyObject *
accumulate_lengthhint(PyIUObject_Accumulate *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }
    return PyLong_FromSsize_t(len);
}

static PyMethodDef accumulate_methods[] = {
    {
        "__length_hint__",                  /* ml_name */
        (PyCFunction)accumulate_lengthhint, /* ml_meth */
        METH_NOARGS,                        /* ml_flags */
        PYIU_lenhint_doc                    /* ml_doc */
    },
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)accumulate_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef accumulate_memberlist[] = {
    {
        "func",                                 /* name */
        T_OBJECT,                               /* type */
        offsetof(PyIUObject_Accumulate, binop), /* offset */
        READONLY,                               /* flags */
        accumulate_prop_func_doc                /* doc */
    },
    {
        "current",                              /* name */
        T_OBJECT_EX,                            /* type */
        offsetof(PyIUObject_Accumulate, total), /* offset */
        READONLY,                               /* flags */
        accumulate_prop_current_doc             /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Accumulate = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.accumulate", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Accumulate),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                 /* tp_itemsize */
    /* methods */
    (destructor)accumulate_dealloc,        /* tp_dealloc */
    (printfunc)0,                          /* tp_print */
    (getattrfunc)0,                        /* tp_getattr */
    (setattrfunc)0,                        /* tp_setattr */
    0,                                     /* tp_reserved */
    (reprfunc)0,                           /* tp_repr */
    (PyNumberMethods *)0,                  /* tp_as_number */
    (PySequenceMethods *)0,                /* tp_as_sequence */
    (PyMappingMethods *)0,                 /* tp_as_mapping */
    (hashfunc)0,                           /* tp_hash  */
    (ternaryfunc)0,                        /* tp_call */
    (reprfunc)0,                           /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc)0,                       /* tp_setattro */
    (PyBufferProcs *)0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,           /* tp_flags */
    (const char *)accumulate_doc,      /* tp_doc */
    (traverseproc)accumulate_traverse, /* tp_traverse */
    (inquiry)accumulate_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)accumulate_next,     /* tp_iternext */
    accumulate_methods,                /* tp_methods */
    accumulate_memberlist,             /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)0,                      /* tp_alloc */
    (newfunc)accumulate_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
