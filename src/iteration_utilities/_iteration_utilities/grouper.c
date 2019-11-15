/******************************************************************************
 * Licensed under Apache License Version 2.0 - see LICENSE
 *****************************************************************************/

#include "grouper.h"
#include "helper.h"
#include "docs_reduce.h"
#include "docs_setstate.h"
#include "docs_lengthhint.h"
#include <structmember.h>

PyDoc_STRVAR(grouper_prop_fillvalue_doc,
    "(any type) The fillvalue if the last group does not contain enough "
    "items (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(grouper_prop_times_doc,
    "(:py:class:`int`) The size of each group (readonly).\n"
    "\n"
    ".. versionadded:: 0.6");
PyDoc_STRVAR(grouper_prop_truncate_doc,
    "(:py:class:`int`) ``True`` if an incomplete last group is discarded "
     "(readonly).\n"
    "\n"
    ".. versionadded:: 0.6");

PyDoc_STRVAR(grouper_doc,
    "grouper(iterable, n, fillvalue=None, truncate=False)\n"
    "--\n\n"
    "Collect data into fixed-length chunks or blocks.\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "iterable : iterable\n"
    "    Any `iterable` to group.\n"
    "\n"
    "n : :py:class:`int`\n"
    "    The number of elements in each chunk.\n"
    "\n"
    "fillvalue : any type, optional\n"
    "    The `fillvalue` if the `iterable` is consumed and the last yielded group\n"
    "    should be filled. If not given the last yielded group may be shorter\n"
    "    than the group before. Using ``fillvalue=None`` is different from not \n"
    "    giving a `fillvalue` in that the last group will be filled with ``None``.\n"
    "\n"
    "truncate : :py:class:`bool`, optional\n"
    "    As alternative to `fillvalue` the last group is discarded if it is\n"
    "    shorter than `n` and `truncate` is ``True``.\n"
    "    Default is ``False``.\n"
    "\n"
    "Raises\n"
    "------\n"
    "TypeError\n"
    "    If `truncate` is ``True`` and a `fillvalue` is given.\n"
    "\n"
    "Returns\n"
    "-------\n"
    "groups : generator\n"
    "    An `iterable` containing the groups/chunks as ``tuple``.\n"
    "\n"
    "Examples\n"
    "--------\n"
    ">>> from iteration_utilities import grouper\n"
    "\n"
    ">>> list(grouper('ABCDEFG', 3))\n"
    "[('A', 'B', 'C'), ('D', 'E', 'F'), ('G',)]\n"
    "\n"
    ">>> list(grouper('ABCDEFG', 3, fillvalue='x'))\n"
    "[('A', 'B', 'C'), ('D', 'E', 'F'), ('G', 'x', 'x')]\n"
    "\n"
    ">>> list(grouper('ABCDEFG', 3, truncate=True))\n"
    "[('A', 'B', 'C'), ('D', 'E', 'F')]\n"
);

/******************************************************************************
 * New
 *****************************************************************************/

static PyObject *
grouper_new(PyTypeObject *type,
            PyObject *args,
            PyObject *kwargs)
{
    static char *kwlist[] = {"iterable", "n", "fillvalue", "truncate", NULL};
    PyIUObject_Grouper *self;

    PyObject *iterable;
    PyObject *iterator = NULL;
    PyObject *fillvalue = NULL;
    Py_ssize_t times;
    int truncate = 0;

    /* Parse arguments */
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "On|Oi:grouper", kwlist,
                                     &iterable, &times,
                                     &fillvalue, &truncate)) {
        goto Fail;
    }
    if (fillvalue != NULL && truncate != 0) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot specify both the `truncate` and the "
                        "`fillvalue` argument for `grouper`.");
        goto Fail;
    }
    if (times <= 0) {
        PyErr_SetString(PyExc_ValueError,
                        "`n` argument for `grouper` must be greater than 0.");
        goto Fail;
    }

    /* Create and fill struct */
    iterator = PyObject_GetIter(iterable);
    if (iterator == NULL) {
        goto Fail;
    }
    self = (PyIUObject_Grouper *)type->tp_alloc(type, 0);
    if (self == NULL) {
        goto Fail;
    }
    Py_XINCREF(fillvalue);
    self->iterator = iterator;
    self->times = times;
    self->fillvalue = fillvalue;
    self->truncate = truncate;
    return (PyObject *)self;

Fail:
    Py_XDECREF(iterator);
    return NULL;
}

