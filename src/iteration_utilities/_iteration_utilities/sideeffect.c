/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "sideeffect.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"

PyDoc_STRVAR(
    sideeffects_prop_func_doc,
    "(callable) The function that is called by `sideeffects` (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    sideeffects_prop_times_doc,
    "(:py:class:`int`) A counter indicating after how many items the `func` "
    "is called (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    sideeffects_prop_count_doc,
    "(:py:class:`int`) The current count for the next `func` call (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    sideeffects_doc,
    "sideeffects(iterable, func, times=0)\n"
    "--\n\n"
    "Does a normal iteration over `iterable` and only uses `func` each `times` \n"
    "items for it's side effects.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    `Iterable` containing the elements.\n"
    "\n"
    "func : callable\n"
    "    Function that is called for the side effects.\n"
    "\n"
    "times : :py:class:`int`, optional\n"
    "    Call the function each `times` items with the last `times` items. \n"
    "    If ``0`` the argument for `func` will be the item itself. For any \n"
    "    number greater than zero the argument will be a tuple.\n"
    "    Default is ``0``.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "iterator : generator\n"
    "    A normal iterator over `iterable`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "A simple example::\n"
    "\n"
    "    >>> from iteration_utilities import sideeffects\n"
    "    >>> list(sideeffects([1,2,3,4], print))\n"
    "    1\n"
    "    2\n"
    "    3\n"
    "    4\n"
    "    [1, 2, 3, 4]\n"
    "    >>> list(sideeffects([1,2,3,4,5], print, 2))\n"
    "    (1, 2)\n"
    "    (3, 4)\n"
    "    (5,)\n"
    "    [1, 2, 3, 4, 5]\n");

static PyObject *
sideeffects_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "func", "times", NULL};
    PyIUObject_Sideeffects *self;
    PyObject *iterable;
    PyObject *func;
    Py_ssize_t times = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|n:sideeffects", kwlist,
                                     &iterable, &func, &times)) {
        return NULL;
    }
    self = (PyIUObject_Sideeffects *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->times = times <= 0 ? 0 : times;
    if (times <= 0) { /* negative values will be interpreted as zero... */
        self->collected = NULL;
    } else {
        self->collected = PyTuple_New(self->times);
        if (self->collected == NULL) {
            Py_DECREF(self);
        }
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_XDECREF(self);
        return NULL;
    }
    Py_INCREF(func);
    self->func = func;
    self->count = 0;
    return (PyObject *)self;
}

static void
sideeffects_dealloc(PyIUObject_Sideeffects *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->func);
    Py_XDECREF(self->collected);
    Py_TYPE(self)->tp_free(self);
}

static int
sideeffects_traverse(PyIUObject_Sideeffects *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->func);
    Py_VISIT(self->collected);
    return 0;
}

static int
sideeffects_clear(PyIUObject_Sideeffects *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->func);
    Py_CLEAR(self->collected);
    return 0;
}

static PyObject *
sideeffects_next(PyIUObject_Sideeffects *self) {
    PyObject *item;
    PyObject *temp = NULL;
    PyObject *tmptuple = NULL;
    Py_ssize_t i;

    item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
    if (item == NULL) {
        /* We don't expect that the sideeffect function is called when
           an exception other than StopIteration is raised by the iterator so
           exit early in that case. */
        if (PyIU_ErrorOccurredClearStopIteration()) {
            return NULL;
        }
        if (self->count != 0) {
            /* Call function with the remaining items. */
            tmptuple = PyIU_TupleGetSlice(self->collected, self->count);
            if (tmptuple == NULL) {
                return NULL;
            }
            temp = PyIU_CallWithOneArgument(self->func, tmptuple);
            Py_DECREF(tmptuple);
            if (temp != NULL) {
                Py_DECREF(temp);
            }
            /* The case where temp == NULL is handled by the following
               "return NULL" anyway so it does not need to be a special case
               here. */
        }
        return NULL;
    }

    if (self->times == 0) {
        /* Always call the function if times == 0 */
        temp = PyIU_CallWithOneArgument(self->func, item);
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
            temp = PyIU_CallWithOneArgument(self->func, self->collected);
            if (temp == NULL) {
                goto Fail;
            } else {
                Py_DECREF(temp);
            }
            /* Try to reuse collected if possible. In this case the "funcargs"
               and the class own a reference to collected so we can only
               reuse the collected tuple IF nobody except the instance owns
               the "funcargs". This can be up to 40-50% faster for small
               "times" values. Even for relatively bigger ones this is still
               10% faster.
               To avoid needing to decrement the values in the tuple while
               iterating these are simply set to NULL.
               */
            if (PYIU_CPYTHON && (Py_REFCNT(self->collected) == 1)) {
                for (i = 0; i < self->times; i++) {
                    temp = PyTuple_GET_ITEM(self->collected, i);
                    PyTuple_SET_ITEM(self->collected, i, NULL);
                    Py_DECREF(temp);
                }
            } else {
                PyObject *new_collected = PyTuple_New(self->times);
                if (new_collected == NULL) {
                    goto Fail;
                }
                Py_SETREF(self->collected, new_collected);
            }
        }
    }

    return item;

