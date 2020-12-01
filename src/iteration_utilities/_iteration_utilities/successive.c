/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "successive.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"

PyDoc_STRVAR(
    successive_prop_times_doc,
    "(:py:class:`int`) The number of successive items (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(
    successive_doc,
    "successive(iterable, times=2)\n"
    "--\n\n"
    "Like the recipe for pairwise but allows to get an arbitrary number\n"
    "of successive elements.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    Get the successive elements from this `iterable`.\n"
    "\n"
    "times : :py:class:`int`, optional\n"
    "    The number of successive elements.\n"
    "    Default is ``2``.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "successive_elements : generator\n"
    "    The successive elements as generator. Each element of the generator\n"
    "    is a tuple containing `times` successive elements.\n"
    "\n"
    "Examples\n"
    "--------\n"
    "Each item of the `iterable` is returned as ``tuple`` with `times` successive\n"
    "items::\n"
    "\n"
    "    >>> from iteration_utilities import successive\n"
    "    >>> list(successive(range(5)))\n"
    "    [(0, 1), (1, 2), (2, 3), (3, 4)]\n"
    "\n"
    "Varying the `times` can give you also 3 successive elements::\n"
    "\n"
    "    >>> list(successive(range(5), times=3))\n"
    "    [(0, 1, 2), (1, 2, 3), (2, 3, 4)]\n"
    "    >>> list(successive('Hello!', times=2))\n"
    "    [('H', 'e'), ('e', 'l'), ('l', 'l'), ('l', 'o'), ('o', '!')]\n");

static PyObject *
successive_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"iterable", "times", NULL};
    PyIUObject_Successive *self;
    PyObject *iterable;
    Py_ssize_t times = 2;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|n:successive", kwlist,
                                     &iterable, &times)) {
        return NULL;
    }
    if (times <= 0) {
        PyErr_Format(PyExc_ValueError,
                     "`times` argument for `successive` must be greater than 0.");
        return NULL;
    }
    self = (PyIUObject_Successive *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iterator = PyObject_GetIter(iterable);
    if (self->iterator == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->times = times;
    self->result = NULL;
    return (PyObject *)self;
}

static void
successive_dealloc(PyIUObject_Successive *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->result);
    Py_TYPE(self)->tp_free(self);
}

static int
successive_traverse(PyIUObject_Successive *self, visitproc visit, void *arg) {
    Py_VISIT(self->iterator);
    Py_VISIT(self->result);
    return 0;
}

static int
successive_clear(PyIUObject_Successive *self) {
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->result);
    return 0;
}

static PyObject *
successive_next(PyIUObject_Successive *self) {
    PyObject *result = self->result;
    PyObject *item;
    Py_ssize_t i;

    /* First call needs to create a tuple for the result. */
    if (result == NULL) {
        result = PyTuple_New(self->times);
        if (result == NULL) {
            return NULL;
        }

        for (i = 0; i < self->times; i++) {
            item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
            if (item == NULL) {
                Py_DECREF(result);
                return NULL;
            }
            PyTuple_SET_ITEM(result, i, item);
        }
        Py_INCREF(result);
        self->result = result;
        return result;
    }

    /* After the first element we can use the normal procedure. */
    item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
    if (item == NULL) {
        return NULL;
    }

    /* Recycle old tuple or create a new one. */
    if (PYIU_CPYTHON && (Py_REFCNT(result) == 1)) {
        /* Remove the first item of the result. */
        PyObject *temp = PyTuple_GET_ITEM(result, 0);
        PyIU_TupleRemove(result, 0, self->times);
        Py_XDECREF(temp);

        /* Insert the new item (at the end) and return it. */
        PyTuple_SET_ITEM(result, self->times - 1, item);
        Py_INCREF(result);
        return result;

    } else {
        PyObject *newresult = PyTuple_New(self->times);
        if (newresult == NULL) {
            Py_DECREF(item);
            return NULL;
        }

        /* Shift all earlier items one index to the left. */
        for (i = 1; i < self->times; i++) {
            PyObject *olditem = PyTuple_GET_ITEM(result, i);
            Py_INCREF(olditem);
            PyTuple_SET_ITEM(newresult, i - 1, olditem);
        }
        /* Insert the new item (at the end), then replace the saved result. */
        PyTuple_SET_ITEM(newresult, self->times - 1, item);
        Py_INCREF(newresult);
        Py_SETREF(self->result, newresult);
        return newresult;
    }
}

