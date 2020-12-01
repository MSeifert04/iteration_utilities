/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "duplicates.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"
#include "seen.h"

PyDoc_STRVAR(
    duplicates_prop_seen_doc,
    "(:py:class:`~iteration_utilities.Seen`) Already seen values (readonly).");
PyDoc_STRVAR(
    duplicates_prop_key_doc,
    "(callable or `None`) The key function (readonly).");

PyDoc_STRVAR(
    duplicates_doc,
    "duplicates(iterable, key=None)\n"
    "--\n\n"
    "Return only duplicate entries, remembers all items ever seen.\n"
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
    "    An iterable containing all duplicates values of the `iterable`.\n"
    "\n"
    "Notes\n"
    "-----\n"
    "The items in the `iterable` should implement equality.\n"
    "\n"
    "If the items are hashable the function is much faster.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "Multiple duplicates will be kept::\n"
    "\n"
    "    >>> from iteration_utilities import duplicates\n"
    "    >>> list(duplicates('AABBCCDA'))\n"
    "    ['A', 'B', 'C', 'A']\n"
    "\n"
    "    >>> list(duplicates('ABBCcAD', str.lower))\n"
    "    ['B', 'c', 'A']\n"
    "\n"
    "To get each duplicate only once this can be combined with \n"
    ":py:func:`~iteration_utilities.unique_everseen`::\n"
    "\n"
    "    >>> from iteration_utilities import unique_everseen\n"
    "    >>> list(unique_everseen(duplicates('AABBCCDA')))\n"
    "    ['A', 'B', 'C']\n");

/******************************************************************************
 *
 * IMPORTANT NOTE (Implementation):
 *
 * This function is almost identical to "unique_everseen", so any changes
 * or bugfixes should also be implemented there!!!
 *
 *****************************************************************************/

static PyObject *
duplicates_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_Duplicates *self;
    PyObject *iterable;
    PyObject *key = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:duplicates", kwlist,
                                     &iterable, &key)) {
        return NULL;
    }
    self = (PyIUObject_Duplicates *)type->tp_alloc(type, 0);
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
duplicates_dealloc(PyIUObject_Duplicates *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->key);
    Py_XDECREF(self->seen);
    Py_TYPE(self)->tp_free(self);
}

static int
duplicates_traverse(PyIUObject_Duplicates *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->key);
    Py_VISIT(self->seen);
    return 0;
}

static int
duplicates_clear(PyIUObject_Duplicates *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->key);
    Py_CLEAR(self->seen);
    return 0;
}

static PyObject *
duplicates_next(PyIUObject_Duplicates *self) {
    PyObject *item = NULL;

    while ((item = Py_TYPE(self->iterator)->tp_iternext(self->iterator))) {
        PyObject *val;
        int ok;
        /* Use the item if key is not given, otherwise apply the key. */
        if (self->key == NULL) {
            Py_INCREF(item);
            val = item;
        } else {
            val = PyIU_CallWithOneArgument(self->key, item);
            if (val == NULL) {
                Py_DECREF(item);
                return NULL;
            }
        }

        /* Check if the item is in seen.  */
        ok = PyIUSeen_ContainsAdd(self->seen, val);
        Py_DECREF(val);
        if (ok == 1) {
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
duplicates_reduce(PyIUObject_Duplicates *self, PyObject *Py_UNUSED(args)) {
    return Py_BuildValue("O(OO)(O)", Py_TYPE(self),
                         self->iterator,
                         self->key ? self->key : Py_None,
                         self->seen);
}

static PyObject *
duplicates_setstate(PyIUObject_Duplicates *self, PyObject *state) {
    PyObject *seen;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "O:duplicates.__setstate__", &seen)) {
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

static PyMethodDef duplicates_methods[] = {
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)duplicates_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {
        "__setstate__",                   /* ml_name */
        (PyCFunction)duplicates_setstate, /* ml_meth */
        METH_O,                           /* ml_flags */
        PYIU_setstate_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef duplicates_memberlist[] = {
    {
        "seen",                                /* name */
        T_OBJECT,                              /* type */
        offsetof(PyIUObject_Duplicates, seen), /* offset */
        READONLY,                              /* flags */
        duplicates_prop_seen_doc               /* doc */
    },
    {
        "key",                                /* name */
        T_OBJECT,                             /* type */
        offsetof(PyIUObject_Duplicates, key), /* offset */
        READONLY,                             /* flags */
        duplicates_prop_key_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Duplicates = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.duplicates", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Duplicates),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                 /* tp_itemsize */
    /* methods */
    (destructor)duplicates_dealloc,        /* tp_dealloc */
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
    (const char *)duplicates_doc,      /* tp_doc */
    (traverseproc)duplicates_traverse, /* tp_traverse */
    (inquiry)duplicates_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)duplicates_next,     /* tp_iternext */
    duplicates_methods,                /* tp_methods */
    duplicates_memberlist,             /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)0,                      /* tp_alloc */
    (newfunc)duplicates_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
