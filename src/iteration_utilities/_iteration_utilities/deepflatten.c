/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "deepflatten.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"

PyDoc_STRVAR(
    deepflatten_prop_types_doc,
    "(:py:class:`type` or :py:class:`tuple` thereof) The types to flatten or "
    "None if `deepflatten` attempts to flatten every type (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(
    deepflatten_prop_ignore_doc,
    "(:py:class:`type` or :py:class:`tuple` thereof) The types that are not "
    "flattened (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(
    deepflatten_prop_depth_doc,
    "(:py:class:`int`) Up to this depth the iterable is flattened (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(
    deepflatten_prop_currentdepth_doc,
    "(:py:class:`int`) The current depth inside the iterable (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    deepflatten_doc,
    "deepflatten(iterable, depth=-1, types=None, ignore=None)\n"
    "--\n\n"
    "Flatten an `iterable` with given `depth`.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    Any `iterable` to flatten.\n"
    "\n"
    "depth : :py:class:`int` or None, optional\n"
    "    Flatten `depth` levels of nesting or all if ``depth=-1``.\n"
    "    Default is ``-1``.\n"
    "\n"
    "    .. note::\n"
    "       If the `depth` is known this significantly speeds up the function!\n"
    "\n"
    "types : type, tuple of types, optional\n"
    "    Which types should be flattened. If not given it flattens all items if\n"
    "    ``iter(item)`` does not throw a ``TypeError``.\n"
    "\n"
    "    .. note::\n"
    "       If the `types` are given this significantly speeds up the function\n"
    "       but only *if* the `depth` is unknown.\n"
    "\n"
    "ignore : type, iterable of types or None, optional\n"
    "    The types which should not be flattened. If not given all `types` are\n"
    "    flattened.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "flattened_iterable : generator\n"
    "    The `iterable` with the `depth` level of nesting flattened.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "To flatten a given depth::\n"
    "\n"
    "    >>> from iteration_utilities import deepflatten\n"
    "    >>> list(deepflatten([1, [1,2], [[1,2]], [[[1,2]]]], depth=1))\n"
    "    [1, 1, 2, [1, 2], [[1, 2]]]\n"
    "\n"
    "To completely flatten it::\n"
    "\n"
    "    >>> list(deepflatten([1, [1,2], [[1,2]], [[[1,2]]]]))\n"
    "    [1, 1, 2, 1, 2, 1, 2]\n"
    "\n"
    "To ignore for example dictionaries::\n"
    "\n"
    "    >>> # Only the keys of a dictionary will be kept with deepflatten.\n"
    "    >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}]))\n"
    "    [1, 2, 1, 2, 1, 2]\n"
    "    >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}], ignore=dict))\n"
    "    [1, 2, 1, 2, {1: 10, 2: 10}]\n"
    "\n"
    "In this case we could have also chosen only to flatten the lists::\n"
    "\n"
    "    >>> list(deepflatten([1, 2, [1,2],  {1: 10, 2: 10}], types=list))\n"
    "    [1, 2, 1, 2, {1: 10, 2: 10}]\n"
    "\n"
    ".. warning::\n"
    "    If the iterable contains recursive iterable objects (i.e. `UserString`)\n"
    "    one either needs to set ``ignore`` or a `depth` that is not ``None``.\n"
    "    Otherwise this will raise an ``RecursionError`` (or ``RuntimeError`` on\n"
    "    older Python versions) because each item in a ``UserString`` is itself a\n"
    "    ``UserString``, even if it has a length of 1! The builtin strings \n"
    "    (``str``, ``bytes``, ``unicode``) are special cased, but only the exact\n"
    "    types because subtypes might implement custom not-recursive ``__iter__``\n"
    "    methods. This means that these won't run into the infinite recursion,\n"
    "    but subclasses might.\n"
    "\n"
    "See for example::\n"
    "\n"
    "    >>> from collections import UserString\n"
    "    >>> list(deepflatten([1, 2, [1,2], UserString('abc')], depth=1))\n"
    "    [1, 2, 1, 2, 'a', 'b', 'c']\n"
    "    >>> list(deepflatten([1, 2, [1,2], UserString('abc')], ignore=UserString))\n"
    "    [1, 2, 1, 2, 'abc']\n"
    "\n"
    "This function is roughly (it's missing some of the complicated details \n"
    "and performance optimizations of the actual function) equivalent to this \n"
    "python function:\n"
    "\n"
    ".. code::\n"
    "\n"
    "    def deepflatten(iterable, depth=None, types=None, ignore=None):\n"
    "        if depth is None:\n"
    "            depth = float('inf')\n"
    "        if depth == -1:\n"
    "            yield iterable\n"
    "        else:\n"
    "            for x in iterable:\n"
    "                if ignore is not None and isinstance(x, ignore):\n"
    "                    yield x\n"
    "                if types is None:\n"
    "                    try:\n"
    "                        iter(x)\n"
    "                    except TypeError:\n"
    "                        yield x\n"
    "                    else:\n"
    "                        yield from deepflatten(x, depth - 1, types, ignore)\n"
    "                elif not isinstance(x, types):\n"
    "                    yield x\n"
    "                else:\n"
    "                    yield from deepflatten(x, depth - 1, types, ignore)\n");