static PyObject *
successive_reduce(PyIUObject_Successive *self, PyObject *Py_UNUSED(args)) {
    /* Separate cases depending on the status of "result". We use and modify
       it in next. It is copied in next when the refcount isn't 1, so we
       don't need to copy it for reduce. However using "reduce" a lot will
       definitely slow the function down. But it does not matter if the slowdown
       is in "next" or "reduce". :)
       */
    if (self->result == NULL) {
        return Py_BuildValue("O(On)", Py_TYPE(self),
                             self->iterator,
                             self->times);
    } else {
        return Py_BuildValue("O(On)(O)", Py_TYPE(self),
                             self->iterator,
                             self->times,
                             self->result);
    }
}

static PyObject *
successive_setstate(PyIUObject_Successive *self, PyObject *state) {
    PyObject *result;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "O:successive.__setstate__", &result)) {
        return NULL;
    }

    /* The result must be a tuple, otherwise we could risk segfaults (because
       "next" use PyTuple_GET_ITEM). It also needs to have the same size as
       "self->times" otherwise the for-loop in "next" could go beyond the
       tuple-size (again risking undefined behaviour).
       */
    if (!PyTuple_CheckExact(result)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple` instance as "
                     "first argument in the `state`, got %.200s.",
                     Py_TYPE(self)->tp_name, Py_TYPE(result)->tp_name);
        return NULL;
    }
    if (PyTuple_GET_SIZE(result) != self->times) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the first argument "
                     "in the `state`, satisfies `len(firstarg) == self->times`. "
                     "But `%zd != %zd`.",
                     Py_TYPE(self)->tp_name,
                     PyTuple_GET_SIZE(result),
                     self->times);
        return NULL;
    }

    /* No need to  copy the "result". If it has a refcount different from
       1 it will be copied in "next" before it is mutated.
       */
    Py_XINCREF(result);
    Py_XSETREF(self->result, result);
    Py_RETURN_NONE;
}

static PyObject *
successive_lengthhint(PyIUObject_Successive *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }
    /* If we are already started we will have one result for every remaining
       item in the iterator. However if we haven't started we have less than
       that. We need "self->times" objects to fill the first return value, so
       we need to subtract "self->times - 1" from the length. In case the
       "times > len" the function won't return anything so we can set the
       length simply to 0.
       */
    if (self->result == NULL) {
        if (self->times > len) {
            len = 0;
        } else {
            len -= (self->times - 1);
        }
    }

    return PyLong_FromSsize_t(len);
}

static PyMethodDef successive_methods[] = {
    {
        "__length_hint__",                  /* ml_name */
        (PyCFunction)successive_lengthhint, /* ml_meth */
        METH_NOARGS,                        /* ml_flags */
        PYIU_lenhint_doc                    /* ml_doc */
    },
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)successive_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {
        "__setstate__",                   /* ml_name */
        (PyCFunction)successive_setstate, /* ml_meth */
        METH_O,                           /* ml_flags */
        PYIU_setstate_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

static PyMemberDef successive_memberlist[] = {
    {
        "times",                                /* name */
        T_PYSSIZET,                             /* type */
        offsetof(PyIUObject_Successive, times), /* offset */
        READONLY,                               /* flags */
        successive_prop_times_doc               /* doc */
    },
    {NULL} /* sentinel */
};

PyTypeObject PyIUType_Successive = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.successive", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Successive),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                 /* tp_itemsize */
    /* methods */
    (destructor)successive_dealloc,        /* tp_dealloc */
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
    (const char *)successive_doc,      /* tp_doc */
    (traverseproc)successive_traverse, /* tp_traverse */
    (inquiry)successive_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)successive_next,     /* tp_iternext */
    successive_methods,                /* tp_methods */
    successive_memberlist,             /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)PyType_GenericAlloc,    /* tp_alloc */
    (newfunc)successive_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
