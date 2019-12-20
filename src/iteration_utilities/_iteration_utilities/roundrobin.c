/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "roundrobin.h"
#include <structmember.h>
#include "docs_lengthhint.h"
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "helper.h"

PyDoc_STRVAR(
    roundrobin_doc,
    "roundrobin(*iterables)\n"
    "--\n\n"
    "Round-Robin implementation ([0]_).\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterables : iterable\n"
    "    `Iterables` to combine using the round-robin. Any amount of iterables\n"
    "    are supported.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "roundrobin : generator\n"
    "    Iterable filled with the values of the `iterables`.\n"
    "\n"
    "Examples\n"
    "--------\n"
    ">>> from iteration_utilities import roundrobin\n"
    ">>> list(roundrobin('ABC', 'D', 'EF'))\n"
    "['A', 'D', 'E', 'B', 'F', 'C']\n"
    "\n"
    "References\n"
    "----------\n"
    ".. [0] https://en.wikipedia.org/wiki/Round-robin_scheduling\n");

static PyObject *
roundrobin_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) {
    PyIUObject_Roundrobin *self;

    self = (PyIUObject_Roundrobin *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->iteratortuple = PyIU_CreateIteratorTuple(args);
    if (self->iteratortuple == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    self->numactive = PyTuple_GET_SIZE(args);
    self->active = 0;
    return (PyObject *)self;
}

static void
roundrobin_dealloc(PyIUObject_Roundrobin *self) {
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iteratortuple);
    Py_TYPE(self)->tp_free(self);
}

static int
roundrobin_traverse(PyIUObject_Roundrobin *self, visitproc visit, void *arg) {
    Py_VISIT(self->iteratortuple);
    return 0;
}

static int
roundrobin_clear(PyIUObject_Roundrobin *self) {
    Py_CLEAR(self->iteratortuple);
    return 0;
}

static PyObject *
roundrobin_next(PyIUObject_Roundrobin *self) {
    PyObject *iterator;
    PyObject *item;

    /* Stop if there is no active iterator left. */
    if (self->numactive == 0) {
        return NULL;
    }

    iterator = PyTuple_GET_ITEM(self->iteratortuple, self->active);
    while ((item = Py_TYPE(iterator)->tp_iternext(iterator)) == NULL) {
        if (PyIU_ErrorOccurredClearStopIteration()) {
            return NULL;
        }
        if (self->active == self->numactive - 1) {
            /* If the last iterable in the iteratortuple is empty simply set it to
               NULL and reset the active pointer to 0. */
            PyTuple_SET_ITEM(self->iteratortuple, self->active, NULL);
            self->active = 0;
        } else {
            /* Otherwise move each item in the tuple (after the empty iterator) one
               to the left. */
            PyIU_TupleRemove(self->iteratortuple, self->active, self->numactive);
        }
        self->numactive--;
        Py_DECREF(iterator);

        /* End the loop as soon as no active iterators are available or use the
           next iterator.
           */
        if (self->numactive == 0) {
            break;
        } else {
            iterator = PyTuple_GET_ITEM(self->iteratortuple, self->active);
        }
    }

    if (self->numactive == 0) {
        return NULL;
    }

    /* Increment the active pointer (potentially wrapping it around to 0). */
    self->active = (self->active + 1) % (self->numactive);
    return item;
}

static PyObject *
roundrobin_reduce(PyIUObject_Roundrobin *self, PyObject *Py_UNUSED(args)) {
    PyObject *ittuple;
    PyObject *res;

    /* The "next" method modifies the "iteratortuple" so when someone uses
       "reduce" they would see how it changes. So we need to return a copy
       of that tuple here.
       An additional side-effect of "next" is that exhausted iterators are
       removed from the tuple, decremented and finally replaced by NULL at the
       end of the tuple. Python interpreters shouldn't see these NULLs
       (otherwise that might segfault) so we can simply "copy by slicing"
       when there are already exhausted iterators inside.
       */
    if (PyTuple_GET_SIZE(self->iteratortuple) != self->numactive) {
        ittuple = PyIU_TupleGetSlice(self->iteratortuple, self->numactive);
    } else {
        ittuple = PyIU_TupleCopy(self->iteratortuple);
    }
    /* The error handling for both branches. */
    if (ittuple == NULL) {
        return NULL;
    }

    res = Py_BuildValue("OO(nn)", Py_TYPE(self),
                        ittuple,
                        self->numactive,
                        self->active);
    Py_DECREF(ittuple);
    return res;
}

static PyObject *
roundrobin_setstate(PyIUObject_Roundrobin *self, PyObject *state) {
    Py_ssize_t numactive, active;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "nn:roundrobin.__setstate__",
                          &numactive, &active)) {
        return NULL;
    }

    /* active and numactive must be greater than zero, otherwise the "next"
       method could access out-of-bounds indices for the iteratortuple.
       */
    if (active < 0 || numactive < 0) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the first (%zd) and "
                     "second (%zd) argument in the `state` are not negative.",
                     Py_TYPE(self)->tp_name, numactive, active);
        return NULL;
    }
    /* If numactive is not zero than the active must be strictly smaller than
       numactive, otherwise the next "next" call would access an out of bounds
       index of the iteratortuple (or NULL). If "numactive" is zero then
       "active" must be zero as well (in this case it must not be greater but
       equal).
       */
    if (numactive != 0 && active >= numactive) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the first (%zd) "
                     "argument in the `state` is strictly greater than the "
                     "second (%zd) argument, if the first argument isn't zero.",
                     Py_TYPE(self)->tp_name, numactive, active);
        return NULL;
    } else if (numactive == 0 && active != 0) {
        PyErr_Format(PyExc_ValueError,
                     "`%.200s.__setstate__` expected that the second (%zd) "
                     "argument in the `state` is zero if the first "
                     "argument (%zd) argument is zero.",
                     Py_TYPE(self)->tp_name, active, numactive);
        return NULL;
    }

    /* The "numactive" argument must match the number of not-NULL arguments
       in the iteratortuple. Luckily the NULLs are at the end of the
       "iteratortuple".
       */
    if (1) {
        Py_ssize_t tupsize = PyTuple_GET_SIZE(self->iteratortuple);
        /* decrement the tuple size as long as the last item is NULL. */
        while (tupsize > 0 &&
               PyTuple_GET_ITEM(self->iteratortuple, tupsize - 1) == NULL) {
            tupsize--;
        }
        if (numactive != tupsize) {
            PyErr_Format(PyExc_ValueError,
                         "`%.200s.__setstate__` expected that the first "
                         "argument in the `state` (%zd) is equal to the number "
                         "of not exhausted iterators (%zd) in the instance.",
                         Py_TYPE(self)->tp_name, numactive, tupsize);
            return NULL;
        }
    }

    self->numactive = numactive;
    self->active = active;
    Py_RETURN_NONE;
}