Fail:
    Py_XDECREF(item);
    return NULL;
}

static PyObject *
sideeffects_reduce(PyIUObject_Sideeffects *self, PyObject *Py_UNUSED(args)) {
    PyObject *collected;
    PyObject *res;

    /* There are several issues that prevent from simply wrapping the
       attributes.
       */
    if (self->collected == NULL) {
        /* When "collected" is NULL we wrap it as None, and no further
           processing is needed.
           */
        Py_INCREF(Py_None);
        collected = Py_None;
    } else {
        /* When we have "collected" then it's a tuple that may contain NULLs.
           The Python interpreter does not like NULLs so these must be
           replaced by some fillvalue (in this case None). However we modify
           the tuple inside the "next" method so if someone called "reduce"
           that person could **see** the "collected" tuple change. That must
           be avoided so we MUST return a copy of the "collected" tuple.
           */
        Py_ssize_t i;
        Py_ssize_t collected_size = PyTuple_GET_SIZE(self->collected);
        collected = PyTuple_New(collected_size);
        if (collected == NULL) {
            return NULL;
        }
        for (i = 0; i < collected_size; i++) {
            PyObject *tmp = PyTuple_GET_ITEM(self->collected, i);
            if (tmp == NULL) {
                tmp = Py_None;
            }
            Py_INCREF(tmp);
            PyTuple_SET_ITEM(collected, i, tmp);
        }
    }

    res = Py_BuildValue("O(OOn)(nO)", Py_TYPE(self),
                        self->iterator,
                        self->func,
                        self->times,
                        self->count,
                        collected);
    Py_DECREF(collected);
    return res;
}

