/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "uniqueever.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"
#include "seen.h"

PyDoc_STRVAR(
    uniqueever_prop_seen_doc,
    "(:py:class:`~iteration_utilities.Seen`) Already seen values (readonly).");

PyDoc_STRVAR(
    uniqueever_prop_key_doc,
    "(callable or None) The key function (readonly).");

PyDoc_STRVAR(
    uniqueever_doc,
    "unique_everseen(iterable, key=None)\n"
    "--\n\n"
    "Find unique elements, preserving their order. Remembers all elements ever seen.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    `Iterable` containing the elements.\n"
    "\n"
    "key : callable, optional\n"
    "    If given it must be a callable taking one argument and this\n"
    "    callable is applied to the value before checking if it was seen yet.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "iterable : generator\n"
    "    An iterable containing all unique values ever seen in the `iterable`.\n"
    "\n"
    "Notes\n"
    "-----\n"
    "The items in the `iterable` should implement equality.\n"
    "\n"
    "If the items are hashable the function is much faster.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "Some simple examples::\n"
    "\n"
    "    >>> from iteration_utilities import unique_everseen\n"
    "    >>> list(unique_everseen('AAAABBBCCDAABBB'))\n"
    "    ['A', 'B', 'C', 'D']\n"
    "    \n"
    "    >>> list(unique_everseen('ABBCcAD', str.lower))\n"
    "    ['A', 'B', 'C', 'D']\n"
    "    \n"
    "Even unhashable values can be processed, like `list`::\n"
    "\n"
    "    >>> list(unique_everseen([[1, 2], [1, 1], [1, 2]]))\n"
    "    [[1, 2], [1, 1]]\n"
    "    \n"
    "However using ``key=tuple`` (to make them hashable) will be faster::\n"
    "\n"
    "    >>> list(unique_everseen([[1, 2], [1, 1], [1, 2]], key=tuple))\n"
    "    [[1, 2], [1, 1]]\n"
    "    \n"
    "One can access the already seen values by accessing the `seen` attribute.\n");

/******************************************************************************
 *
 * IMPORTANT NOTE (Implementation):
 *
 * This function is almost identical to "duplicates", so any changes
 * or bugfixes should also be implemented there!!!
 *
 *****************************************************************************/

static PyObject *
uniqueever_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_UniqueEver *self;
    PyObject *iterable;
    PyObject *key = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_everseen", kwlist,
                                     &iterable, &key)) {
        return NULL;
    }
    self = (PyIUObject_UniqueEver *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->seen = PyIUSeen_New();
    if (self->seen == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->key = key == Py_None ? NULL : key;
    Py_XINCREF(self->key);
    return (PyObject *)self;
}

static void
uniqueever_dealloc(PyIUObject_UniqueEver *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->key);
    Py_XDECREF(self->seen);
    Py_TYPE(self)->tp_free(self);
}

static int
uniqueever_traverse(PyIUObject_UniqueEver *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->key);
    Py_VISIT(self->seen);
    return 0;
}

static int
uniqueever_clear(PyIUObject_UniqueEver *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->key);
    Py_CLEAR(self->seen);
    return 0;
}

static PyObject *
uniqueever_next(PyIUObject_UniqueEver *self) {
    PyObject *item;

    while ((item = Py_TYPE(self->iterator)->tp_iternext(self->iterator))) {
        PyObject *temp;
        int ok;

        /* Use the item if key is not given, otherwise apply the key. */
        if (self->key == NULL) {
            Py_INCREF(item);
            temp = item;
        } else {
            temp = PyIU_CallWithOneArgument(self->key, item);
            if (temp == NULL) {
                Py_DECREF(item);
                return NULL;
            }
        }

        ok = PyIUSeen_ContainsAdd(self->seen, temp);
        Py_DECREF(temp);
        if (ok == 0) {
            return item;
        }
        Py_DECREF(item);
        if (ok == -1) {
            return NULL;
        }
    }

    return NULL;
}

static PyObject *
uniqueever_reduce(PyIUObject_UniqueEver *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OO)(O)", Py_TYPE(self),
                         self->iterator,
                         self->key ? self->key : Py_None,
                         self->seen);
}

static PyObject *
uniqueever_setstate(PyIUObject_UniqueEver *self, PyObject *state) {
    PyObject *seen;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "O:unique_everseen.__setstate__", &seen)) {
        return NULL;
    }

    /* object passed in must be an instance of Seen. Otherwise the function
       calls could result in an segmentation fault.
       */
    if (!PyIU_IsTypeExact(seen, &PyIUType_Seen)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `Seen` instance as "
                     "first argument in the `state`, got %.200s.",
                     Py_TYPE(self)->tp_name, Py_TYPE(seen)->tp_name);
        return NULL;
    }

    Py_INCREF(seen);
    Py_XSETREF(self->seen, seen);
    Py_RETURN_NONE;
}

static PyMethodDef uniqueever_methods[] = {
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)uniqueever_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {
        "__setstate__",                   /* ml_name */
        (PyCFunction)uniqueever_setstate, /* ml_meth */
        METH_O,                           /* ml_flags */
        PYIU_setstate_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef uniqueever_memberlist[] = {
    {
        "seen",                                /* name */
        T_OBJECT,                              /* type */
        offsetof(PyIUObject_UniqueEver, seen), /* offset */
        READONLY,                              /* flags */
        uniqueever_prop_seen_doc               /* doc */
    },
    {
        "key",                                /* name */
        T_OBJECT,                             /* type */
        offsetof(PyIUObject_UniqueEver, key), /* offset */
        READONLY,                             /* flags */
        uniqueever_prop_key_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_UniqueEver = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.unique_everseen", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_UniqueEver),                                          /* tp_basicsize */
    (Py_ssize_t)0,                                                                      /* tp_itemsize */
    /* methods */
    (destructor)uniqueever_dealloc,        /* tp_dealloc */
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
        Py_TPFLAGS_BASETYPE,           /* tp_flags */
    (const char *)uniqueever_doc,      /* tp_doc */
    (traverseproc)uniqueever_traverse, /* tp_traverse */
    (inquiry)uniqueever_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)uniqueever_next,     /* tp_iternext */
    uniqueever_methods,                /* tp_methods */
    uniqueever_memberlist,             /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)0,                      /* tp_alloc */
    (newfunc)uniqueever_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