/******************************************************************************
 * Destructor
 *****************************************************************************/

static void
grouper_dealloc(PyIUObject_Grouper *self)
{
    PyObject_GC_UnTrack(self);
    Py_XDECREF(self->iterator);
    Py_XDECREF(self->fillvalue);
    Py_TYPE(self)->tp_free(self);
}

/******************************************************************************
 * Traverse
 *****************************************************************************/

static int
grouper_traverse(PyIUObject_Grouper *self,
                 visitproc visit,
                 void *arg)
{
    Py_VISIT(self->iterator);
    Py_VISIT(self->fillvalue);
    return 0;
}

/******************************************************************************
 * Clear
 *****************************************************************************/

static int
grouper_clear(PyIUObject_Grouper *self)
{
    Py_CLEAR(self->iterator);
    Py_CLEAR(self->fillvalue);
    return 0;
}

/******************************************************************************
 * Next
 *****************************************************************************/

static PyObject *
grouper_next(PyIUObject_Grouper *self)
{
    Py_ssize_t idx;

    PyObject *result = PyTuple_New(self->times);
    if (result == NULL) {
        return NULL;
    }

    /* Take the next self->times elements from the iterator.  */
    for (idx=0 ; idx<self->times ; idx++) {
        PyObject *item = Py_TYPE(self->iterator)->tp_iternext(self->iterator);
        if (item != NULL) {
            PyTuple_SET_ITEM(result, idx, item);
        } else {
            if (PyErr_Occurred()) {
                if (PyErr_ExceptionMatches(PyExc_StopIteration)) {
                    PyErr_Clear();
                } else {
                    return NULL;
                }
            }

            if (idx == 0 || self->truncate != 0) {
                /* In case it would be the first element of a new tuple or we
                   truncate the iterator we stop here.
                   */
                Py_DECREF(result);
                return NULL;
            } else if (self->fillvalue != NULL) {
                /* If we want to fill the last group just proceed but use the
                fillvalue as item.
                */
                while (idx < self->times) {
                    Py_INCREF(self->fillvalue);
                    PyTuple_SET_ITEM(result, idx, self->fillvalue);
                    idx++;
                }
            } else {
                /* Otherwise we need a return just the last idx1 items. Because
                idx1 is by definition smaller than self->times we need a new
                tuple to hold the result.
                */
                PyObject *last_result = PyIU_TupleGetSlice(result, idx);
                Py_DECREF(result);
                return last_result;
            }
        }
    }
    return result;
}

/******************************************************************************
 * Reduce
 *****************************************************************************/

static PyObject *
grouper_reduce(PyIUObject_Grouper *self, PyObject *Py_UNUSED(args))
{
    /* Separate cases depending on fillvalue == NULL because otherwise "None"
       would be ambiguous. It could mean that we did not had a fillvalue or
       that the next item was None.
       Better to make an "if" than to introduce another variable depending on
       fillvalue == NULL.
       */
    if (self->fillvalue == NULL) {
        return Py_BuildValue("O(On)(i)", Py_TYPE(self),
                             self->iterator,
                             self->times,
                             self->truncate);
    } else {
        return Py_BuildValue("O(OnO)(i)", Py_TYPE(self),
                             self->iterator,
                             self->times,
                             self->fillvalue,
                             self->truncate);
    }
}

/******************************************************************************
 * Setstate
 *****************************************************************************/

static PyObject *
grouper_setstate(PyIUObject_Grouper *self,
                 PyObject *state)
{
    int truncate;

    if (!PyTuple_Check(state)) {
        PyErr_Format(PyExc_TypeError,
                     "`%.200s.__setstate__` expected a `tuple`-like argument"
                     ", got `%.200s` instead.",
                     Py_TYPE(self)->tp_name, Py_TYPE(state)->tp_name);
        return NULL;
    }

    if (!PyArg_ParseTuple(state, "i:grouper.__setstate__", &truncate)) {
        return NULL;
    }

    /* truncate is just a boolean-like flag so there isn't anything that could
       checked here.
       */

    self->truncate = truncate;
    Py_RETURN_NONE;
}

/******************************************************************************
 * LengthHint
 *****************************************************************************/