static PyObject *
sideeffects_setstate(PyIUObject_Sideeffects *self, PyObject *state) {
    Py_ssize_t count;
    PyObject *collected;
    PyObject *newcollected = NULL;
    Py_ssize_t collected_size = 0;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "nO:sideeffects.__setstate__",
                          &count, &collected)) {
        return NULL;
    }

    /* The "collected" argument should be a tuple (because we use
       PyTuple_GET_ITEM and PyTuple_SET_ITEM and thus would risk segmentation
       faults if we don't check that it's a tuple) or None.
       */
    if (PyTuple_CheckExact(collected)) {
        /* The class itself has a "times" attribute, if that attribute is zero
           we do not need a "collected" tuple, it should have been "None".
           */
        if (self->times == 0) {
            PyErr_Format(PyExc_TypeError,
                         "`%.200s.__setstate__` expected `None` as second "
                         "argument in the `state` when `self->times == 0`, "
                         "got %.200s.",
                         Py_TYPE(self)->tp_name, Py_TYPE(collected)->tp_name);
            return NULL;
        }
        /* The "count" must not be negative or bigger/equal to the size of
           the "collected" tuple. Otherwise we would access indices that are
           out of bounds for the tuple in "next".
           */
        collected_size = PyTuple_GET_SIZE(collected);
        if (count < 0 || count >= collected_size) {
            PyErr_Format(PyExc_ValueError,
                         "`%.200s.__setstate__` expected that the first "
                         "argument in the `state` (%zd) is not negative and "
                         "smaller than the length of the second argument "
                         "(%zd).",
                         Py_TYPE(self)->tp_name, count, collected_size);
            return NULL;
        }
        /* The length of the "collected" tuple must also be equal to the
           "self->times" attribute.
           */
        if (self->times != collected_size) {
            PyErr_Format(PyExc_ValueError,
                         "`%.200s.__setstate__` expected that the second "
                         "argument in the `state` has a length (%zd) "
                         "equal to the `self->times` (%zd) attribute.",
                         Py_TYPE(self)->tp_name, collected_size, self->times);
            return NULL;
        }
    } else if (collected == Py_None) {
        /* We only expect None if self->times and count is zero. */
        if (count != 0 || self->times != 0) {
            PyErr_Format(PyExc_TypeError,
                         "`%.200s.__setstate__` expected a `tuple` as second "
                         "argument in the `state` when `self->times != 0` or "
                         "the first argument in the `state` is not zero, "
                         "got None",
                         Py_TYPE(self)->tp_name);
            return NULL;
        }
    } else {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple` or `None` as "
                     "second argument in the `state`, got %.200s",
                     Py_TYPE(self)->tp_name, Py_TYPE(collected)->tp_name);
        return NULL;
    }

    /* In any case we need to process the "collected" value. In case it is
       "None" we simply set it to NULL. However if it's not None then it's
       a tuple. We process the tuple in the "next" function but it's possible
       that someone still holds a reference to the tuple he passed in. So to
       make sure that we don't mutate tuples that are in use elsewhere we
       create a new tuple here. That also has the additional advantage that
       we can leave the values with index below "count" as NULL. The "next"
       method assumes that it doesn't have to decrement items that it sets so
       this makes sure we don't create a memory leak there.
       */
    newcollected = NULL;
    if (collected != Py_None) {
        Py_ssize_t i;
        newcollected = PyTuple_New(collected_size);
        if (newcollected == NULL) {
            return NULL;
        }
        for (i = 0; i < count; i++) {
            PyObject *tmp = PyTuple_GET_ITEM(collected, i);
            Py_INCREF(tmp);
            PyTuple_SET_ITEM(newcollected, i, tmp);
        }
    }

    self->count = count;

    /* We already created a new tuple for "collected" or it's None so no need
       to increment the reference count here.
       */
    Py_XSETREF(self->collected, newcollected);

    Py_RETURN_NONE;
}

static PyObject *
sideeffects_lengthhint(PyIUObject_Sideeffects *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }
    return PyLong_FromSsize_t(len);
}

static PyMethodDef sideeffects_methods[] = {
    {
        "__length_hint__",                   /* ml_name */
        (PyCFunction)sideeffects_lengthhint, /* ml_meth */
        METH_NOARGS,                         /* ml_flags */
        PYIU_lenhint_doc                     /* ml_doc */
    },
    {
        "__reduce__",                    /* ml_name */
        (PyCFunction)sideeffects_reduce, /* ml_meth */
        METH_NOARGS,                     /* ml_flags */
        PYIU_reduce_doc                  /* ml_doc */
    },
    {
        "__setstate__",                    /* ml_name */
        (PyCFunction)sideeffects_setstate, /* ml_meth */
        METH_O,                            /* ml_flags */
        PYIU_setstate_doc                  /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef sideeffects_memberlist[] = {
    {
        "func",                                 /* name */
        T_OBJECT,                               /* type */
        offsetof(PyIUObject_Sideeffects, func), /* offset */
        READONLY,                               /* flags */
        sideeffects_prop_func_doc               /* doc */
    },
    {
        "times",                                 /* name */
        T_PYSSIZET,                              /* type */
        offsetof(PyIUObject_Sideeffects, times), /* offset */
        READONLY,                                /* flags */
        sideeffects_prop_times_doc               /* doc */
    },
    {
        "count",                                 /* name */
        T_PYSSIZET,                              /* type */
        offsetof(PyIUObject_Sideeffects, count), /* offset */
        READONLY,                                /* flags */
        sideeffects_prop_count_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Sideeffects = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.sideeffects", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Sideeffects),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                  /* tp_itemsize */
    /* methods */
    (destructor)sideeffects_dealloc,       /* tp_dealloc */
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
    (const char *)sideeffects_doc,      /* tp_doc */
    (traverseproc)sideeffects_traverse, /* tp_traverse */
    (inquiry)sideeffects_clear,         /* tp_clear */
    (richcmpfunc)0,                     /* tp_richcompare */
    (Py_ssize_t)0,                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,     /* tp_iter */
    (iternextfunc)sideeffects_next,     /* tp_iternext */
    sideeffects_methods,                /* tp_methods */
    sideeffects_memberlist,             /* tp_members */
    0,                                  /* tp_getset */
    0,                                  /* tp_base */
    0,                                  /* tp_dict */
    (descrgetfunc)0,                    /* tp_descr_get */
    (descrsetfunc)0,                    /* tp_descr_set */
    (Py_ssize_t)0,                      /* tp_dictoffset */
    (initproc)0,                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,     /* tp_alloc */
    (newfunc)sideeffects_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,          /* tp_free */
};