static PyObject *
deepflatten_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "depth", "types", "ignore", NULL};
    PyIUObject_DeepFlatten *self;
    PyObject *iterable;
    PyObject *iterator = NULL;
    PyObject *types = NULL;
    PyObject *ignore = NULL;
    Py_ssize_t depth = -1;
    Py_ssize_t i;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|nOO:deepflatten", kwlist,
                                     &iterable, &depth, &types, &ignore)) {
        return NULL;
    }
    self = (PyIUObject_DeepFlatten *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    /* Create a list of size "depth+1" or if depth was not given use 3 for
       a start. Fill all entries with None except for the first which should
       be the iterator over the iterable. */
    self->iteratorlist = PyList_New(depth >= 0 ? depth + 1 : 3);
    if (self->iteratorlist == NULL) {
        Py_DECREF(self);
        Py_DECREF(iterator);
        return NULL;
    }
    PyList_SET_ITEM(self->iteratorlist, 0, iterator);
    iterator = NULL;
    for (i = 1; i < PyList_GET_SIZE(self->iteratorlist); i++) {
        Py_INCREF(Py_None);
        PyList_SET_ITEM(self->iteratorlist, i, Py_None);
    }
    self->types = types == Py_None ? NULL : types;
    Py_XINCREF(self->types);
    self->ignore = ignore == Py_None ? NULL : ignore;
    Py_XINCREF(self->ignore);
    self->depth = depth;
    self->currentdepth = 0;
    self->isstring = 0;
    return (PyObject *)self;
}

static void
deepflatten_dealloc(PyIUObject_DeepFlatten *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iteratorlist);
    Py_XDECREF(self->types);
    Py_XDECREF(self->ignore);
    Py_TYPE(self)->tp_free(self);
}

static int
deepflatten_traverse(PyIUObject_DeepFlatten *self, visitproc visit, void *arg) {
    Py_VISIT(self->iteratorlist);
    Py_VISIT(self->types);
    Py_VISIT(self->ignore);
    return 0;
}

static int
deepflatten_clear(PyIUObject_DeepFlatten *self) {
    Py_CLEAR(self->iteratorlist);
    Py_CLEAR(self->types);
    Py_CLEAR(self->ignore);
    return 0;
}