static PyObject *
roundrobin_lengthhint(PyIUObject_Roundrobin *self, PyObject *Py_UNUSED(args)) {
    Py_ssize_t i;
    size_t len = 0;

    for (i = 0; i < self->numactive; i++) {
        PyObject *it = PyTuple_GET_ITEM(self->iteratortuple, i);
        Py_ssize_t len_tmp = PyObject_LengthHint(it, 0);

        if (len_tmp == -1) {
            return NULL;
        }

        /* The logic to avoid overflow is the same as in merge. Basically
           adding the current length + next iterator length cannot lead to
           overflow for size_t because we check after each addition if the
           current length goes above py_ssize_t maximum.
           */
        len += (size_t)len_tmp;
        if (len > (size_t)PY_SSIZE_T_MAX) {
            PyErr_SetString(PyExc_OverflowError,
                            "cannot fit 'int' into an index-sized "
                            "integer");
            return NULL;
        }
    }

    return PyLong_FromSsize_t(len);
}

static PyMethodDef roundrobin_methods[] = {
    {
        "__length_hint__",                  /* ml_name */
        (PyCFunction)roundrobin_lengthhint, /* ml_meth */
        METH_NOARGS,                        /* ml_flags */
        PYIU_lenhint_doc                    /* ml_doc */
    },
    {
        "__reduce__",                   /* ml_name */
        (PyCFunction)roundrobin_reduce, /* ml_meth */
        METH_NOARGS,                    /* ml_flags */
        PYIU_reduce_doc                 /* ml_doc */
    },
    {
        "__setstate__",                   /* ml_name */
        (PyCFunction)roundrobin_setstate, /* ml_meth */
        METH_O,                           /* ml_flags */
        PYIU_setstate_doc                 /* ml_doc */
    },
    {NULL, NULL} /* sentinel */
};

PyTypeObject PyIUType_Roundrobin = {
    PyVarObject_HEAD_INIT(NULL, 0)(const char *) "iteration_utilities.roundrobin", /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Roundrobin),                                     /* tp_basicsize */
    (Py_ssize_t)0,                                                                 /* tp_itemsize */
    /* methods */
    (destructor)roundrobin_dealloc,        /* tp_dealloc */
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
    (const char *)roundrobin_doc,      /* tp_doc */
    (traverseproc)roundrobin_traverse, /* tp_traverse */
    (inquiry)roundrobin_clear,         /* tp_clear */
    (richcmpfunc)0,                    /* tp_richcompare */
    (Py_ssize_t)0,                     /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,    /* tp_iter */
    (iternextfunc)roundrobin_next,     /* tp_iternext */
    roundrobin_methods,                /* tp_methods */
    0,                                 /* tp_members */
    0,                                 /* tp_getset */
    0,                                 /* tp_base */
    0,                                 /* tp_dict */
    (descrgetfunc)0,                   /* tp_descr_get */
    (descrsetfunc)0,                   /* tp_descr_set */
    (Py_ssize_t)0,                     /* tp_dictoffset */
    (initproc)0,                       /* tp_init */
    (allocfunc)PyType_GenericAlloc,    /* tp_alloc */
    (newfunc)roundrobin_new,           /* tp_new */
    (freefunc)PyObject_GC_Del,         /* tp_free */
};
