/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "uniquejust.h"
#include <structmember.h>
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"

PyDoc_STRVAR(
    uniquejust_prop_key_doc,
    "(callable or None) The key function (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    uniquejust_prop_lastseen_doc,
    "(any type) The last seen item (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    uniquejust_doc,
    "unique_justseen(iterable, key=None)\n"
    "--\n\n"
    "List unique elements, preserving order. Remember only the element just seen.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    `Iterable` to check.\n"
    "\n"
    "key : callable or None, optional\n"
    "    If ``None`` the values are taken as they are. If it's a callable the\n"
    "    callable is applied to the value before comparing it.\n"
    "    Default is ``None``.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "iterable : generator\n"
    "    An iterable containing all unique values just seen in the `iterable`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    ">>> from iteration_utilities import unique_justseen\n"
    ">>> list(unique_justseen('AAAABBBCCDAABBB'))\n"
    "['A', 'B', 'C', 'D', 'A', 'B']\n"
    "\n"
    ">>> list(unique_justseen('ABBCcAD', str.lower))\n"
    "['A', 'B', 'C', 'A', 'D']\n");

static PyObject *
uniquejust_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "key", NULL};
    PyIUObject_UniqueJust *self;
    PyObject *iterable;
    PyObject *keyfunc = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:unique_justseen", kwlist,
                                     &iterable, &keyfunc)) {
        return NULL;
    }
    self = (PyIUObject_UniqueJust *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->keyfunc = keyfunc == Py_None ? NULL : keyfunc;
    Py_XINCREF(self->keyfunc);
    self->lastitem = NULL;
    return (PyObject *)self;
}

static void
uniquejust_dealloc(PyIUObject_UniqueJust *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->keyfunc);
    Py_XDECREF(self->lastitem);
    Py_TYPE(self)->tp_free(self);
}

static int
uniquejust_traverse(PyIUObject_UniqueJust *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->keyfunc);
    Py_VISIT(self->lastitem);
    return 0;
}

static int
uniquejust_clear(PyIUObject_UniqueJust *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->keyfunc);
    Py_CLEAR(self->lastitem);
    return 0;
}

static PyObject *
uniquejust_next(PyIUObject_UniqueJust *self) {
    PyObject *item;

    while ((item = Py_TYPE(self->iterator)->tp_iternext(self->iterator))) {
        PyObject *val;
        int ok;

        /* Apply keyfunc or use the original */
        if (self->keyfunc == NULL) {
            Py_INCREF(item);
            val = item;
        } else {
            val = PyIU_CallWithOneArgument(self->keyfunc, item);
            if (val == NULL) {
                Py_DECREF(item);
                return NULL;
            }
        }

        /* If no lastitem set it to the current and simply return the item. */
        if (self->lastitem == NULL) {
            self->lastitem = val;
            return item;
        }

        /* Otherwise compare it with the last item and only return it if it
           differs. */
        ok = PyObject_RichCompareBool(val, self->lastitem, Py_EQ);

        if (ok == 0) {
            Py_SETREF(self->lastitem, val);
            return item;
        }
        Py_DECREF(val);
        Py_DECREF(item);
        if (ok < 0) {
            return NULL;
        }
    }
    return NULL;
}

static PyObject *
uniquejust_reduce(PyIUObject_UniqueJust *self, PyObject *Py_UNUSED(args)) {
    /* Separate cases depending on lastitem == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a last item or
       that the last item was None.
       Better to make an "if" than to introduce another variable depending on
       lastitem == NULL.
       */
    if (self->lastitem != NULL) {
        return Py_BuildValue("O(OO)(O)", Py_TYPE(self),
                             self->iterator,
                             self->keyfunc ? self->keyfunc : Py_None,
                             self->lastitem);
    } else {
        return Py_BuildValue("O(OO)", Py_TYPE(self),
                             self->iterator,
                             self->keyfunc ? self->keyfunc : Py_None);
    }
}

static PyObject *
uniquejust_setstate(PyIUObject_UniqueJust *self, PyObject *state) {
    PyObject *lastitem;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }
    if (!PyArg_ParseTuple(state, "O:unique_justseen.__setstate__", &lastitem)) {
        return NULL;
    }
    /* No need to check the type of "lastitem" because any python object is
       valid.
       */
    Py_INCREF(lastitem);
    Py_XSETREF(self->lastitem, lastitem);
    Py_RETURN_NONE;
}

static PyMethodDef uniquejust_methods[] = {
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)uniquejust_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {
        "__setstate__",                   /* ml_name */
        (PyCFunction)uniquejust_setstate, /* ml_meth */
        METH_O,                           /* ml_flags */
        PYIU_setstate_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef uniquejust_memberlist[] = {
    {
        "key",                                    /* name */
        T_OBJECT,                                 /* type */
        offsetof(PyIUObject_UniqueJust, keyfunc), /* offset */
        READONLY,                                 /* flags */
        uniquejust_prop_key_doc                   /* doc */
    },
    {
        "lastseen",                                /* name */
        T_OBJECT_EX,                               /* type */
        offsetof(PyIUObject_UniqueJust, lastitem), /* offset */
        READONLY,                                  /* flags */
        uniquejust_prop_lastseen_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_UniqueJust = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.unique_justseen", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_UniqueJust),                                          /* tp_basicsize */
    (Py_ssize_t)0,                                                                      /* tp_itemsize */
    /* methods */
    (destructor)uniquejust_dealloc,        /* tp_dealloc */
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
    (const char *)uniquejust_doc,      /* tp_doc */
    (traverseproc)uniquejust_traverse, /* tp_traverse */
    (inquiry)uniquejust_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)uniquejust_next,     /* tp_iternext */
    uniquejust_methods,                /* tp_methods */
    uniquejust_memberlist,             /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)0,                      /* tp_alloc */
    (newfunc)uniquejust_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