static PyObject *
deepflatten_next(PyIUObject_DeepFlatten *self) {
    PyObject *activeiterator;
    PyObject *item;
    PyObject *temp;
    int ok;

    if (self->currentdepth < 0) {
        return NULL;
    }

    /* TODO: This is likely a problem when using copy because currentdepth
             might be changed by the copy. However deepcopy should work as
             as expected.
       */
    activeiterator = PyList_GET_ITEM(self->iteratorlist, self->currentdepth);

    while (self->currentdepth >= 0) {
        item = Py_TYPE(activeiterator)->tp_iternext(activeiterator);

        /* The active iterator finished, remove it from the list and take
           up the iterator one level up. */
        if (item == NULL) {
            if (PyIU_ErrorOccurredClearStopIteration()) {
                return NULL;
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

        if (self->depth >= 0 && self->currentdepth >= self->depth) {
            /* If the currentdepth exceeds the specified depth just return. */
            return item;
        } else if (self->isstring) {
            /* If we're in a built-in string/bytes or unicode simply return. */
            return item;
        } else if (self->ignore &&
                   (ok = PyObject_IsInstance(item, self->ignore))) {
            /* First check if the item is an instance of the ignored types, if
               it is, then simply return it. */
            if (ok == 1) {
                return item;
            }
            Py_DECREF(item);
            return NULL;
        } else if (self->types) {
            /* If types is given then check if it's an instance thereof and if
               so replace activeiterator, otherwise return the item. */
            if ((ok = PyObject_IsInstance(item, self->types))) {
                if (ok == -1) {
                    Py_DECREF(item);
                    return NULL;
                }
                /* Check if it's a builtin-string-type and if so set
                   "isstring". Check for the exact type because sub types might
                   have custom __iter__ methods, better not to interfere with
                   these. */
                if (PyBytes_CheckExact(item) || PyUnicode_CheckExact(item)) {
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
        } else {
            /* If no types are given just try to convert it to an iterator and
               if that succeeds replaces activeiterator, otherwise return item. */
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
                if (PyBytes_CheckExact(item) || PyUnicode_CheckExact(item)) {
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
            PyErr_SetString(PyExc_RecursionError,
                            "`deepflatten` reached maximum recursion depth.");
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

static PyObject *
deepflatten_reduce(PyIUObject_DeepFlatten *self, PyObject *Py_UNUSED(args)) {
    PyObject *res;
    /* We need to copy the iteratorlist in case someone grabs it. This could
       lead to segmentation faults if the list is partially deleted, the next
       call to "next" could try to access an out-of-bounds index.
       */
    PyObject *itlist = PyList_GetSlice(self->iteratorlist, 0, PY_SSIZE_T_MAX);
    res = Py_BuildValue("O(OnOO)(Oni)", Py_TYPE(self),
                        PyList_GET_ITEM(self->iteratorlist, 0), /* stub */
                        self->depth,
                        self->types ? self->types : Py_None,
                        self->ignore ? self->ignore : Py_None,
                        itlist,
                        self->currentdepth,
                        self->isstring);
    Py_DECREF(itlist);
    return res;
}

static PyObject *
deepflatten_setstate(PyIUObject_DeepFlatten *self, PyObject *state) {
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
        for (i = 0; i <= currentdepth; i++) {
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

    /* No need to incref iteratorlist, we copied it. */
    Py_XSETREF(self->iteratorlist, iteratorlist);
    self->currentdepth = currentdepth;
    self->isstring = isstring;
    Py_RETURN_NONE;
}

static PyMethodDef deepflatten_methods[] = {
    {
        "__reduce__",                    /* ml_name */
        (PyCFunction)deepflatten_reduce, /* ml_meth */
        METH_NOARGS,                     /* ml_flags */
        PYIU_reduce_doc                  /* ml_doc */
    },
    {
        "__setstate__",                    /* ml_name */
        (PyCFunction)deepflatten_setstate, /* ml_meth */
        METH_O,                            /* ml_flags */
        PYIU_setstate_doc                  /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef deepflatten_memberlist[] = {
    {
        "types",                                 /* name */
        T_OBJECT,                                /* type */
        offsetof(PyIUObject_DeepFlatten, types), /* offset */
        READONLY,                                /* flags */
        deepflatten_prop_types_doc               /* doc */
    },
    {
        "ignore",                                 /* name */
        T_OBJECT,                                 /* type */
        offsetof(PyIUObject_DeepFlatten, ignore), /* offset */
        READONLY,                                 /* flags */
        deepflatten_prop_ignore_doc               /* doc */
    },
    {
        "depth",                                 /* name */
        T_PYSSIZET,                              /* type */
        offsetof(PyIUObject_DeepFlatten, depth), /* offset */
        READONLY,                                /* flags */
        deepflatten_prop_depth_doc               /* doc */
    },
    {
        "currentdepth",                                 /* name */
        T_PYSSIZET,                                     /* type */
        offsetof(PyIUObject_DeepFlatten, currentdepth), /* offset */
        READONLY,                                       /* flags */
        deepflatten_prop_currentdepth_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_DeepFlatten = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.deepflatten", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_DeepFlatten),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                  /* tp_itemsize */
    /* methods */
    (destructor)deepflatten_dealloc,       /* tp_dealloc */
    (printfunc)0,                          /* tp_print */
    (getattrfunc)0,                        /* tp_getattr */
    (setattrfunc)0,                        /* tp_setattr */
    0,                                     /* tp_reserved */
    (reprfunc)0,                           /* tp_repr */
    (PyNumberMethods *)0,                  /* tp_as_number */
    (PySequenceMethods *)0,                /* tp_as_sequence */
    (PyMappingMethods *)0,                 /* tp_as_mapping */
    (hashfunc)0,                           /* tp_hash */
    (ternaryfunc)0,                        /* tp_call */
    (reprfunc)0,                           /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc)0,                       /* tp_setattro */
    (PyBufferProcs *)0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,            /* tp_flags */
    (const char *)deepflatten_doc,      /* tp_doc */
    (traverseproc)deepflatten_traverse, /* tp_traverse */
    (inquiry)deepflatten_clear,         /* tp_clear */
    (richcmpfunc)0,                     /* tp_richcompare */
    (Py_ssize_t)0,                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,     /* tp_iter */
    (iternextfunc)deepflatten_next,     /* tp_iternext */
    deepflatten_methods,                /* tp_methods */
    deepflatten_memberlist,             /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    (descrgetfunc)0,                    /* tp_descr_get */
    (descrsetfunc)0,                    /* tp_descr_set */
    (Py_ssize_t)0,                      /* tp_dictoffset */
    (initproc)0,                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,     /* tp_alloc */
    (newfunc)deepflatten_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,          /* tp_free */
};