static PyObject *
grouper_lengthhint(PyIUObject_Grouper *self, PyObject *Py_UNUSED(args))
{
    Py_ssize_t groups, rem;
    Py_ssize_t len = PyObject_LengthHint(self->iterator, 0);
    if (len == -1) {
        return NULL;
    }
    groups = len / self->times;
    rem = len % self->times;
    if (self->truncate || rem == 0) {
        return PyLong_FromSsize_t(groups);
    } else {
        /* groups + 1 cannot overflow because that could only happen if
           "times" is 1 and in that case "rem==0". So it would always enter the
           first branch which does not contain addition.
           */
        return PyLong_FromSsize_t(groups + 1);
    }
}

/******************************************************************************
 * Type
 *****************************************************************************/

static PyMethodDef grouper_methods[] = {

    {"__length_hint__",                                 /* ml_name */
     (PyCFunction)grouper_lengthhint,                   /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_lenhint_doc                                   /* ml_doc */
     },

    {"__reduce__",                                      /* ml_name */
     (PyCFunction)grouper_reduce,                       /* ml_meth */
     METH_NOARGS,                                       /* ml_flags */
     PYIU_reduce_doc                                    /* ml_doc */
     },

    {"__setstate__",                                    /* ml_name */
     (PyCFunction)grouper_setstate,                     /* ml_meth */
     METH_O,                                            /* ml_flags */
     PYIU_setstate_doc                                  /* ml_doc */
     },

    {NULL, NULL}                                        /* sentinel */
};

#define OFF(x) offsetof(PyIUObject_Grouper, x)
static PyMemberDef grouper_memberlist[] = {

    {"fillvalue",                                       /* name */
     T_OBJECT_EX,                                       /* type */
     OFF(fillvalue),                                    /* offset */
     READONLY,                                          /* flags */
     grouper_prop_fillvalue_doc                         /* doc */
     },

    {"times",                                           /* name */
     T_PYSSIZET,                                        /* type */
     OFF(times),                                        /* offset */
     READONLY,                                          /* flags */
     grouper_prop_times_doc                             /* doc */
     },

    {"truncate",                                        /* name */
     T_BOOL,                                            /* type */
     OFF(truncate),                                     /* offset */
     READONLY,                                          /* flags */
     grouper_prop_truncate_doc                          /* doc */
     },

    {NULL}                                              /* sentinel */
};
#undef OFF

PyTypeObject PyIUType_Grouper = {
    PyVarObject_HEAD_INIT(NULL, 0)
    (const char *)"iteration_utilities.grouper",        /* tp_name */
    (Py_ssize_t)sizeof(PyIUObject_Grouper),             /* tp_basicsize */
    (Py_ssize_t)0,                                      /* tp_itemsize */
    /* methods */
    (destructor)grouper_dealloc,                        /* tp_dealloc */
    (printfunc)0,                                       /* tp_print */
    (getattrfunc)0,                                     /* tp_getattr */
    (setattrfunc)0,                                     /* tp_setattr */
    0,                                                  /* tp_reserved */
    (reprfunc)0,                                        /* tp_repr */
    (PyNumberMethods *)0,                               /* tp_as_number */
    (PySequenceMethods *)0,                             /* tp_as_sequence */
    (PyMappingMethods *)0,                              /* tp_as_mapping */
    (hashfunc)0,                                        /* tp_hash */
    (ternaryfunc)0,                                     /* tp_call */
    (reprfunc)0,                                        /* tp_str */
    (getattrofunc)PyObject_GenericGetAttr,              /* tp_getattro */
    (setattrofunc)0,                                    /* tp_setattro */
    (PyBufferProcs *)0,                                 /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                            /* tp_flags */
    (const char *)grouper_doc,                          /* tp_doc */
    (traverseproc)grouper_traverse,                     /* tp_traverse */
    (inquiry)grouper_clear,                             /* tp_clear */
    (richcmpfunc)0,                                     /* tp_richcompare */
    (Py_ssize_t)0,                                      /* tp_weaklistoffset */
    (getiterfunc)PyObject_SelfIter,                     /* tp_iter */
    (iternextfunc)grouper_next,                         /* tp_iternext */
    grouper_methods,                                    /* tp_methods */
    grouper_memberlist,                                 /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    (descrgetfunc)0,                                    /* tp_descr_get */
    (descrsetfunc)0,                                    /* tp_descr_set */
    (Py_ssize_t)0,                                      /* tp_dictoffset */
    (initproc)0,                                        /* tp_init */
    (allocfunc)PyType_GenericAlloc,                     /* tp_alloc */
    (newfunc)grouper_new,                               /* tp_new */
    (freefunc)PyObject_GC_Del,                          /* tp_free */
};
