/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE.rst
 *****************************************************************************/

typedef struct {
    PyObject_HEAD
    PyObject *iteratorlist;
    PyObject *types;
    PyObject *ignore;
    Py_ssize_t depth;
    Py_ssize_t currentdepth;
    int isstring;
} PyIUObject_DeepFlatten;

PyTypeObject PyIUType_DeepFlatten;

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
deepflatten_new(PyTypeObject *type,
                PyObject *args,
                PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "depth", "types", "ignore", NULL};
    PyIUObject_DeepFlatten *self;

    PyObject *iterable;
    PyObject *iterator = NULL;
    PyObject *iteratorlist = NULL;
    PyObject *types = NULL;
    PyObject *ignore = NULL;
    Py_ssize_t depth = -1;
    Py_ssize_t i;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|nOO:deepflatten", kwlist,
                                     &iterable, &depth, &types, &ignore)) {
        goto Fail;
    }
    PYIU_NULL_IF_NONE(types);
    PYIU_NULL_IF_NONE(ignore);

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    /* Create a list of size "depth+1" or if depth was not given use 3 for
       a start. Fill all entries with None except for the first which should
       be the iterator over the iterable. */
    iteratorlist = PyList_New(depth >= 0 ? depth + 1 : 3);
    if (iteratorlist == NULL) {
        goto Fail;
    } else {
        PyList_SET_ITEM(iteratorlist, 0, iterator);
        iterator = NULL;
    }
    for (i=1 ; i < PyList_GET_SIZE(iteratorlist) ; i++) {
        Py_INCREF(Py_None);
        PyList_SET_ITEM(iteratorlist, i, Py_None);
    }
    self = (PyIUObject_DeepFlatten *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(types);
    Py_XINCREF(ignore);
    self->iteratorlist = iteratorlist;
    self->types = types;
    self->ignore = ignore;
    self->depth = depth;
    self->currentdepth = 0;
    self->isstring = 0;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    Py_XDECREF(iteratorlist);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
deepflatten_dealloc(PyIUObject_DeepFlatten *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iteratorlist);
    Py_XDECREF(self->types);
    Py_XDECREF(self->ignore);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
deepflatten_traverse(PyIUObject_DeepFlatten *self,
                     visitproc visit,
                     void *arg)
{
    Py_VISIT(self->iteratorlist);
    Py_VISIT(self->types);
    Py_VISIT(self->ignore);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
deepflatten_next(PyIUObject_DeepFlatten *self)
{
    PyObject *activeiterator, *item, *temp;

    if (self->currentdepth < 0) {
        return NULL;
    }

    /* TODO: This is likely a problem when using copy because currentdepth
             might be changed by the copy. However deepcopy should work as
             as expected.
       */
    activeiterator = PyList_GET_ITEM(self->iteratorlist, self->currentdepth);

    while (self->currentdepth >= 0) {
        item = (*Py_TYPE(activeiterator)->tp_iternext)(activeiterator);

        /* The active iterator finished, remove it from the list and take
           up the iterator one level up. */
        if (item == NULL) {
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                    PyErr_Clear();
                } else {
                    return NULL;
                }
            }

            Py_INCREF(Py_None);
            PyList_SET_ITEM(self->iteratorlist, self->currentdepth, Py_None);
            self->currentdepth--;
            /* The iterator finished so we're not in a string anymore. */
            self->isstring = 0;
            Py_DECREF(activeiterator);
            if (self->currentdepth < 0) {
                break;
            }
            activeiterator = PyList_GET_ITEM(self->iteratorlist,
                                             self->currentdepth);
            continue;
        }

        /* If the currentdepth exceeds the specified depth just return. */
        if (self->depth >= 0 && self->currentdepth >= self->depth) {
            return item;

        /* If we're in a built-in string/bytes or unicode simply return. */
        } else if (self->isstring) {
            return item;

        /* First check if the item is an instance of the ignored types, if
           it is, then simply return it. */
        } else if (self->ignore && PyObject_IsInstance(item, self->ignore)) {
            return item;

        /* If types is given then check if it's an instance thereof and if
           so replace activeiterator, otherwise return the item. */
        } else if (self->types) {
            if (PyObject_IsInstance(item, self->types)) {
                /* Check if it's a builtin-string-type and if so set
                   "isstring". Check for the exact type because sub types might
                   have custom __iter__ methods, better not to interfere with
                   these. */
#if PY_MAJOR_VERSION == 2
                if (PyString_CheckExact(item) || PyUnicode_CheckExact(item)) {
#else
                if (PyBytes_CheckExact(item) || PyUnicode_CheckExact(item)) {
#endif
                    self->isstring = 1;
                }
                self->currentdepth++;
                activeiterator = PyObject_GetIter(item);
                Py_DECREF(item);
                if (activeiterator == NULL) {
                    return NULL;
                }
            } else {
                return item;
            }

        /* If no types are given just try to convert it to an iterator and
           if that succeeds replaces activeiterator, otherwise return item.
           */
        } else {
            temp = PyObject_GetIter(item);
            if (temp == NULL) {
                if (PyErr_Occurred() &&
                        PyErr_ExceptionMatches(PyExc_TypeError)) {
                    PyErr_Clear();
                    return item;
                } else {
                    Py_DECREF(item);
                    return NULL;
                }
            } else {
                /* See comment above why the exact check is (probably)
                   better. */
#if PY_MAJOR_VERSION == 2
                if (PyString_CheckExact(item) || PyUnicode_CheckExact(item)) {
#else
                if (PyBytes_CheckExact(item) || PyUnicode_CheckExact(item)) {
#endif
                    self->isstring = 1;
                }
                self->currentdepth++;
                activeiterator = temp;
                temp = NULL;
                Py_DECREF(item);
            }
        }

        /* Still here? That means we have a new activeiterator.
           Make sure we can save the new iterator (if necessary increase
           the list size).
           However first make sure we are not in danger of being in an endless
           recursion, to this means we "borrow" the recursion depth built into
           Python as limit for the list length.
           */
        if ((Py_ssize_t)Py_GetRecursionLimit() < self->currentdepth) {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 5)
            PyErr_Format(PyExc_RecursionError,
#else
            PyErr_Format(PyExc_RuntimeError,
#endif
                         "deepflatten reached maximum recursion depth.");
            Py_DECREF(activeiterator);
            return NULL;
        }
        if (self->currentdepth >= PyList_GET_SIZE(self->iteratorlist)) {
            int ok = PyList_Append(self->iteratorlist, activeiterator);
            Py_DECREF(activeiterator);
            if (ok == -1) {
                return NULL;
            }
        } else {
            PyObject *tmp = PyList_GET_ITEM(self->iteratorlist, self->currentdepth);
            PyList_SET_ITEM(self->iteratorlist, self->currentdepth, activeiterator);
            Py_DECREF(tmp);
        }

    }
    return NULL;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
deepflatten_reduce(PyIUObject_DeepFlatten *self)
{
    PyObject *res;
    /* We need to copy the iteratorlist in case someone grabs it. This could
       lead to segmentation faults if the list is partially deleted, the next
       call to "next" could try to access an out-of-bounds index.
       */
    PyObject *itlist = PyList_GetSlice(self->iteratorlist, 0, PY_SSIZE_T_MAX);
    res = Py_BuildValue("O(OnOO)(Oni)", Py_TYPE(self),
                        PyList_GET_ITEM(self->iteratorlist, 0),  /* stub */
                        self->depth,
                        self->types ? self->types : Py_None,
                        self->ignore ? self->ignore : Py_None,
                        itlist,
                        self->currentdepth,
                        self->isstring);
    Py_DECREF(itlist);
    return res;
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
deepflatten_setstate(PyIUObject_DeepFlatten *self,
                     PyObject *state)
{
    PyObject *iteratorlist;
    Py_ssize_t currentdepth;
    int isstring;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "Oni:deepflatten.__setstate__",
                          &iteratorlist, &currentdepth, &isstring)) {
        return NULL;
    }

    if (!PyList_CheckExact(iteratorlist)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `list` instance as "
                     "first argument in the `state`, got %.200s.",
                     Py_TYPE(self)->tp_name, Py_TYPE(iteratorlist)->tp_name);
        return NULL;
    }
    if (currentdepth < -1) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the second (%zd) "
                     "argument in the `state` is bigger than or equal to -1.",
                     Py_TYPE(self)->tp_name, currentdepth);
        return NULL;
    } else {
        Py_ssize_t i;
        Py_ssize_t listlength = PyList_GET_SIZE(iteratorlist);
        if (currentdepth >= listlength) {
            PyErr_Format(PyExc_ValueError,
                         "`%.200s.__setstate__` expected that the second (%zd) "
                         "argument in the `state` is smaller than the length "
                         "of the first argument (%zd)",
                         Py_TYPE(self)->tp_name, currentdepth, listlength);
            return NULL;
        }
        /* The iteratorlist requires iterators in the list so make sure no
           bad items could be accessed. */
        for (i=0 ; i <= currentdepth ; i++) {
            if (!PyIter_Check(PyList_GET_ITEM(iteratorlist, i))) {
                PyErr_Format(PyExc_TypeError,
                             "`%.200s.__setstate__` expected only iterators "
                             "in the first argument in the `state`, got %.200s.",
                             Py_TYPE(self)->tp_name,
                             Py_TYPE(PyList_GET_ITEM(iteratorlist, i))->tp_name);
                return NULL;
            }
        }
    }

    /* We need to make sure nobody can alter the iteratorlist so we need a
       copy.
       */
    iteratorlist = PyList_GetSlice(iteratorlist, 0, PY_SSIZE_T_MAX);
    if (iteratorlist == NULL) {
        return NULL;
    }

    Py_CLEAR(self->iteratorlist);
    /* No need to incref iteratorlist, we copied it. */
    self->iteratorlist = iteratorlist;
    self->currentdepth = currentdepth;
    self->isstring = isstring;
    Py_RETURN_NONE;
}

