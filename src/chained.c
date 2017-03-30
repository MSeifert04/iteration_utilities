/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *funcs;
    int all;
    PyObject *funcargs;
} PyIUObject_Chained;

PyTypeObject PyIUType_Chained;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
chained_new(PyTypeObject *type,
            PyObject *funcs,
            PyObject *kwargs)
{
    static char *kwlist[] = {"reverse", "all", NULL};
    PyIUObject_Chained *self = NULL;

    int reverse = 0;
    int all = 0;
    Py_ssize_t num_funcs = PyTuple_GET_SIZE(funcs);

    /* Parse arguments */
    if (num_funcs == 0) {
        PyErr_Format(PyExc_TypeError, "at least 1 function must be given.");
        goto Fail;
    }

    if (!PyArg_ParseTupleAndKeywords(PyIU_global_0tuple, kwargs,
                                     "|ii:chained", kwlist,
                                     &reverse, &all)) {
        return NULL;
    }

    /* Create struct */
    self = (PyIUObject_Chained *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }

    if (!all) {
        self->funcargs = PyTuple_New(1);
        if (self->funcargs == NULL) {
            goto Fail;
        }
    }

    /* In case we want consecutive function calls (and not all) of them we can
       unwrap other "chained" instances inside the function-tuple passed in.
       */
    if (all == 0 && type == &PyIUType_Chained) {
        Py_ssize_t finalsize = 0;
        Py_ssize_t i;  /* Index for the input "funcs". */
        Py_ssize_t j;  /* Index for the out "funcs". */
        /* First pass over the data to get the number of functions. This is
           mostly unnecessary except when there are other "chained" instances
           inside the functions. These can be unwrapped.
           */
        for (i=0 ; i<num_funcs ; i++) {
            PyObject *function = PyTuple_GET_ITEM(funcs, i);
            if (Py_TYPE(function) == &PyIUType_Chained &&
                    ((PyIUObject_Chained *)function)->all == 0) {
                finalsize += PyTuple_GET_SIZE(((PyIUObject_Chained *)function)->funcs);
            } else {
                finalsize++;
            }
        }
        /* The second step involves creating a suitable tuple and inserting
           the functions while unwrapping "chained" instances that have
           "all == 0". We don't want to unwrap "all" chained instances because
           these don't participate in "chains" and would produce other results
           if unwrapped. One could argue that unwrapping "all"-chained inside
           other "all"-chained might make sense but it would change the way
           the function works and not to forget: That behaviour would be
           confusing.

           Special care has to be taken for "reversed" because even though all
           other functions are inserted in reversed order an unwrapped
           "chained" instance must be inserted in the original order so not to
           change the order of execution compared to the case when they would
           not be unwrapped.
           */
        self->funcs = PyTuple_New(finalsize);
        if (self->funcs == NULL) {
            goto Fail;
        }
        j = reverse ? (finalsize - 1) : 0;
        for (i=0 ; i<num_funcs ; i++) {
            PyObject *function = PyTuple_GET_ITEM(funcs, i);
            if (Py_TYPE(function) == &PyIUType_Chained &&
                    ((PyIUObject_Chained *)function)->all == 0) {
                Py_ssize_t k;
                PyIUObject_Chained *sub = (PyIUObject_Chained *)function;
                Py_ssize_t sub_size = PyTuple_GET_SIZE(sub->funcs);
                /* Prepare the index for inserting the array in normal order
                   even when "reversed" is given.
                   */
                j = reverse ? (j - sub_size + 1) : j;
                for (k=0 ; k<sub_size ; k++) {
                    PyObject *subfunc = PyTuple_GET_ITEM(sub->funcs, k);
                    Py_INCREF(subfunc);
                    PyTuple_SET_ITEM(self->funcs, j, subfunc);
                    j++;
                }
                /* The index needs to jump back to the original position in
                   case a "chained" instance was unwrapped while "reverse" was
                   given.
                   */
                j = reverse ? (j - sub_size - 1) : j;
            } else {
                /* This is the normal behaviour without unwrapping. Just change
                   the insertion index differently depending on "reverse".
                   */
                Py_INCREF(function);
                PyTuple_SET_ITEM(self->funcs, j, function);
                j = reverse ? (j - 1) : (j + 1);
            }
        }

    } else {
        if (reverse) {
            self->funcs = PyIU_TupleReverse(funcs);
        } else {
            self->funcs = PyIU_TupleCopy(funcs);
        }
    }

    if (self->funcs == NULL) {
        goto Fail;
    }

    self->all = all;
    return (PyObject *)self;

Fail:
    Py_XDECREF(self);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
chained_dealloc(PyIUObject_Chained *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->funcs);
    Py_XDECREF(self->funcargs);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
chained_traverse(PyIUObject_Chained *self,
                 visitproc visit,
                 void *arg)
{
    Py_VISIT(self->funcs);
    Py_VISIT(self->funcargs);
    return 0;
}

/******************************************************************************
 * Call
 *****************************************************************************/

static PyObject *
chained_call_normal(PyIUObject_Chained *self,
                    PyObject *args,
                    PyObject *kwargs)
{
    Py_ssize_t idx;

    PyObject *temp = PyObject_Call(PyTuple_GET_ITEM(self->funcs, 0),
                                   args, kwargs);
    if (temp == NULL) {
        return NULL;
    }

    for (idx=1 ; idx < PyTuple_GET_SIZE(self->funcs) ; idx++) {
        PyObject *func = PyTuple_GET_ITEM(self->funcs, idx);
        PyObject *oldtemp = temp;
        PYIU_RECYCLE_ARG_TUPLE(self->funcargs, temp, Py_DECREF(oldtemp);
                                                     return NULL);
        temp = PyObject_Call(func, self->funcargs, NULL);
        Py_DECREF(oldtemp);

        if (temp == NULL) {
            return NULL;
        }
    }

    return temp;
}

static PyObject *
chained_call_all(PyIUObject_Chained *self,
                 PyObject *args,
                 PyObject *kwargs)
{
    PyObject *result;
    Py_ssize_t idx;
    Py_ssize_t num_funcs = PyTuple_GET_SIZE(self->funcs);

    /* Create a placeholder tuple for "all=True".  */
    result = PyTuple_New(num_funcs);
    if (result == NULL) {
        return NULL;
    }

    for (idx=0 ; idx<num_funcs ; idx++) {
        PyObject *func = PyTuple_GET_ITEM(self->funcs, idx);
        PyObject *temp = PyObject_Call(func, args, kwargs);
        PyTuple_SET_ITEM(result, idx, temp);

        if (temp == NULL) {
            Py_DECREF(result);
            return NULL;
        }
    }

    return result;
}


static PyObject *
chained_call(PyIUObject_Chained *self,
             PyObject *args,
             PyObject *kwargs)
{
    if (self->all) {
        return chained_call_all(self, args, kwargs);
    } else {
        return chained_call_normal(self, args, kwargs);
    }
}

/******************************************************************************
 * Repr
 *****************************************************************************/

static PyObject *
chained_repr(PyIUObject_Chained *self)
{
    PyObject *result = NULL;
    PyObject *arglist;
    Py_ssize_t i, n;
    int ok;

    ok = Py_ReprEnter((PyObject *)self);
    if (ok != 0) {
        return ok > 0 ? PyUnicode_FromString("...") : NULL;
    }

    arglist = PyUnicode_FromString("");
    if (arglist == NULL) {
        goto done;
    }

    /* Pack positional arguments */
    n = PyTuple_GET_SIZE(self->funcs);
    for (i = 0; i < n; i++) {
        PyObject *tmp = PyUnicode_FromFormat("%U%R, ",
                                             arglist,
                                             PyTuple_GET_ITEM(self->funcs, i));
        Py_CLEAR(arglist);
        arglist = tmp;
        if (arglist == NULL) {
            goto done;
        }
    }

    result = PyUnicode_FromFormat("%s(%Uall=%R)",
                                  Py_TYPE(self)->tp_name,
                                  arglist,
                                  self->all ? Py_True : Py_False);
    Py_DECREF(arglist);


done:
    Py_ReprLeave((PyObject *)self);
    return result;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
chained_reduce(PyIUObject_Chained *self,
               PyObject *unused)
{
    return Py_BuildValue("OO(i)", Py_TYPE(self),
                         self->funcs,
                         self->all);
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
chained_setstate(PyIUObject_Chained *self,
                 PyObject *state)
{
    int all;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "i:chained.__setstate__", &all)) {
        return NULL;
    }

    self->all = all;
    Py_RETURN_NONE;
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef chained_methods[] = {
    {"__reduce__", (PyCFunction)chained_reduce, METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)chained_setstate, METH_O, PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(chained_doc, "chained(*funcs, /, reverse=False, all=False)\n\
--\n\
\n\
Chained function calls.\n\
\n\
Parameters\n\
----------\n\
funcs\n\
    Any number of callables.\n\
\n\
reverse : bool, optional\n\
    If ``True`` apply the the `funcs` in reversed order.\n\
    Default is ``False``.\n\
\n\
all : bool, optional\n\
    If ``True`` apply each of the `funcs` seperatly and return a tuple\n\
    containing the individual results when calling the instance.\n\
\n\
Returns\n\
-------\n\
chained_func : callable\n\
    The chained `funcs`.\n\
\n\
Methods\n\
-------\n\
__call__(\\*args, \\*\\*kwargs)\n\
   Returns ``func1(...(funcn(*args, **kwargs)))``.\n\
   Or ``funcn(...(func1(*args, **kwargs)))`` (if `reverse` is True).\n\
   Or ``func1(*args, **kwargs), ..., funcn(*args, **kwargs)`` if `all` is True.\n\
\n\
Examples\n\
--------\n\
`chained` simple calls all `funcs` on the result of the previous one::\n\
\n\
    >>> from iteration_utilities import chained\n\
    >>> double = lambda x: x*2\n\
    >>> increment = lambda x: x+1\n\
    >>> double_then_increment = chained(double, increment)\n\
    >>> double_then_increment(10)\n\
    21\n\
\n\
Or apply them in reversed order::\n\
\n\
    >>> increment_then_double = chained(double, increment, reverse=True)\n\
    >>> increment_then_double(10)\n\
    22\n\
\n\
Or apply all of them on the input::\n\
\n\
    >>> double_and_increment = chained(double, increment, all=True)\n\
    >>> double_and_increment(10)\n\
    (20, 11)");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_Chained = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.chained",                      /* tp_name */
    sizeof(PyIUObject_Chained),                         /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)chained_dealloc,                        /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)chained_repr,                             /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    (ternaryfunc)chained_call,                          /* tp_call */
    0,                                                  /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    chained_doc,                                        /* tp_doc */
    (traverseproc)chained_traverse,                     /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    0,                                                  /* tp_iter */
    0,                                                  /* tp_iternext */
    chained_methods,                                    /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    chained_new,                                        /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