/******************************************************************************
 * Methods
 *****************************************************************************/

static PyMethodDef deepflatten_methods[] = {
    {"__reduce__",   (PyCFunction)deepflatten_reduce,   METH_NOARGS, PYIU_reduce_doc},
    {"__setstate__", (PyCFunction)deepflatten_setstate, METH_O,      PYIU_setstate_doc},
    {NULL, NULL}
};

/******************************************************************************
 * Docstring
 *****************************************************************************/

PyDoc_STRVAR(deepflatten_doc, "deepflatten(iterable, depth=-1, types=None, ignore=None)\n\
--\n\
\n\
Flatten an `iterable` with given `depth`.\n\
\n\
Parameters\n\
----------\n\
iterable : iterable\n\
    Any `iterable` to flatten.\n\
\n\
depth : int or None, optional\n\
    Flatten `depth` levels of nesting or all if ``depth=-1``.\n\
    Default is ``-1``.\n\
\n\
    .. note::\n\
       If the `depth` is known this significantly speeds up the function!\n\
\n\
types : type, tuple of types, optional\n\
    Which types should be flattened. If not given it flattens all items if\n\
    ``iter(item)`` does not throw a ``TypeError``.\n\
\n\
    .. note::\n\
       If the `types` are known this significantly speeds up the function\n\
       but only *if* the `depth` is unknown.\n\
\n\
ignore : type, iterable of types or None, optional\n\
    The types which should not be flattened. If not given all `types` are\n\
    flattened.\n\
\n\
Returns\n\
-------\n\
flattened_iterable : generator\n\
    The `iterable` with the `depth` level of nesting flattened.\n\
\n\
Examples\n\
--------\n\
To flatten a given depth::\n\
\n\
    >>> from iteration_utilities import deepflatten, EQ_PY2\n\
    >>> list(deepflatten([1, [1,2], [[1,2]], [[[1,2]]]], depth=1))\n\
    [1, 1, 2, [1, 2], [[1, 2]]]\n\
\n\
To completly flatten it::\n\
\n\
    >>> list(deepflatten([1, [1,2], [[1,2]], [[[1,2]]]]))\n\
    [1, 1, 2, 1, 2, 1, 2]\n\
\n\
To ignore for example dictionaries::\n\
\n\
    >>> # Only the keys of a dictionary will be kept with deepflatten.\n\
    >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}]))\n\
    [1, 2, 1, 2, 1, 2]\n\
    >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}], ignore=dict))\n\
    [1, 2, 1, 2, {1: 10, 2: 10}]\n\
\n\
In this case we could have also chosen only to flatten the lists::\n\
\n\
    >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}], types=list))\n\
    [1, 2, 1, 2, {1: 10, 2: 10}]\n\
\n\
.. warning::\n\
    If the iterable contains recursive iterable objects (i.e. `UserString`)\n\
    one either needs to set ``ignore`` or a `depth` that is not ``None``.\n\
    Otherwise this will raise an ``RecursionError`` (or ``RuntimeError`` on\n\
    older Python versions) because each item in a ``UserString`` is itself a\n\
    ``UserString``, even if it has a length of 1! The builtin strings \n\
    (``str``, ``bytes``, ``unicode``) are special cased, but only the exact\n\
    types because subtypes might implement custom not-recusive ``__iter__``\n\
    methods. This means that these won't run into the infinite recursion,\n\
    but subclasses might.\n\
\n\
See for example::\n\
\n\
    >>> if EQ_PY2:\n\
    ...     from UserString import UserString\n\
    ... else:\n\
    ...     from collections import UserString\n\
    >>> list(deepflatten([1, 2, [1,2], UserString('abc')], depth=1))\n\
    [1, 2, 1, 2, 'a', 'b', 'c']\n\
    >>> list(deepflatten([1, 2, [1,2], UserString('abc')], ignore=UserString))\n\
    [1, 2, 1, 2, 'abc']\n\
\n\
For Python2 you should ignore ``basestring`` instead of ``str``.\n\
\n\
This function is roughly (it's missing some of the complicated details of\n\
the actual function) equivalent to this python function:\n\
\n\
.. code::\n\
\n\
    def deepflatten(iterable, depth=None, types=None, ignore=None):\n\
        if depth is None:\n\
            depth = float('inf')\n\
        if depth == -1:\n\
            yield iterable\n\
        else:\n\
            for x in iterable:\n\
                if ignore is not None and isinstance(x, ignore):\n\
                    yield x\n\
                if types is None:\n\
                    try:\n\
                        iter(x)\n\
                    except TypeError:\n\
                        yield x\n\
                    else:\n\
                        for item in deepflatten(x, depth - 1, types, ignore):\n\
                            yield item\n\
                elif not isinstance(x, types):\n\
                    yield x\n\
                else:\n\
                    for item in deepflatten(x, depth - 1, types, ignore):\n\
                        yield item");

/******************************************************************************
 * Type
 *****************************************************************************/

PyTypeObject PyIUType_DeepFlatten = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iteration_utilities.deepflatten",                  /* tp_name */
    sizeof(PyIUObject_DeepFlatten),                     /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    /* methods */
    (destructor)deepflatten_dealloc,                    /* tp_dealloc */
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
    deepflatten_doc,                                    /* tp_doc */
    (traverseproc)deepflatten_traverse,                 /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)deepflatten_next,                     /* tp_iternext */
    deepflatten_methods,                                /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    deepflatten_new,                                    /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};
